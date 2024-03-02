
#include "board_pin_def.h"
#include "uart_com.h"
#include "frame_ctl_pwb.h"
#include "frame.h"
#include "gd32e23x.h"
#include "shell_user.h"
// #if defined(__GNUC__)
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
// #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
// #else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
// #endif /* __GNUC__ */
PUTCHAR_PROTOTYPE
{
    usart_data_transmit(USART0, (uint8_t)ch);
    while (RESET == usart_flag_get(USART0, USART_FLAG_TBE))
        ;
    return ch;
}

void uart_dbg_init()
{

    /* enable COM GPIO clock */
    rcu_periph_clock_enable(RCU_GPIOA);

    /* enable USART clock */
    rcu_periph_clock_enable(RCU_USART0);

    /* connect port to USARTx_Tx */
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_9);

    /* connect port to USARTx_Rx */
    gpio_af_set(GPIOA, GPIO_AF_1, GPIO_PIN_10);

    /* configure USART Tx as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_9);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_9);

    /* configure USART Rx as alternate function push-pull */
    gpio_mode_set(GPIOA, GPIO_MODE_AF, GPIO_PUPD_PULLUP, GPIO_PIN_10);
    gpio_output_options_set(GPIOA, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, GPIO_PIN_10);

    /* USART configure */
    usart_deinit(USART0);
    usart_baudrate_set(USART0, 115200U);
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);

    usart_enable(USART0);

    usart_interrupt_enable(USART0, USART_INT_RBNE);

    nvic_irq_enable(USART0_IRQn, 1);

}

void USART0_IRQHandler(void)
{
    uint8_t ch = 0;
    if (RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_RBNE))
    {
        /* receive data */
        ch = usart_data_receive(USART0);
        shell_user_put_char(ch);

        // 清除
    }
    usart_interrupt_flag_clear(USART0, USART_INT_FLAG_TBE);
}

// 串口通讯- 与底部板通讯

void uart_com_init()
{
    /* enable COM GPIO clock */
    rcu_periph_clock_enable(GPIOA);

    /* enable USART clock */
    rcu_periph_clock_enable(RCU_USART1);

    /* connect port to USARTx_Tx */
    gpio_af_set(COM_TX_GPIO_PORT, GPIO_AF_1, COM_TX_PIN);

    /* connect port to USARTx_Rx */
    gpio_af_set(COM_RX_GPIO_PORT, GPIO_AF_1, COM_RX_PIN);

    /* configure USART Tx as alternate function push-pull */
    gpio_mode_set(COM_TX_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, COM_TX_PIN);
    gpio_output_options_set(COM_TX_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, COM_TX_PIN);

    /* configure USART Rx as alternate function push-pull */
    gpio_mode_set(COM_RX_GPIO_PORT, GPIO_MODE_AF, GPIO_PUPD_PULLUP, COM_RX_PIN);
    gpio_output_options_set(COM_RX_GPIO_PORT, GPIO_OTYPE_PP, GPIO_OSPEED_10MHZ, COM_RX_PIN);

    /* USART configure */
    usart_deinit(USART1);
    usart_baudrate_set(USART1, 115200U);
    usart_receive_config(USART1, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);

    usart_enable(USART1);

    usart_interrupt_enable(USART1, USART_INT_RBNE);

    nvic_irq_enable(USART1_IRQn, 0);
}

void com_put_char(uint8_t ch)
{
    usart_data_transmit(USART1, ch);
    while (RESET == usart_flag_get(USART1, USART_FLAG_TBE))
        ;
}

void USART1_IRQHandler(void)
{
    uint8_t ch = 0;
    if (RESET != usart_interrupt_flag_get(USART1, USART_INT_FLAG_RBNE))
    {
        /* receive data */
        ch = usart_data_receive(USART1);
        // pwb_ch_put(ch);
        frame_push_ch(&frame_pwb, ch);
        // 清除
    }
    usart_interrupt_flag_clear(USART1, USART_INT_FLAG_TBE);
}
