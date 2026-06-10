#include "driver_timer.h"
#include "driver/gptimer.h"
#include "esp_err.h"
#include "stdbool.h"
#include "stdint.h"
/** 
* @brief Función para configurar el timer
* @param timer_id identificador del timer a configurar
* @param divider valor del divisor para ajustar la resolución del timer
* @param auto_reload true para habilitar auto-reload, false para deshabilitar
* @param alarm_en true para habilitar alarma, false para deshabilitar
* @param intr_en true para habilitar interrupción, false para deshabilitar
* @param alarm_value valor de la alarma en microsegundos
*/
void timer_config (gptimer_handle_t *timer_id, int divider, bool auto_reload, bool alarm_en, uint64_t alarm_value) {
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 80000000 / divider, // Resolución a partir del divisor
    };

    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, timer_id));

    if (alarm_en) {
        gptimer_alarm_config_t alarm_config = {
            .alarm_count = alarm_value,
            .flags.auto_reload_on_alarm = auto_reload,
        };
        ESP_ERROR_CHECK(gptimer_set_alarm_action(*timer_id, &alarm_config));
    };
    ESP_ERROR_CHECK (gptimer_enable(*timer_id));
    ESP_ERROR_CHECK(gptimer_start(*timer_id));
}

/**
 * @brief Función para registrar una ISR para el timer
 * @param timer_id identificador del timer para el cual se registra la ISR
 * @param isr_callback función de callback que se ejecutará cuando se active la alarma del timer
 * @param args puntero a datos de usuario que se pasarán al callback
 * @return ESP_OK si el registro fue exitoso, o un código de error en caso
 */
esp_err_t timer_register_isr(gptimer_handle_t *timer_id, timer_isr_t isr_callback, void *args) {
    gptimer_event_callbacks_t cbs = {
        .on_alarm = isr_callback,
    };
    return gptimer_register_event_callbacks(*timer_id, &cbs, args);
}
