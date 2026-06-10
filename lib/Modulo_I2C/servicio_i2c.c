#include "servicio_i2c.h"

#include "freertos/semphr.h"

static SemaphoreHandle_t i2c_mutexes[I2C_SERVICIO_MAX_PUERTOS] = { NULL, NULL };

/**
 * @brief Valida que el puerto I2C esté dentro del rango soportado por el servicio.
 * @param port Puerto I2C a validar.
 * @return ESP_OK si el puerto es válido, o ESP_ERR_INVALID_ARG en caso contrario.
 */
static esp_err_t i2c_validar_puerto(i2c_port_t port)
{
	if (port < 0 || port >= I2C_SERVICIO_MAX_PUERTOS) {
		return ESP_ERR_INVALID_ARG;
	}
	return ESP_OK;
}

/**
 * @brief Obtiene o crea el semáforo asociado a un puerto I2C.
 * @param port Puerto I2C cuyo semáforo se requiere.
 * @return Puntero al semáforo del puerto, o NULL si hubo un error.
 */
static SemaphoreHandle_t i2c_obtener_mutex(i2c_port_t port)
{
	if (i2c_validar_puerto(port) != ESP_OK) {
		return NULL;
	}

	if (i2c_mutexes[port] == NULL) {
		i2c_mutexes[port] = xSemaphoreCreateMutex();
	}

	return i2c_mutexes[port];
}

/**
 * @brief Inicializa el semáforo asociado a un puerto I2C.
 * @param port Puerto I2C cuyo semáforo se desea preparar.
 * @return ESP_OK si el semáforo quedó disponible, o un código de error en caso contrario.
 */
esp_err_t servicio_i2c_init_port(i2c_port_t port)
{
	SemaphoreHandle_t mutex = i2c_obtener_mutex(port);
	if (mutex == NULL) {
		return ESP_ERR_NO_MEM;
	}

	return ESP_OK;
}

/**
 * @brief Toma el semáforo de un puerto I2C por un tiempo máximo determinado.
 * @param port Puerto I2C cuyo acceso se desea proteger.
 * @param timeout Tiempo máximo para tomar el semáforo.
 * @return ESP_OK si el semáforo fue tomado, o un código de error en caso contrario.
 */
static esp_err_t i2c_tomar_mutex(i2c_port_t port, TickType_t timeout)
{
	SemaphoreHandle_t mutex = i2c_obtener_mutex(port);
	if (mutex == NULL) {
		return ESP_ERR_NO_MEM;
	}

	if (xSemaphoreTake(mutex, timeout) != pdTRUE) {
		return ESP_ERR_TIMEOUT;
	}

	return ESP_OK;
}

/**
 * @brief Libera el semáforo asociado a un puerto I2C.
 * @param port Puerto I2C protegido.
 * @return ESP_OK si el semáforo fue liberado, o un código de error en caso contrario.
 */
static esp_err_t i2c_soltar_mutex(i2c_port_t port)
{
	SemaphoreHandle_t mutex = i2c_obtener_mutex(port);
	if (mutex == NULL) {
		return ESP_ERR_INVALID_STATE;
	}

	if (xSemaphoreGive(mutex) != pdTRUE) {
		return ESP_FAIL;
	}

	return ESP_OK;
}

/**
 * @brief Configura un puerto I2C y deja protegido su acceso con el semáforo correspondiente.
 * @param port Puerto I2C a configurar.
 * @param freq Frecuencia del bus en Hz.
 * @param sda Pin GPIO asignado a SDA.
 * @param scl Pin GPIO asignado a SCL.
 * @return ESP_OK si la configuración fue correcta, o un código de error en caso contrario.
 */
esp_err_t servicio_i2c_config_port(i2c_port_t port, int freq, gpio_num_t sda, gpio_num_t scl)
{
	esp_err_t ret = servicio_i2c_init_port(port);
	if (ret != ESP_OK) {
		return ret;
	}

	ret = i2c_tomar_mutex(port, portMAX_DELAY);
	if (ret != ESP_OK) {
		return ret;
	}

	ret = i2c_config(port, freq, sda, scl);

	esp_err_t unlock_ret = i2c_soltar_mutex(port);
	if (ret == ESP_OK && unlock_ret != ESP_OK) {
		ret = unlock_ret;
	}

	return ret;
}

/**
 * @brief Escribe datos en un dispositivo I2C protegiendo el puerto con su semáforo.
 * @param device Descripción del dispositivo I2C.
 * @param data Buffer con los datos a transmitir.
 * @param data_len Cantidad de bytes a escribir.
 * @param timeout Tiempo máximo para tomar el semáforo.
 * @return ESP_OK si la operación fue correcta, o un código de error en caso contrario.
 */

esp_err_t servicio_i2c_write(i2c_device_t *device, const uint8_t *data, TickType_t timeout)
{
	if (device == NULL) {
		return ESP_ERR_INVALID_ARG;
	}

	esp_err_t ret = i2c_tomar_mutex(device->port, timeout);
	if (ret != ESP_OK) {
		return ret;
	}

	ret = i2c_write(device,data);

	esp_err_t unlock_ret = i2c_soltar_mutex(device->port);
	if (ret == ESP_OK && unlock_ret != ESP_OK) {
		ret = unlock_ret;
	}

	return ret;
}

/**
 * @brief Lee datos desde un dispositivo I2C protegiendo el puerto con su semáforo.
 * @param device Descripción del dispositivo I2C.
 * @param data Buffer donde se almacenarán los datos leídos.
 * @param data_len Cantidad de bytes a leer.
 * @param timeout Tiempo máximo para tomar el semáforo.
 * @return ESP_OK si la operación fue correcta, o un código de error en caso contrario.
 */

esp_err_t servicio_i2c_read(i2c_device_t *device, uint8_t *data, size_t data_len, TickType_t timeout)
{
	if (device == NULL) {
		return ESP_ERR_INVALID_ARG;
	}

	esp_err_t ret = i2c_tomar_mutex(device->port, timeout);
	if (ret != ESP_OK) {
		return ret;
	}

	ret = i2c_read(device, data, data_len);

	esp_err_t unlock_ret = i2c_soltar_mutex(device->port);
	if (ret == ESP_OK && unlock_ret != ESP_OK) {
		ret = unlock_ret;
	}

	return ret;
}
