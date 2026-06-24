#include "driver_adc.h"
#include "esp_err.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_adc/adc_cali.h"
#include "esp_adc/adc_cali_scheme.h"
#include "servicio_adc.h"


/**
 * @brief Configura el ADC para el canal indicado y deja lista la calibracion.
 * @param unit_id Unidad ADC a utilizar.
 * @param channel Canal ADC a configurar.
 * @return ESP_OK si la configuracion fue correcta, o un codigo de error en caso contrario.
 */
esp_err_t servicio_adc_configurar(adc_unit_t unit_id, adc_channel_t channel){
	esp_err_t ret = adc_unit_config(unit_id);
	if (ret != ESP_OK) {
		return ret;
	};

	ret = adc_channel_config(channel);
	if (ret != ESP_OK) {
		return ret;
	};

	return adc_cali_config(unit_id, channel);
}

/**
 * @brief Lee una muestra cruda del canal ADC configurado.
 * @param channel Canal ADC a leer.
 * @param value Puntero donde se almacenara la lectura.
 * @return ESP_OK si la lectura fue correcta, o un codigo de error en caso contrario.
 */
esp_err_t servicio_adc_leer(adc_channel_t channel, int *value)
{
	if (value == NULL) {
		return ESP_ERR_INVALID_ARG;
	}

	int lectura = adc_leer(channel);
	if (lectura < 0) {
		return ESP_FAIL;
	}

	*value = lectura;
	return ESP_OK;
}
