#include "esp_err.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "servicio_control_fase.h"
#include <inttypes.h>

#define CONTROL_FASE_ADC_UNIT_EJEMPLO ADC_UNIT_1
#define CONTROL_FASE_ADC_CHANNEL_EJEMPLO ADC_CHANNEL_0
#define CONTROL_FASE_TRIAC_PIN_EJEMPLO GPIO_NUM_NC

static const char *TAG = "MAIN";

/**
 * @brief Tarea de ejemplo que imprime el estado del diagnostico de frecuencia.
 * @param pvParameters Parametro de tarea, no usado.
 */
static void tarea_monitoreo_control_fase(void *pvParameters)
{
	(void) pvParameters;

	for (;;) {
		control_fase_diagnostico_estado_t estado;
		esp_err_t ret = control_fase_obtener_estado(&estado);
		if (ret == ESP_OK) {
			ESP_LOGI(
				TAG,
				"control_fase activo=%d señal=%d rango=%d eventos=%" PRIu32 " raw=%d freq_evento=%.2f Hz freq_red=%.2f Hz periodo=%" PRIu64 " us errores_adc=%" PRIu32,
				estado.activo,
				estado.senal_presente,
				estado.dentro_rango,
				estado.eventos_detectados,
				estado.ultima_muestra_raw,
				estado.frecuencia_evento_hz,
				estado.frecuencia_red_hz,
				estado.periodo_evento_us,
				estado.errores_adc);
		} else {
			ESP_LOGE(TAG, "No se pudo leer el estado del diagnostico: %s", esp_err_to_name(ret));
		}

		vTaskDelay(pdMS_TO_TICKS(1000));
	}
}

void app_main(void)
{
	control_fase_diagnostico_config_t config = {
		.adc_unit = CONTROL_FASE_ADC_UNIT_EJEMPLO,
		.adc_channel = CONTROL_FASE_ADC_CHANNEL_EJEMPLO,
		.pin_triac = CONTROL_FASE_TRIAC_PIN_EJEMPLO,
		.sample_period_us = 500U,
		.umbral_bajo_raw = 200,
		.umbral_alto_raw = 800,
		.frecuencia_minima_hz = 50U,
		.frecuencia_maxima_hz = 70U,
		.pulsos_por_ciclo = 2U,
	};

	esp_err_t ret = control_fase_init(&config);
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "Fallo la inicializacion del control de fase: %s", esp_err_to_name(ret));
		return;
	}

	ESP_LOGI(TAG, "Control de fase inicializado. Esperando cruces por cero activos en nivel alto por ADC1_CH0.");

	ret = control_fase_iniciar_diagnostico();
	if (ret != ESP_OK) {
		ESP_LOGE(TAG, "No fue posible iniciar el diagnostico: %s", esp_err_to_name(ret));
		return;
	}

	ESP_LOGI(TAG, "Diagnostico de frecuencia activo. Revisa los logs para ver la frecuencia estimada.");

	xTaskCreate(
		tarea_monitoreo_control_fase,
		"mon_ctrl_fase",
		4096,
		NULL,
		5,
		NULL);
}
