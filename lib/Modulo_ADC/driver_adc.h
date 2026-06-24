#ifndef DRIVER_ADC_H
#define DRIVER_ADC_H
#include "driver/adc.h"

#include "esp_err.h"

esp_err_t adc_unit_config(adc_unit_t unit_id);
esp_err_t adc_channel_config(adc_channel_t PIN_ADC);
esp_err_t adc_cali_config(adc_unit_t unit_id, adc_channel_t PIN_ADC);
int adc_leer(adc_channel_t PIN_ADC);


#endif