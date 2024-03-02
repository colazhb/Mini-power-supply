#include "led.h"
#include "board_pin_def.h"

void led_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOA);

    gpio_mode_set(MCU_RUN_GPIO_PORT, GPIO_MODE_OUTPUT, GPIO_PUPD_NONE, MCU_RUN_PIN);
    gpio_output_options_set(MCU_RUN_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_50MHZ, MCU_RUN_PIN);

    gpio_bit_set(MCU_RUN_GPIO_PORT, MCU_RUN_PIN);

    gpio_bit_reset(MCU_RUN_GPIO_PORT, MCU_RUN_PIN);
}
void led_blink()
{
    gpio_bit_toggle(MCU_RUN_GPIO_PORT, MCU_RUN_PIN);
}