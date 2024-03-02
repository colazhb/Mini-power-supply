#include "led_rgb.h"
#include "board_pin_def.h"
void led_rgb_init(void)
{
    /* enable the led clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    rcu_periph_clock_enable(RCU_GPIOB);
    /* configure led GPIO port */ 
  
    gpio_mode_set(LED_R_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_R_PIN);
    gpio_output_options_set(LED_R_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LED_R_PIN);

    gpio_mode_set(LED_G_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_G_PIN);
    gpio_output_options_set(LED_G_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LED_G_PIN);

    gpio_mode_set(LED_B_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LED_B_PIN);
    gpio_output_options_set(LED_B_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LED_B_PIN);

    /* turn off leds */
    gpio_bit_set(LED_R_GPIO_PORT, LED_R_PIN);
    gpio_bit_set(LED_G_GPIO_PORT, LED_G_PIN);
    gpio_bit_set(LED_B_GPIO_PORT, LED_B_PIN);

   //  gpio_mode_set(LCD_PWM_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, LCD_PWM_PIN);
   // gpio_output_options_set(LCD_PWM_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, LCD_PWM_PIN);

    // gpio_bit_set(LCD_PWM_GPIO_PORT, LCD_PWM_PIN);
    led_led_on(LED_R);
    delay_1ms(50);
     led_led_on(LED_G);
    delay_1ms(50);
     led_led_on(LED_B);
    // 关闭
    delay_1ms(50);
    led_led_on(LED_MAX);
}

void led_led_on(uint8_t id)
{
    switch (id)
    {
    case 0:
        led_rgb_switch(LED_R, 0);
        led_rgb_switch(LED_G, 1);
        led_rgb_switch(LED_B, 1);

        break;
    case 1:
        led_rgb_switch(LED_R, 1);
        led_rgb_switch(LED_G, 0);
        led_rgb_switch(LED_B, 1);
        break;
    case 2:
        led_rgb_switch(LED_R, 1);
        led_rgb_switch(LED_G, 1);
        led_rgb_switch(LED_B, 0);
        break;
    default:
        led_rgb_switch(LED_R, 1);
        led_rgb_switch(LED_G, 1);
        led_rgb_switch(LED_B, 1);

        break;

    }
}
void led_rgb_switch(uint8_t id, uint8_t on_off)
{
    switch(id)
    {
        case 0:
            gpio_bit_write(LED_R_GPIO_PORT, LED_R_PIN, on_off);            
            break;
        case 1:
            gpio_bit_write(LED_G_GPIO_PORT, LED_G_PIN, on_off);
            break;
        case 2:
            gpio_bit_write(LED_B_GPIO_PORT, LED_B_PIN, on_off);
            break;
        default:
            break;
    }
}