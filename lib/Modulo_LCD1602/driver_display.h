#ifndef DRIVER_DISPLAY_H
#define DRIVER_DISPLAY_H

#include "esp_err.h"
#include "driver/i2c_master.h"
/**
 * @brief Inicializa el LCD1602 en modo I2C
 * @param i2c_port El puerto I2C a utilizar (I2C_NUM_0 o I2C_NUM_1)
 * @return ESP_OK si la inicialización fue exitosa, o un código de error en caso contrario
 */
esp_err_t lcd1602_init(i2c_port_t i2c_port);

/**
 * @brief Escribe un byte en el LCD1602
 * @param byte El byte a escribir (comando o dato)
 * @param flags Indica si el byte es un comando (0) o un dato (RS=1)
 * @return ESP_OK si la escritura fue exitosa, o un código de error en caso contrario
 */
esp_err_t lcd_1602_write(uint8_t byte, uint8_t flags);

/**
 * @brief Escribe una cadena de texto en el LCD1602
 * @param str La cadena de texto a escribir (debe ser menor a 16 caracteres)
 * @return ESP_OK si la escritura fue exitosa, o un código de error en caso contrario
 */
esp_err_t lcd1602_write_string(const char *str,...);

/**
 * @brief Mueve el cursor a una posición específica en el LCD1602
 * @param row La fila a la que mover el cursor (0 o 1)
 * @param col La columna a la que mover el cursor (0 a 15)
 * @return ESP_OK si el movimiento fue exitoso, o un código de error en caso contrario
 */

 
/**
 * @brief Limpia la pantalla del LCD1602
 * @return ESP_OK si la operación fue exitosa, o un código de error en caso
 */
esp_err_t lcd1602_clear(void);


#endif 