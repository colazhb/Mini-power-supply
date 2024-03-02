#include "key.h"
#include "board_pin_def.h"
#include "multi_button.h"
#include "power_data.h"
#include "led_rgb.h"
static uint16_t ec_curr_value = 0;
static uint16_t ec_last_value = 0;
static int16_t ec_diff = 0;
//extern struct power_data_t power_data;

#define EC_TIM_COUNT 0x3fff

#define DEF_MAX_VOL 30 // 最大电压
#define DEF_MIN_VOL 3  // 最小电压
#define DEF_MAX_CUR 5  // 最大电流
#define DEF_MIN_CUR 1  // 最小电流

// 模式切换 显示<-->设置
void mode_switch()
{
    if (power_data.gui_mode == 1)
    {
        timer_counter_value_config(TIMER2, EC_TIM_COUNT);
        power_data.gui_set_mode_id = 0;
        power_data.gui_mode = 2;
        // 
    }
    else if (power_data.gui_mode == 2) // 设置模式
    {
        power_data.gui_mode = 0;
    }
    power_data.set_mode_change_flag = 1;
    conv_val_to_set_val();
    beep();
}

// 电源输出  打开<-->关闭
void power_out_switch()
{
    if (power_data.switch_status)
    {
        power_data.switch_status = 0;
        printf("power off\r\n");
        led_led_on(LED_MAX);  // 关闭
    }
    else
    {
        //发送设置的值
        pwb_set_mode(power_data.cc_cv, power_data.set_voltage, power_data.set_current);

        power_data.switch_status = 1;
        printf("power on\r\n");
        led_led_on(LED_G);
    }
    power_data.gui_mode = 0;
    beep();
    pwb_set_output(power_data.switch_status);
}

// 设置内容切换 CC-CV  <--> 电压  <----> 电流
void set_type_switch()
{
    power_data.gui_set_mode_id++;
    power_data.gui_set_mode_id %= 9;
}

// void set_cc_cv_switch()
// {
//     power_data.cc_cv = !power_data.cc_cv;
// }

// BT1 ,BT2,SW
void key_init(void)
{
    // KEY1_PIN
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    gpio_mode_set(KEY1_GPIO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, KEY1_PIN);
    gpio_mode_set(KEY2_GPIO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, KEY2_PIN);
    gpio_mode_set(EC_SW_GPIO_PORT, GPIO_MODE_INPUT, GPIO_PUPD_PULLUP, EC_SW_PIN);
}

// 旋转编码器 EC_A PB5,EC_B PB4,EC_SW PB3
// A:TIME2 CH1  B:TIME2 CH0
void ec_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOB);

    // A B 正交译码
    gpio_af_set(EC_A_GPIO_PORT, GPIO_AF_1, EC_A_PIN);
    gpio_af_set(EC_B_GPIO_PORT, GPIO_AF_1, EC_B_PIN);

    gpio_mode_set(EC_A_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, EC_A_PIN);
    gpio_mode_set(EC_B_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, EC_B_PIN);

    // TIMER 设置
    rcu_periph_clock_enable(RCU_TIMER2);

    timer_parameter_struct timer_initpara;
    timer_initpara.prescaler = 0; // 注意 要0
    timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection = TIMER_COUNTER_UP;
    timer_initpara.period = 0xffff;
    timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER2, &timer_initpara);

    // NVIC
    // nvic_irq_enable(TIMER2_IRQn, 0);

    timer_update_source_config(TIMER2, TIMER_UPDATE_SRC_REGULAR);

    timer_quadrature_decoder_mode_config(TIMER2, TIMER_QUAD_DECODER_MODE2, TIMER_IC_POLARITY_FALLING, TIMER_IC_POLARITY_RISING);
    timer_interrupt_enable(TIMER2, TIMER_INT_UP);

    // timer_master_slave_mode_config(TIMER2, TIMER_MASTER_SLAVE_MODE_ENABLE);
    //  auto-reload preload enable
    timer_auto_reload_shadow_enable(TIMER2);

    // 初始值
    timer_counter_value_config(TIMER2, EC_TIM_COUNT);

    ec_last_value = 0xff;
    timer_enable(TIMER2);
}
// timer2 interrupt
void TIMER2_IRQHandler(void)
{
    if (SET == timer_interrupt_flag_get(TIMER2, TIMER_INT_UP))
    {
        timer_interrupt_flag_clear(TIMER2, TIMER_INT_UP);
        // printf("timer2 interrupt\r\n");
    }
}

// TIME5 用来定时计算编码器间隔,算出加减速度  定时中断时间 20ms
void ec_cal_timer_init()
{
    rcu_periph_clock_enable(RCU_TIMER5);
    timer_parameter_struct timer_initpara;
    timer_initpara.prescaler = 7200 - 1; // 72M/7200=10K
    timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection = TIMER_COUNTER_UP;
    timer_initpara.period = 100 - 1; // 10ms
    timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER5, &timer_initpara);

    timer_update_source_config(TIMER5, TIMER_UPDATE_SRC_REGULAR);

    timer_interrupt_enable(TIMER5, TIMER_INT_UP);

    timer_enable(TIMER5);

    nvic_irq_enable(TIMER5_IRQn, 0);
}
struct ec_key
{
    uint32_t last_val;
    int8_t ec_val;
    /* data */
};

