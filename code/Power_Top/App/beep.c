#include "beep.h"
#include "board_pin_def.h"

void timer_config(void)
{
/* -----------------------------------------------------------------------
    TIMER0 configuration to:
    generate 3 complementary PWM signals with 3 different duty cycles:
    TIMER0CLK is fixed to systemcoreclock,  the TIMER0 prescaler is equal to 3600 so the 
    TIMER0 counter clock used is 20KHz.
    the three duty cycles are computed as the following description: 
    the channel 0 duty cycle is set to 25% so channel 1N is set to 75%.
  ----------------------------------------------------------------------- */
    timer_oc_parameter_struct timer_ocinitpara;
    timer_parameter_struct timer_initpara;

    rcu_periph_clock_enable(RCU_TIMER0);

    timer_deinit(TIMER0);
    /* initialize TIMER init parameter struct */
    timer_struct_para_init(&timer_initpara);
    /* TIMER0 configuration */
    timer_initpara.prescaler         = 71;
    timer_initpara.alignedmode       = TIMER_COUNTER_EDGE;
    timer_initpara.counterdirection  = TIMER_COUNTER_UP;
    timer_initpara.period            = 250;
    timer_initpara.clockdivision     = TIMER_CKDIV_DIV1;
    timer_initpara.repetitioncounter = 0;
    timer_init(TIMER0, &timer_initpara);

    /* initialize TIMER channel output parameter struct */
    timer_channel_output_struct_para_init(&timer_ocinitpara);
    /* CH0, CH1 and CH2 configuration in PWM mode */
    timer_ocinitpara.outputstate  = TIMER_CCX_ENABLE;
    timer_ocinitpara.outputnstate = TIMER_CCXN_ENABLE;
    timer_ocinitpara.ocpolarity   = TIMER_OC_POLARITY_HIGH;
    timer_ocinitpara.ocnpolarity  = TIMER_OCN_POLARITY_HIGH;
    timer_ocinitpara.ocidlestate  = TIMER_OC_IDLE_STATE_LOW;
    timer_ocinitpara.ocnidlestate = TIMER_OCN_IDLE_STATE_LOW;
    timer_channel_output_config(TIMER0, TIMER_CH_0, &timer_ocinitpara);


    /* configure TIMER channel 0 output pulse value */
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, 175);
    /* configure TIMER channel 0 PWM0 mode */
    timer_channel_output_mode_config(TIMER0, TIMER_CH_0, TIMER_OC_MODE_PWM0);
    /* disable TIMER channel output shadow function */
    timer_channel_output_shadow_config(TIMER0, TIMER_CH_0, TIMER_OC_SHADOW_DISABLE);

    /* enable TIMER primary output function */
    timer_primary_output_config(TIMER0, ENABLE);

    /* auto-reload preload enable */
    timer_auto_reload_shadow_enable(TIMER0);
    /* enable a TIMER */
    timer_enable(TIMER0);
}

// beep 无源 pwm  4000hz 50%占空比 TIME0 CH0   AF2
void beep_init()
{

    rcu_periph_clock_enable(RCU_GPIOA);


    gpio_mode_set(BEEP_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_NONE, BEEP_PIN);
    gpio_output_options_set(BEEP_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ,BEEP_PIN);
    gpio_af_set(BEEP_GPIO_PORT, GPIO_AF_2, BEEP_PIN);


    timer_config();
    timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, 0);
}

void beep_switch(uint8_t on_off)
{
    if(on_off)
    {
        timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, 175);
    }
    else
    {
        timer_channel_output_pulse_value_config(TIMER0, TIMER_CH_0, 0);
    }
}
static uint8_t beep_flag = 0;
void beep()
{
    beep_flag = 1;
}

void beep_loop()
{
    static uint8_t beep_last = 0;
    static uint8_t beep_cnt = 0;

    if(beep_last == beep_flag) return;
    if(beep_flag)
    {
        beep_flag = 0;
        beep_switch(1);
        beep_last = 1;
    }
    else
    {
        beep_cnt++;
        if(beep_cnt<5) return;
        beep_cnt = 0;

        beep_switch(0);
        beep_last = 0;
    }

}
