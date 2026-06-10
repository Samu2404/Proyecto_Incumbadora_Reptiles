#include "driver_i2c.h"

#include "driver/i2c_master.h"
#include "esp_log.h"
#include "freertos/FreeRTOS.h"

static const char *TAG = "DRIVER_I2C";

/**
 * @brief Configura el bus I2C con la frecuencia y los pines SDA y SCL especificados.
 * @param port Puerto I2C a configurar.
 * @param freq Frecuencia del bus I2C en Hz.
 * @param sda Pin GPIO para SDA.
 * @param scl Pin GPIO para SCL.
 * @return ESP_OK si la configuración fue correcta, o un código de error en caso contrario.
 */

esp_err_t i2c_config(i2c_port_t port, int freq, gpio_num_t sda, gpio_num_t scl)
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = sda,
        .scl_io_num = scl,
        .sda_pullup_en = GPIO_PULLUP_DISABLE,
        .scl_pullup_en = GPIO_PULLUP_DISABLE,
        .master.clk_speed = freq
    };

    esp_err_t ret = i2c_param_config(port, &conf);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error al configurar el puerto I2C %d", port);
        return ret;
    }

    ret = i2c_driver_install(port, conf.mode, 0, 0, 0);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error al instalar el driver I2C %d", port);
    }
    return ret;
}

/**
 * @brief Escribe datos en un dispositivo I2C.
 * @param device Puntero al dispositivo I2C.
 * @param data Puntero a los datos a escribir.
 * @param data_len Longitud de los datos a escribir.
 * @return ESP_OK si la operación fue exitosa, o un código de error en caso contrario.
 */
esp_err_t i2c_write(i2c_device_t *device, const uint8_t data)
{
    if (device == NULL || data == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    const char *tag = (device->nombre != NULL) ? device->nombre : TAG;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    if (cmd == NULL) {
        return ESP_ERR_NO_MEM;
    }

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (device->address << 1) | I2C_MASTER_WRITE, true);
    i2c_master_write_byte(cmd, data, true);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(device->port, cmd, pdMS_TO_TICKS(100));
    if (ret != ESP_OK) {
        ESP_LOGE(tag, "Error al escribir en el dispositivo I2C");
    }

    i2c_cmd_link_delete(cmd);
    return ret;
}

/**
 * @brief Lee datos de un dispositivo I2C.
 * @param device Puntero al dispositivo I2C.
 * @param data Puntero a los datos a leer.
 * @param data_len Longitud de los datos a leer.
 * @return ESP_OK si la operación fue exitosa, o un código de error en caso contrario.
 */
esp_err_t i2c_read(i2c_device_t *device, uint8_t *data, size_t data_len)
{
    if (device == NULL || data == NULL || data_len == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    const char *tag = (device->nombre != NULL) ? device->nombre : TAG;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    if (cmd == NULL) {
        return ESP_ERR_NO_MEM;
    }

    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (device->address << 1) | I2C_MASTER_READ, true);
    if (data_len > 1) {
        i2c_master_read(cmd, data, data_len - 1, I2C_MASTER_ACK);
    }
    i2c_master_read_byte(cmd, data + (data_len - 1), I2C_MASTER_NACK);
    i2c_master_stop(cmd);

    esp_err_t ret = i2c_master_cmd_begin(device->port, cmd, pdMS_TO_TICKS(100));
    if (ret != ESP_OK) {
        ESP_LOGE(tag, "Error al leer del dispositivo I2C");
    }

    i2c_cmd_link_delete(cmd);
    return ret;
}