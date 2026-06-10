#include "driver_i2c.h"
#include "servicio_i2c.h"
#include "driver_display.h"
#include "driver_wifi.h"
#include "driver_ble.h"

void app_main(void) {
    // Inicializar el I2C
    wifi_init();
    servicio_i2c_config_port(I2C_NUM_0, 100000, GPIO_NUM_21, GPIO_NUM_22);
    bluetooth_init();

}