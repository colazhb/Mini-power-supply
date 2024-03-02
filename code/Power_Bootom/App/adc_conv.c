#include "adc_conv.h"
#include "board_pin_def.h"
#include "gd32e23x_libopt.h"
#include "power_data.h"
#include <math.h>

/*
    // 温度采样
    PA4 TEMP_ADC

    // 电源输入
    PA5 VIN_ADC
*/
uint16_t adc_value[4];


void gpio_config(void)
{
    /* config the GPIO as analog mode */
    gpio_mode_set(GPIOA, GPIO_MODE_ANALOG, GPIO_PUPD_NONE, TEMP_ADC_PIN|VIN_ADC_PIN);
}


void dma_config(void)
{
    /* ADC_DMA_channel configuration */
    dma_parameter_struct dma_data_parameter;
    
    /* ADC DMA_channel configuration */
    dma_deinit(DMA_CH0);

    /* initialize DMA single data mode */
    dma_data_parameter.periph_addr  = (uint32_t)(&ADC_RDATA);
    dma_data_parameter.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
    dma_data_parameter.memory_addr  = (uint32_t)(&adc_value);
    dma_data_parameter.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
    dma_data_parameter.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
    dma_data_parameter.memory_width = DMA_MEMORY_WIDTH_16BIT;  
    dma_data_parameter.direction    = DMA_PERIPHERAL_TO_MEMORY;
    dma_data_parameter.number       = 2U;
    dma_data_parameter.priority     = DMA_PRIORITY_HIGH;
    dma_init(DMA_CH0, &dma_data_parameter);

    dma_circulation_enable(DMA_CH0);
  
    /* enable DMA channel */
    dma_channel_enable(DMA_CH0);
}

void adc_config(void)
{
    /* ADC continuous function enable */
    adc_special_function_config(ADC_CONTINUOUS_MODE, ENABLE);
    /* ADC scan function enable */
    adc_special_function_config(ADC_SCAN_MODE, ENABLE);
    /* ADC data alignment config */
    adc_data_alignment_config(ADC_DATAALIGN_RIGHT);
    /* ADC channel length config */
    adc_channel_length_config(ADC_REGULAR_CHANNEL, 2U);
 
    /* ADC regular channel config */
    adc_regular_channel_config(0, ADC_CHANNEL_4, ADC_SAMPLETIME_55POINT5);
    adc_regular_channel_config(1, ADC_CHANNEL_5, ADC_SAMPLETIME_55POINT5);

    /* ADC trigger config */
    adc_external_trigger_source_config(ADC_REGULAR_CHANNEL, ADC_EXTTRIG_REGULAR_NONE); 
    adc_external_trigger_config(ADC_REGULAR_CHANNEL, ENABLE);

    /* enable ADC interface */
    adc_enable();
    delay_1ms(1U);
    /* ADC calibration and reset calibration */
    adc_calibration_enable();

    /* ADC DMA function enable */
    adc_dma_mode_enable();
    /* ADC software trigger enable */
    adc_software_trigger_enable(ADC_REGULAR_CHANNEL);
}
void adc_conv_init()
{
    /* enable GPIOC clock */
    rcu_periph_clock_enable(RCU_GPIOA);
    /* enable ADC clock */
    rcu_periph_clock_enable(RCU_ADC);
    /* enable DMA clock */
    rcu_periph_clock_enable(RCU_DMA);
    /* config ADC clock */
    rcu_adc_clock_config(RCU_ADCCK_APB2_DIV6);

    /* GPIO config */
    gpio_config();

    /* DMA config */
    dma_config();

    /* ADC config */
    adc_config();

}
const float BETA = 3950; // should match the Beta Coefficient of the thermistor
float NTC3950_Temp(uint16_t adc_in)
{

    float temp = 1 / (logf(1 / (4095. / adc_in - 1)) / BETA + 1.0 / 298.15) - 273.15;
    
    return temp;
}
uint16_t adc_conv_cal(uint8_t type)
{
    int16_t ret = 0;
    switch(type)
    {
        case ADC_TYPE_TEMP: 
            return  NTC3950_Temp(adc_value[0]);
            break;
        case ADC_TYPE_VIN:
                // 分压电阻 ra:120k  rb:6.8k
            ret = (int16_t)(adc_value[1] * 3300 / 4096) * 18.69;
            break;


        default:
            break;
    }
    return ret;

}
uint8_t dbg_adc_print_flag = 0;
void adc_conv_print()
{
    dbg_adc_print_flag = !dbg_adc_print_flag;
}
void adc_conv_loop()
{
   

    power_data.ntc_temp = adc_conv_cal(ADC_TYPE_TEMP);
    power_data.input_voltage = adc_conv_cal(ADC_TYPE_VIN);

    if(dbg_adc_print_flag)
    {
        printf("\r\nadc_temp:%d adc_vin:%d",power_data.ntc_temp, power_data.input_voltage);
    }
}


