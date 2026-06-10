#ifndef SERVICIO_I2C_H
#define SERVICIO_I2C_H

#include <stddef.h>
#include <stdint.h>

#include "driver/gpio.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"

#include "driver_i2c.h"

#define I2C_SERVICIO_MAX_PUERTOS 2

/**
 * @brief Inicializa el semáforo asociado a un puerto I2C.
 * @param port Puerto I2C cuyo semáforo se desea preparar.
 * @return ESP_OK si el semáforo quedó disponible, o un código de error en caso contrario.
 */
esp_err_t servicio_i2c_init_port(i2c_port_t port);

/**
 * @brief Configura un puerto I2C y deja protegido su acceso con el semáforo correspondiente.
 * @param port Puerto I2C a configurar.
 * @param freq Frecuencia del bus en Hz.
 * @param sda Pin GPIO asignado a SDA.
 * @param scl Pin GPIO asignado a SCL.
 * @return ESP_OK si la configuración fue correcta, o un código de error en caso contrario.
 */
esp_err_t servicio_i2c_config_port(i2c_port_t port, int freq, gpio_num_t sda, gpio_num_t scl);

/**
 * @brief Escribe datos en un dispositivo I2C protegiendo el puerto con su semáforo.
 * @param device Descripción del dispositivo I2C.
 * @param data Buffer con los datos a transmitir.
 * @param data_len Cantidad de bytes a escribir.
 * @param timeout Tiempo máximo para tomar el semáforo.
 * @return ESP_OK si la operación fue correcta, o un código de error en caso contrario.
 */
esp_err_t servicio_i2c_write(i2c_device_t *device, const uint8_t *data,TickType_t timeout);

/**
 * @brief Lee datos desde un dispositivo I2C protegiendo el puerto con su semáforo.
 * @param device Descripción del dispositivo I2C.
 * @param data Buffer donde se almacenarán los datos leídos.
 * @param data_len Cantidad de bytes a leer.
 * @param timeout Tiempo máximo para tomar el semáforo.
 * @return ESP_OK si la operación fue correcta, o un código de error en caso contrario.
 */
esp_err_t servicio_i2c_read(i2c_device_t *device, uint8_t *data, size_t data_len, TickType_t timeout);

#endif
