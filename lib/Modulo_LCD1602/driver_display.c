#include "driver_display.h"
#include "driver_i2c.h"
#include "servicio_i2c.h"
#include "driver/i2c_master.h"
#include "string.h"
#include "stdio.h"
#include "esp_rom_sys.h"
#include "esp_err.h"
#include "stdarg.h"


i2c_device_t lcd_1602_device ={
    .port = -1, // se asignará en la inicialización
    .address = 0x27,
    .name= "LCD1602"
} ;

#define BL   0x08   // bit 3
#define EN   0x04   // bit 2
#define RW   0x02   // bit 1  
#define RS   0x01   // bit 0

#define WRITE_TIMEOUT_MS 10

static esp_err_t lcd1602_send(uint1_t value) {
    return servicio_i2c_write(&lcd_1602_device, &value, pdMS_TO_TICKS(WRITE_TIMEOUT_MS));
}

// registros y banderas de control del LCD1602
/**
 * @brief Funcion que inicializa el LCD1602
 * @param port Puerto I2C al que esta conectado el LCD1602
 * @return ESP_OK si la inicializacion fue exitosa, ESP_FAIL en caso contrario
 */
esp_err_t lcd_1602_init(i2c_port_t port) {
    lcd_1602_device.port = port;
    esp_err_t ret = ESP_OK;
    return ret;
}
/** 
 * @brief Funcion que escribe un byte en el LCD1602
 * @param byte Byte a escribir en el LCD1602
 * @param flags Banderas de control (RS, RW, EN) para la escritura
 * @return ESP_OK si la escritura fue exitosa, ESP_FAIL en caso contrario
 */
esp_err_t lcd1602_write(uint8_t byte, uint8_t flags) {
    uint8_t nibble_alto = (byte & 0xF0);        // toma los 4 bits altos
    uint8_t nibble_bajo = (byte & 0x0F) << 4;   // toma los 4 bits bajos y los sube

    // nibble alto con pulso EN
    lcd1602_send(nibble_alto | BL | EN | flags);
    esp_rom_delay_us(1);
    lcd1602_send(nibble_alto | BL | flags);
    esp_rom_delay_us(50);

    // nibble bajo con pulso EN
    lcd1602_send(nibble_bajo | BL | EN | flags);
    esp_rom_delay_us(1);
    lcd1602_send(nibble_bajo | BL | flags);
    esp_rom_delay_us(50);

    return ESP_OK;
}
/**
 * @brief Funcion que configura el LCD1602 para su uso
 * @return ESP_OK si la configuracion fue exitosa, ESP_FAIL en caso contrario
 */
esp_err_t lcd1602_setup(void){
vTaskDelay(pdMS_TO_TICKS(50)); // Espera a que el LCD esté listo
    lcd1602_send(0x30 | BL | EN);   // 0x3 en posición alta, EN=1
    esp_rom_delay_us(100);
    lcd1602_send(0x30 | BL);        // EN=0, HD44780 captura
    vTaskDelay(pdMS_TO_TICKS(5));

    lcd1602_send(0x30 | BL | EN);
    esp_rom_delay_us(100);
    lcd1602_send(0x30 | BL);
    esp_rom_delay_us(150);

    lcd1602_send(0x30 | BL | EN);
    esp_rom_delay_us(100);
    lcd1602_send(0x30 | BL);
    esp_rom_delay_us(150);

    lcd1602_send(0x20 | BL | EN);
    esp_rom_delay_us(100);
    lcd1602_send(0x20 | BL);
    esp_rom_delay_us(150);

    lcd1602_write(0x28, 0x00);          // function set: 4 bits, 2 líneas, 5x8
    lcd1602_write(0x0C, 0x00);          // display on, cursor off, blink off
    lcd1602_write(0x01, 0x00);          // clear display
    vTaskDelay(pdMS_TO_TICKS(2));       
    lcd1602_write(0x06, 0x00);          // entry mode: cursor avanza derecha

    return ESP_OK;
}

esp_err_t lcd1602_write_string(const char *str,...) {
    char buffer[33];  // máximo 32 caracteres del display + null terminator
    
    va_list args;
    va_start(args, str);
    vsnprintf(buffer, sizeof(buffer), str, args);
    va_end(args);
    
    // enviar cada carácter
    char* ptr = buffer;
    while (*ptr) {
        if (*ptr == '\x01') {          // código especial para °
            lcd1602_write(0xDF, RS);
        } else {
            lcd1602_write((uint8_t)*ptr, RS);
        }
        ptr++;
    }
    return ESP_OK;
}

esp_err_t lcd1602_set_cursor(uint8_t row, uint8_t col) {

    uint8_t direccion;

    switch(row) {

        case 0:
            direccion = 0x00 + col;
            break;

        case 1:
            direccion = 0x40 + col;
            break;

        default:
            direccion = 0x00 + col;
            break;
    }

    lcd1602_write(0x80 | direccion, 0x00);
    return ESP_OK;
}

esp_err_t lcd1602_clear(void) {
    lcd1602_write(0x01, 0x00);
    vTaskDelay(pdMS_TO_TICKS(2));
    return ESP_OK;
}

