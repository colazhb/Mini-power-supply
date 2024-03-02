#ifndef __ADC_CONV_H
#define __ADC_CONV_H

#include "gd32e23x.h"

enum
{
    ADC_TYPE_TEMP,
    ADC_TYPE_VIN,
    ADC_TYPE_MAX
};

void adc_conv_init();
void adc_conv_loop();

#endif