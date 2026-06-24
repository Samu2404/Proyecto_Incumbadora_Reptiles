#include "driver_timer.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "servicio_control_fase.h"
#include "servicio_adc.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

