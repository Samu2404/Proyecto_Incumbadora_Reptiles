#ifndef DRIVER_WIFI_H
#define DRIVER_WIFI_H
#include "string.h"
#include "esp_log.h"
#include "esp_wifi.h"

esp_err_t wifi_init(void);
esp_err_t wifi_connect(char* wifi_ssid, char* wifi_password);
esp_err_t wifi_disconnect(void);
esp_err_t wifi_deinit(void);

#endif 
