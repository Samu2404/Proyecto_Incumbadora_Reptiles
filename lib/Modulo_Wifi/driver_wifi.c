#include <string.h>
#include <inttypes.h>
#include "freertos/FreeRTOS.h"
#include "esp_err.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_netif.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"


// Variables globales
static const char *TAG = "wifi_driver";

// Variables de conexion wifi
#define WIFI_AUTHMODE WIFI_AUTH_WPA2_PSK
#define WIFI_HOSTNAME "esp32-node"
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT BIT1
static const int WIFI_RETRY_ATTEMPT = 3;
static int wifi_retry_count = 0;


// Variables de eventos y handlers
static esp_netif_t *wifi_netif = NULL;
static esp_event_handler_instance_t ip_event_handler;
static esp_event_handler_instance_t wifi_event_handler;

static EventGroupHandle_t s_wifi_event_group = NULL;

/** @brief Callback para manejar eventos de IP
 * @param arg puntero a datos de usuario
 * @param event_base base de eventos
 * @param event_id código del evento
 * @param event_data puntero a datos del evento
 */
static void ip_event_cb(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    ESP_LOGI(TAG, "Handling IP event, event code 0x%" PRIx32, event_id);
    switch (event_id)
    {
    case (IP_EVENT_STA_GOT_IP):
        ip_event_got_ip_t *event_ip = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "Got IP: " IPSTR, IP2STR(&event_ip->ip_info.ip));
        wifi_retry_count = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        break;
    case (IP_EVENT_STA_LOST_IP):
        ESP_LOGI(TAG, "Lost IP");
        break;
    case (IP_EVENT_GOT_IP6):
        ip_event_got_ip6_t *event_ip6 = (ip_event_got_ip6_t *)event_data;
        ESP_LOGI(TAG, "Got IPv6: " IPV6STR, IPV62STR(event_ip6->ip6_info.ip));
        wifi_retry_count = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        break;
    default:
        ESP_LOGI(TAG, "IP event not handled");
        break;
    }
}

/** @brief Callback para manejar eventos de Wi-Fi
 * @param arg puntero a datos de usuario
 * @param event_base base de eventos
 * @param event_id código del evento
 * @param event_data puntero a datos del evento
 */
static void wifi_event_cb(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    ESP_LOGI(TAG, "Handling Wi-Fi event, event code 0x%" PRIx32, event_id);

    switch (event_id)
    {
    case (WIFI_EVENT_WIFI_READY):
        ESP_LOGI(TAG, "Wi-Fi listo");
        break;
    case (WIFI_EVENT_SCAN_DONE):
        ESP_LOGI(TAG, "Wi-Fi escaneo completado");
        break;
    case (WIFI_EVENT_STA_START):
        ESP_LOGI(TAG, "Wi-Fi iniciado, conectando...");
        esp_wifi_connect();
        break;
    case (WIFI_EVENT_STA_STOP):
        ESP_LOGI(TAG, "Wi-Fi detenido");
        break;
    case (WIFI_EVENT_STA_CONNECTED):
        ESP_LOGI(TAG, "Wi-Fi conectado");
        break;
    case (WIFI_EVENT_STA_DISCONNECTED):
        ESP_LOGI(TAG, "Wi-Fi desconectado");
        if (wifi_retry_count < WIFI_RETRY_ATTEMPT) {
            ESP_LOGI(TAG, "Intentanndo reconexion...");
            esp_wifi_connect();
            wifi_retry_count++;
        } else {
            ESP_LOGI(TAG, "Fallo en reconexion, alcanzado el maximo de intentos");
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        break;
    case (WIFI_EVENT_STA_AUTHMODE_CHANGE):
        ESP_LOGI(TAG, "Wi-Fi authmode cambiado");
        break;
    default:
        ESP_LOGI(TAG, "evento Wi-Fi no manejado");
        break;
    }
}


/**     
 * @brief Inicializa el módulo Wi-Fi, configurando la pila de red, el almacenamiento no volátil y los handlers de eventos necesarios para la conexión Wi-Fi
 * @return ESP_OK si la inicialización fue exitosa, ESP_FAIL si hubo un error durante la inicialización del módulo Wi-Fi
 */

esp_err_t wifi_init() {

    esp_err_t ret = nvs_flash_init(); // Inicializa el almacenamiento no volátil (NVS)
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }

    s_wifi_event_group = xEventGroupCreate();

    ret = esp_netif_init(); // Inicializa la pila de red TCP/IP
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error al inicializar la pila de red TCP/IP");
        return ret;
    }

    ret = esp_event_loop_create_default(); // Crea un loop de eventos por defecto para manejar eventos del sistema
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error al crear el loop de eventos por defecto");
        return ret;
    }
        ret = esp_wifi_set_default_wifi_sta_handlers();
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error al establecer los handlers por defecto para WiFi STA");
        return ret;
    }

    wifi_netif = esp_netif_create_default_wifi_sta(); // Crea una interfaz de red por defecto para el modo estación WiFi
    if (wifi_netif == NULL) {
        ESP_LOGE(TAG, "Error al crear la interfaz por defecto para WiFi STA");
        return ESP_FAIL;
    }

    ret = esp_netif_set_hostname(wifi_netif, WIFI_HOSTNAME);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error al configurar el hostname Wi-Fi");
        return ret;
    }
    ESP_LOGI(TAG, "Hostname Wi-Fi configurado como: %s", WIFI_HOSTNAME);

    // Wi-Fi stack configuration parameters
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_cb,
                                                        NULL,
                                                        &wifi_event_handler));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &ip_event_cb,
                                                        NULL,
                                                        &ip_event_handler));
    return ret;
}

