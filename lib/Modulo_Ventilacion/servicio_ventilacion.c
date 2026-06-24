#include "driver/gptimer.h"
#include "driver_ventilacion.h"
#include "servicio_ventilacion.h"
#include "driver_timer.h"
#include "freertos/semphr.h"
#include "driver/gpio.h"
#include "stdint.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


ventilacion_config_t config_ventilacion = {
    .num_etapas = 3,
    .raw_treshold = (uint16_t[]){1000, 2000, 3000},
    .velocidad = (uint8_t[]){85, 170, 255}
};


int velocidad=0;
int current_speed = 0;
#define SPEED_UPDATE_DELAY_US 10000


gptimer_handle_t timer_handler;
SemaphoreHandle_t xRampSemaphore = NULL;


static void  IRAM_ATTR step_update_isr(void* arg){
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // Desbloquea la tarea de la rampa de manera segura desde la ISR
    xSemaphoreGiveFromISR(xRampSemaphore, &xHigherPriorityTaskWoken);

    // Retornar si se requiere un cambio de contexto inmediato
    return (xHigherPriorityTaskWoken == pdTRUE);
};


void vRampProcessingTask(void *pvParameters) {
    while (1) {
        // Espera indefinidamente el semáforo de la ISR sin consumir CPU
        if (xSemaphoreTake(xRampSemaphore, portMAX_DELAY) == pdTRUE) {
            
            // Aquí se realiza el trabajo "pesado" de forma segura
            if (current_speed != velocidad) {
                if (current_speed < velocidad) {
                    current_speed++;
                } else {
                    current_speed--;
                }
                pwm_set_duty(current_speed); // Llamada segura fuera de la ISR
            }
        }
    }
};


esp_err_t servicio_pwm_init (gpio_num_t pwm_pin){
    pwm_init(pwm_pin);

    // Creación del semáforo binario
    xRampSemaphore = xSemaphoreCreateBinary();
    if (xRampSemaphore == NULL) {
        return ESP_ERR_NO_MEM;
    }

    // Creación de la tarea con alta prioridad (ej. prioridad 10)
    BaseType_t xReturned = xTaskCreate(
        vRampProcessingTask,
        "RampTask",
        2048,
        NULL,
        10, // Prioridad alta para responder inmediatamente al semáforo
        NULL
    );

    if (xReturned != pdPASS) {
        return ESP_ERR_NO_MEM;
    }
    timer_config(&timer_handler, 80, true, true, SPEED_UPDATE_DELAY_US); // Configura el timer para generar una interrupción cada 10 ms
    gptimer_register_isr(timer_handler, step_update_isr, NULL); // Registra la ISR para el timer
    ESP_ERROR_CHECK(gptimer_enable(timer_handler));
    ESP_ERROR_CHECK(gptimer_start(timer_handler));
}


int raw_to_speed (uint16_t  raw_value){
    int speed; 
    if (raw_value > 4095){
        return speed = 255; 
    }
    else {
        return speed = (uint8_t)(raw_value >> 4);
    }
}

esp_err_t servicio_control_ventilacion (uint16_t raw_value){
    for (int i = 0; i < config_ventilacion.num_etapas; i++) {
        if (raw_value < config_ventilacion.raw_treshold[i]) {
            velocidad = config_ventilacion.velocidad[i];
            return ESP_OK;
        }
    }
    return ESP_OK; // Si el valor raw_value supera todas las etapas, se mantiene la velocidad actual (última etapa)
}


