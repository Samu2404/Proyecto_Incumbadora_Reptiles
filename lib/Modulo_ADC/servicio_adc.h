#ifndef SERVICIO_ADC_H
#define SERVICIO_ADC_H

#include "driver/adc.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "esp_err.h"

/**
 * @brief Configura la unidad ADC, el canal y la calibracion basica.
 * @param unit_id Unidad ADC a configurar.
 * @param channel Canal ADC a configurar.
 * @return ESP_OK si la configuracion fue correcta, o un codigo de error en caso contrario.
 */
esp_err_t servicio_adc_configurar(adc_unit_t unit_id, adc_channel_t channel);

/**
 * @brief Lee una muestra del canal ADC configurado.
 * @param channel Canal ADC a leer.
 * @param value Puntero donde se almacenara la lectura cruda.
 * @return ESP_OK si la lectura fue correcta, o un codigo de error en caso contrario.
 */
esp_err_t servicio_adc_leer(adc_channel_t channel, int *value);

#endif