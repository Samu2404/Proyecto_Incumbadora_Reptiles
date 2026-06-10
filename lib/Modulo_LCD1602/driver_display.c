#include "driver_display.h"
#include "driver_i2c.h"
#include "servicio_i2c.h"
#include "driver/i2c_master.h"

const i2c_port_t lcd_1602_port = NULL; 

i2c_device_t lcd_1602_device ={
    .port = lcd_1602_port,
    .address = 0x27
};

// registros y banderas de control del LCD1602
/**
 * @brief Funcion que inicializa el LCD1602
 * @param port Puerto I2C al que esta conectado el LCD1602
 * @return ESP_OK si la inicializacion fue exitosa, ESP_FAIL en caso contrario
 */
esp_err_t lcd_1602_init(i2c_port_t port){
    // Implementacion de la inicializacion
    return ESP_OK;
}