/**
 * @brief Conecta a una red Wi-Fi específica
 * @param wifi_ssid El SSID de la red Wi-Fi a la que se desea conectar
 * @param wifi_password La contraseña de la red Wi-Fi a la que se desea conectar
 * @return ESP_OK si la conexión fue exitosa, ESP_FAIL si hubo un error al conectar a la red Wi-Fi
 */
esp_err_t wifi_connect(char* wifi_ssid, char* wifi_password)
{
    wifi_config_t wifi_config = {
        .sta = {
            // this sets the weakest authmode accepted in fast scan mode (default)
            .threshold.authmode = WIFI_AUTHMODE,
        },
    };

    strncpy((char*)wifi_config.sta.ssid, wifi_ssid, sizeof(wifi_config.sta.ssid));
    strncpy((char*)wifi_config.sta.password, wifi_password, sizeof(wifi_config.sta.password));

    ESP_ERROR_CHECK(esp_wifi_set_ps(WIFI_PS_NONE)); // default is WIFI_PS_MIN_MODEM
    ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM)); // default is WIFI_STORAGE_FLASH

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    ESP_LOGI(TAG, "Connecting to Wi-Fi network: %s", wifi_config.sta.ssid);
    ESP_ERROR_CHECK(esp_wifi_start());

    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group, WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
        pdFALSE, pdFALSE, portMAX_DELAY);

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "Conexion establecida con: %s", wifi_config.sta.ssid);
        return ESP_OK;
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGE(TAG, "Fallo al establecer conexion con: %s", wifi_config.sta.ssid);
        return ESP_FAIL;
    }

    ESP_LOGE(TAG, "Error desconocido al conectar a Wi-Fi");
    return ESP_FAIL;
}

/**
 * @brief Desconecta de la red Wi-Fi actual y limpia los recursos asociados a la conexión Wi-Fi
 * @return ESP_OK si la desconexión fue exitosa, ESP_FAIL si hubo un error al desconectar de la red Wi-Fi
 */
esp_err_t wifi_disconnect(void) {
    if (s_wifi_event_group) {
        vEventGroupDelete(s_wifi_event_group);
    }
    return esp_wifi_disconnect();
}


/** 
* @brief Desinicializa el módulo Wi-Fi, deteniendo la conexión Wi-Fi, limpiando los recursos asociados y eliminando los handlers de eventos registrados
* @return ESP_OK si la desinicialización fue exitosa, ESP_FAIL si hubo un error
*/
esp_err_t wifi_deinitialization(void)
{
    esp_err_t ret = esp_wifi_stop();
    if (ret == ESP_ERR_WIFI_NOT_INIT) {
        ESP_LOGE(TAG, "Wi-Fi stack no se ha inicializado");
        return ret;
    }

    ESP_ERROR_CHECK(esp_wifi_deinit());
    ESP_ERROR_CHECK(esp_wifi_clear_default_wifi_driver_and_handlers(wifi_netif));
    esp_netif_destroy(wifi_netif);

    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, ESP_EVENT_ANY_ID, ip_event_handler));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_event_handler));

    return ESP_OK;
}