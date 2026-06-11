#include "driver_adc.h"
#include "esp_err.h"
#include "stdlib.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"

adc_cali_handle_t adc_cali_handle = NULL;
adc_oneshot_unit_handle_t adc_handle;
#define TAG "ADC_DRIVER"

esp_err_t adc_unit_config(adc_unit_t unit_id) {
    adc_oneshot_unit_init_cfg_t adc_config = {
        .unit_id = unit_id,
        .ulp_mode = ADC_ULP_MODE_DISABLE,
    };
    esp_err_t ret = adc_oneshot_new_unit(&adc_config, &adc_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error al inicializar la unidad ADC %d", unit_id);
    }
    return ret;
}

esp_err_t adc_channel_config(adc_channel_t PIN_ADC) {
    adc_oneshot_chan_cfg_t channel_config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };
    esp_err_t ret = adc_oneshot_config_channel(adc_handle,PIN_ADC,&channel_config);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error al configurar el canal ADC %d", PIN_ADC);
    }
    return ret;
}

esp_err_t adc_cali_config(adc_channel_t PIN_ADC) {
    adc_cali_line_fitting_config_t cali_config = {
        .unit_id = ADC_UNIT_1,
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_DEFAULT,
    };

    esp_err_t ret = adc_cali_create_scheme_line_fitting(&cali_config,&adc_cali_handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error al crear el esquema de calibración ADC %d", PIN_ADC);
    }
    return ret;
}

int adc_leer(adc_channel_t PIN_ADC) {
    int adc_value;
    esp_err_t ret = adc_oneshot_read(adc_handle, PIN_ADC, &adc_value);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error al leer el valor ADC del canal %d", PIN_ADC);
        return -1; // Retorna -1 en caso de error
    }
    return adc_value;
}
