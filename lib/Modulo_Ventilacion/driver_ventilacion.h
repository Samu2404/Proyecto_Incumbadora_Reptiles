#ifndef DRIVER_VENTILACION_H
#define DRIVER_VENTILACION_H

#include "esp_err.h"
#include "driver/gpio.h"

esp_err_t pwm_init (gpio_num_t pwm_pin, uint32_t freq_hz);
esp_err_t pwm_set_duty (uint32_t duty);

#endif