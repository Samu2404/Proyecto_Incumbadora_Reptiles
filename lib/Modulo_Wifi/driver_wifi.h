#ifndef DRIVER_WIFI_H
#define DRIVER_WIFI_H
#include "string.h"
#include "esp_log.h"
#include "esp_wifi.h"

/**
 * @brief Función para inicializar el módulo WiFi
 * @return esp_err_t - Código de error de la operación
 * 
 */
esp_err_t wifi_init(void);
/**
 * @brief Función para conectar a una red WiFi
 * @param wifi_ssid - Nombre de la red WiFi
 * @param wifi_password - Contraseña de la red WiFi
 * @return esp_err_t - Código de error de la operación
 */
esp_err_t wifi_connect(char* wifi_ssid, char* wifi_password);

/**
 * @brief Función para desconectar del WiFi
 * @return esp_err_t - Código de error de la operación
 */
esp_err_t wifi_disconnect(void);

/**
 * @brief Función para desinicializar el módulo WiFi
 * @return esp_err_t - Código de error de la operación
 */
esp_err_t wifi_deinitialization(void);

#endif 
