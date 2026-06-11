#ifndef SERVICIO_VENTILACION_H
#define SERVICIO_VENTILACION_H
#include "stdint.h"
typedef struct {
    const int num_etapas;
    const uint16_t *raw_treshold;
    const uint8_t *velocidad;
} ventilacion_config_t;

esp_err_t servicio_pwm_init (gpio_num_t pwm_pin);
esp_err_t servicio_control_ventilacion (uint16_t raw_value);
int raw_to_speed(uint16_t raw_value);

#endif