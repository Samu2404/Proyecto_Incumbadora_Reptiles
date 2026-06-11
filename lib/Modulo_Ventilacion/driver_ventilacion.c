#include "driver/ledc.h"
#include "driver_ventilacion.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "esp_err.h"
#include "esp_log.h"


#define LEDC_TIMER              LEDC_TIMER_0
#define LEDC_MODE               LEDC_LOW_SPEED_MODE

esp_err_t pwm_init (gpio_num_t pwm_pin, uint32_t freq_hz){
    // Configuración del timer para el LEDC
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_MODE,
        .timer_num        = LEDC_TIMER,
        .duty_resolution  = LEDC_TIMER_8_BIT, // Resolución de 8 bits
        .freq_hz          = freq_hz,             // Frecuencia de PWM
        .clk_cfg          = LEDC_AUTO_CLK
    };
    esp_err_t err = ledc_timer_config(&ledc_timer);
    if (err != ESP_OK) {
        ESP_LOGE("PWM", "Error al configurar el timer: %s", esp_err_to_name(err));
        return err;
    }

    // Configuración del canal para el pin de ventilación
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_MODE,
        .channel        = LEDC_CHANNEL_0,
        .timer_sel      = LEDC_TIMER,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = pwm_pin,
        .duty           = 0, // Inicialmente apagado
        .hpoint         = 0
    };
    err = ledc_channel_config(&ledc_channel);
    if (err != ESP_OK) {
        ESP_LOGE("PWM", "Error al configurar el canal: %s", esp_err_to_name(err));
        return err;
    }
    return ESP_OK;
}

esp_err_t pwm_set_duty (uint32_t duty){
    // Establecer el ciclo de trabajo del PWM
    esp_err_t err = ledc_set_duty(LEDC_MODE, LEDC_CHANNEL_0, duty);
    if (err != ESP_OK) {
        ESP_LOGE("PWM", "Error al establecer el duty: %s", esp_err_to_name(err));
        return err;
    }
    err = ledc_update_duty(LEDC_MODE, LEDC_CHANNEL_0);
    if (err != ESP_OK) {
        ESP_LOGE("PWM", "Error al actualizar el duty: %s", esp_err_to_name(err));
        return err;
    }
    return ESP_OK;
}
