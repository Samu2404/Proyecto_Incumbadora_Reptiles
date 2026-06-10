#ifndef DRIVER_TIMER_H
#define DRIVER_TIMER_H
#include "driver/gptimer.h"

/**
 * @brief Tipo de función de callback para el manejador de interrupciones del temporizador
 * @param timer_id Identificador del temporizador
 * @param edata Datos del evento de alarma
 * @param user_ctx Contexto del usuario
 * @return true si se procesó correctamente, false en caso contrario
 */
typedef bool (*timer_isr_t)(gptimer_handle_t timer_id, const gptimer_alarm_event_data_t *edata, void *user_ctx);

/**
 * @brief Función para registrar una función de callback como manejador de interrupciones del temporizador
 * @param timer_id Identificador del temporizador
 * @param isr_callback Función de callback a registrar
 * @param args Argumentos adicionales para la función de callback
 */

esp_err_t timer_register_isr(gptimer_handle_t *timer_id, timer_isr_t isr_callback, void *args);

/**
 * @brief Función para configurar un temporizador
 * @param timer_id Identificador del temporizador
 * @param divider Divisor de frecuencia
 * @param auto_reload Indicador de recarga automática
 * @param alarm_en Indicador de habilitación de alarma
 * @param alarm_value Valor de la alarma
 */
void timer_config (gptimer_handle_t *timer_id, int divider, bool auto_reload, bool alarm_en, uint64_t alarm_value);


#endif