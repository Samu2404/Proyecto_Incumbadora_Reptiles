#include "driver_timer.h"
#include "driver_control_fase.h"
#include "servicio_adc.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "driver/gptimer.h"


#define CONTROL_FASE_SAMPLE_PERIOD_US_DEFAULT 500U
#define CONTROL_FASE_UMBRAL_BAJO_DEFAULT      200
#define CONTROL_FASE_UMBRAL_ALTO_DEFAULT      800
#define CONTROL_FASE_FRECUENCIA_MIN_DEFAULT   50U
#define CONTROL_FASE_FRECUENCIA_MAX_DEFAULT   70U
#define CONTROL_FASE_PULSOS_POR_CICLO_DEFAULT 2U
#define CONTROL_FASE_TASK_STACK               4096U
#define CONTROL_FASE_TASK_PRIORITY            5U
#define CONTROL_FASE_TIMEOUT_CICLOS           4U


gptimer_handle_t phase_control_timer = NULL;
static bool control_fase_isr ()

esp_err_t phase_control_init (gpio_num_t triac_pin, adc_unit_t adc_unit, adc_channel_t adc_channel) {
    ESP_ERROR_CHECK(salida_gpio_config (triac_pin));
    ESP_ERROR_CHECK(servicio_adc_configurar(adc_unit, adc_channel));
    ESP_ERROR_CHECK(timer_config(&phase_control_timer, 80, true, true, 500)); // Configura el timer para generar una interrupción cada 500 us
    return ESP_OK;
}

