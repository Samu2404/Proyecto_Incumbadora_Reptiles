#include "driver_timer.h"
#include "driver/gptimer.h"

/** 
* @brief Función para configurar el timer
* @param timer_id identificador del timer a configurar
* @param divider valor del divisor para ajustar la resolución del timer
* @param auto_reload true para habilitar auto-reload, false para deshabilitar
* @param alarm_en true para habilitar alarma, false para deshabilitar
* @param intr_en true para habilitar interrupción, false para deshabilitar
* @param alarm_value valor de la alarma en microsegundos
*/
void timer_config (gptimer_handle_t timer_id, int divider, bool auto_reload, bool alarm_en, bool intr_en, uint64_t alarm_value) {
    gptimer_config_t timer_config = {
        .clk_src = GPTIMER_CLK_SRC_DEFAULT,
        .direction = GPTIMER_COUNT_UP,
        .resolution_hz = 80000000 / divider, // Resolución en microsegundos
    };


    ESP_ERROR_CHECK(gptimer_new_timer(&timer_config, &timer_id));

    if (alarm_en) {
        gptimer_alarm_config_t alarm_config = {
            .alarm_count = alarm_value,
            .flags.auto_reload_on_alarm = auto_reload,
        };
        ESP_ERROR_CHECK(gptimer_set_alarm_action(timer_id, &alarm_config));
    }

    if (intr_en) {
        ESP_ERROR_CHECK(gptimer_enable_intr(timer_id));
    }

    ESP_ERROR_CHECK(gptimer_start(timer_id));
}

/** 
* @brief Función para obtener el valor del timer
* @param timer_id identificador del timer
* @return valor del timer
*/
int get_timer_value(gptimer_handle_t timer_id) {
    uint64_t count;
    ESP_ERROR_CHECK(gptimer_get_raw_count(timer_id, &count));
    return count;
}

/** 
* @brief Función para verificar si la alarma del timer ha sido activada
* @param timer_id identificador del timer
* @return true si la alarma ha sido activada, false en caso contrario
*/

bool is_timer_alarm_triggered(gptimer_handle_t timer_id) {
    bool triggered;
    ESP_ERROR_CHECK(gptimer_is_alarm_triggered(timer_id, &triggered));
    return triggered;
}

/** 
* @brief Función para detener el timer
* @param timer_id identificador del timer a detener
*/

bool stop_timer(gptimer_handle_t timer_id) {
    return ESP_ERROR_CHECK(gptimer_stop(timer_id)) == ESP_OK;
}

/** 
* @brief Función para reiniciar el valor del timer
* @param timer_id identificador del timer a reiniciar
* @return true si el timer fue reiniciado correctamente, false en caso contrario
*/
bool reset_timer_value(gptimer_handle_t timer_id) {
    return ESP_ERROR_CHECK(gptimer_set_raw_count(timer_id, 0)) == ESP_OK;
}