int16_t ec_val = 0;
int16_t temp_val = 0;
void ec_add_val(int val)
{
    int8_t temp_val;
    if (power_data.gui_mode != 2)
        return;

    if(val == 0) return; // 滚轮没有动作
    
    // mode 切换后清零,解决切换后滚轮值突变问题
    if(power_data.set_mode_change_flag)
    {
        power_data.set_mode_change_flag = 0;
        return;
    }

    //printf("ec_val:%d\r\n", val);

    conv_val_to_set_val();
    switch (power_data.gui_set_mode_id)
    {
    case 0: // cc cv
   
        //     set_cc_cv_switch();
        
        // break;
    case 2:
    

            temp_val = power_data.set_v_h;
            temp_val += val;
            if (temp_val >= DEF_MAX_VOL)
                temp_val = DEF_MAX_VOL;
            if (temp_val <= DEF_MIN_VOL)
                temp_val = DEF_MIN_VOL;

            power_data.set_v_h = temp_val;
            power_data.set_voltage = power_data.set_v_h * 1000 + power_data.set_v_l*100;
           
       
        break;

    case 4:
 
            temp_val = power_data.set_v_l;
            temp_val += val;
            if (temp_val > 9)
                temp_val = 9;
            if (temp_val <= 0)
                temp_val = 0;

            power_data.set_v_l = temp_val;
            power_data.set_voltage = power_data.set_v_h * 1000 + power_data.set_v_l*100;
            
        
        break;

    case 6:


            temp_val = power_data.set_i_h;
            temp_val += val;
            if (temp_val >= DEF_MAX_CUR)
                temp_val = DEF_MAX_CUR;
            if (temp_val <= DEF_MIN_CUR)
                temp_val = DEF_MIN_CUR;

            power_data.set_i_h = temp_val;
            power_data.set_current = power_data.set_i_h * 1000 + power_data.set_i_l*100;
            
        
        break;

    case 8:
 
            temp_val = power_data.set_i_l;
            temp_val += val;
            if (temp_val > 9)
                temp_val = 9;
            if (temp_val <= 0)
                temp_val = 0;

            power_data.set_i_l = temp_val;
            power_data.set_current = power_data.set_i_h * 1000 + power_data.set_i_l*100;
           
        
        break;

    default:
        break;
    }
    //printf("set voltage:%d set current:%d\r\n", power_data.set_voltage, power_data.set_current);
    // 如果开关是打开状态,一直发送设置的值
    if(power_data.switch_status)
    {
        //printf("set voltage:%d set current:%d\r\n", power_data.set_voltage, power_data.set_current);
        //发送设置的值
        pwb_set_mode(power_data.cc_cv, power_data.set_voltage, power_data.set_current);
    }
    
 
}
// TIME5 interrupt
void TIMER5_IRQHandler(void)
{
  
    if (SET == timer_interrupt_flag_get(TIMER5, TIMER_INT_UP))
    {
        timer_interrupt_flag_clear(TIMER5, TIMER_INT_UP);
        ec_val = timer_counter_read(TIMER2);

        ec_curr_value = ec_val /= 4;
        ec_diff = ec_last_value - ec_curr_value;
        ec_last_value = ec_curr_value;

        ec_add_val(ec_diff);
        button_ticks();
        beep_loop();


    }
}

uint8_t ec_val_get()
{
    return ec_val;
}

// 打印编码器值
void ec_print(void)
{
    if (ec_diff != 0)
        printf("ec:%d\r\n", ec_diff);
}

/*------------------------------- */

static struct button btn_a;
static struct button btn_b;
static struct button btn_ec;

static uint8_t button_a_read_pin(void)
{
    return gpio_input_bit_get(KEY1_GPIO_PORT, KEY1_PIN);
}

static uint8_t button_b_read_pin(void)
{
    return gpio_input_bit_get(KEY2_GPIO_PORT, KEY2_PIN);
}

static uint8_t button_ec_read_pin(void)
{
    return gpio_input_bit_get(EC_SW_GPIO_PORT, EC_SW_PIN);
}

void button_a_callback(void *btn)
{
    uint32_t btn_event_val;

    btn_event_val = get_button_event((struct button *)btn);

    switch (btn_event_val)
    {
    case SINGLE_CLICK:

        break;

    case LONG_PRESS_START:
        power_out_switch();

        break;
    }
}

void button_b_callback(void *btn)
{
    uint32_t btn_event_val;

    btn_event_val = get_button_event((struct button *)btn);

    switch (btn_event_val)
    {
    case SINGLE_CLICK:

        break;

    case LONG_PRESS_START:
        mode_switch();
        break;
    }
}

void button_ec_callback(void *btn)
{
    uint32_t btn_event_val;

    btn_event_val = get_button_event((struct button *)btn);

    switch (btn_event_val)
    {
    case SINGLE_CLICK:
        // 切换下一个设置内容
        set_type_switch();

        break;

    case LONG_PRESS_START:
        // 设置结束或进入设置
        mode_switch();
        break;
    }
}


int multi_button_init(void)
{

    /* low level drive */
    button_init(&btn_a, button_a_read_pin, 0);
    button_attach(&btn_a, SINGLE_CLICK, button_a_callback);
    button_attach(&btn_a, LONG_PRESS_START, button_a_callback);
    button_start(&btn_a);

    button_init(&btn_b, button_b_read_pin, 0);
    button_attach(&btn_b, SINGLE_CLICK, button_b_callback);
    button_attach(&btn_b, LONG_PRESS_START, button_b_callback);
    button_start(&btn_b);

    button_init(&btn_ec, button_ec_read_pin, 0);
    button_attach(&btn_ec, SINGLE_CLICK, button_ec_callback);
    button_attach(&btn_ec, LONG_PRESS_START, button_ec_callback);
    button_start(&btn_ec);


};
