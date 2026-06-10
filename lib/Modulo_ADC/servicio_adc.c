#include "driver_adc.h"
#include "esp_err.h"
#include "freertos/semphr.h"
#include "servicio_adc.h"

int adc_leer(adc_channel_t PIN_ADC) {
    int adc_value;
    esp_err_t ret = adc_oneshot_read(adc_handle, PIN_ADC, &adc_value);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error al leer el valor ADC del canal %d", PIN_ADC);
        return -1; // Retorna -1 en caso de error
    }
    return adc_value;
}



