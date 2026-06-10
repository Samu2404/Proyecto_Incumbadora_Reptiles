#ifndef DRIVER_I2C_H
#define DRIVER_I2C_H

#include <stddef.h>
#include <stdint.h>

#include "driver/gpio.h"
#include "driver/i2c.h"
#include "esp_err.h"

typedef struct {
    i2c_port_t port;
    uint8_t address;
    const char *nombre;
} i2c_device_t;

/**
 * @brief Configura un puerto I2C como maestro con la frecuencia y pines indicados.
 * @param port Puerto I2C a configurar.
 * @param freq Frecuencia del bus en Hz.
 * @param sda Pin GPIO asignado a SDA.
 * @param scl Pin GPIO asignado a SCL.
 * @return ESP_OK si la configuración fue correcta, o un código de error en caso contrario.
 */
esp_err_t i2c_config(i2c_port_t port, int freq, gpio_num_t sda, gpio_num_t scl);

/**
 * @brief Escribe un bloque de datos en un dispositivo I2C.
 * @param device Descripción del dispositivo I2C.
 * @param data Buffer con los datos a transmitir.
 * @param data_len Cantidad de bytes a escribir.
 * @return ESP_OK si la transferencia fue correcta, o un código de error en caso contrario.
 */
esp_err_t i2c_write(i2c_device_t *device, const uint8_t *data, size_t data_len);

/**
 * @brief Lee un bloque de datos desde un dispositivo I2C.
 * @param device Descripción del dispositivo I2C.
 * @param data Buffer donde se almacenarán los datos leídos.
 * @param data_len Cantidad de bytes a leer.
 * @return ESP_OK si la transferencia fue correcta, o un código de error en caso contrario.
 */
esp_err_t i2c_read(i2c_device_t *device, uint8_t *data, size_t data_len);

#endif