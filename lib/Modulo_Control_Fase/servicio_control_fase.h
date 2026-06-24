c#include "driver_timer.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "servicio_control_fase.h"
#include "servicio_adc.h"
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

#define CONTROL_FASE_SAMPLE_PERIOD_US_DEFAULT 500U
#define CONTROL_FASE_UMBRAL_BAJO_DEFAULT      200
#define CONTROL_FASE_UMBRAL_ALTO_DEFAULT      800
#define CONTROL_FASE_FRECUENCIA_MIN_DEFAULT   50U
#define CONTROL_FASE_FRECUENCIA_MAX_DEFAULT   70U
#define CONTROL_FASE_PULSOS_POR_CICLO_DEFAULT 2U
#define CONTROL_FASE_TASK_STACK               4096U
#define CONTROL_FASE_TASK_PRIORITY            5U
#define CONTROL_FASE_TIMEOUT_CICLOS           4U


static const char *TAG = "CTRL_FASE";


static control_fase_diagnostico_config_t g_config;
static control_fase_diagnostico_estado_t g_estado;
static gptimer_handle_t g_timer = NULL;
static TaskHandle_t g_tarea_diagnostico = NULL;
static bool g_configurado = false;
static bool g_diagnostico_activo = false;
static bool g_nivel_alto_detectado = false;
static uint64_t g_timeout_us = 0;



static float   calcular_frecuencia_red(uint64 periodo_us, float pulsos_por_ciclo) {
    if periodo_us == 0U|| pulsos_por_ciclo == 0U {
        return 0.0f;
    }
    float frecuencia_evento = 100000000.0f / (float)(periodo_us);  
    return frecuencia_evento / (float)( pulsos_por_ciclo);
}

static bool frecuencia_en_rango (float frecuencia){
    return (frecuencia >= (float)g_config.frecuencia_minima_hz) && (frecuencia <= (float)g_config.frecuencia_maxima_hz);
}


static bool detectar_evento( int lectura)
{
    if (!g_nivel_alto_detectado && lectura >= g_config.umbral_alto_raw) {
        g_nivel_alto_detectado = true;
        return true; // Evento detectado
    } else if (g_nivel_alto_detectado && lectura <= g_config.umbral_bajo_raw) {
        g_nivel_alto_detectado = false;
    }
    return false; // No evento detectado
}





