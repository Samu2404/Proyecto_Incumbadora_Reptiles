#include "driver_sht31d.h"
#include "servicio_i2c.h"
#include "driver/i2c_master.h"


i2c_device_t sht31d ={
    .port = -1, // se asignará en la inicialización
    .address = 0x44,
    .name = "SHT31D"
};

static esp_err_t _sht31d_send (uint16_t data){
    return servicio_i2c_write(&sht31d, &data, pdMS_TO_TICKS(10));
};


