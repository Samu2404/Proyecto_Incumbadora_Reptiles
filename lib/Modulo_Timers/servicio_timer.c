#include "servicio_timer.h"
#include "driver_timer.h"
#include "driver/gptimer.h"
// Variables globales

// Declaración del temporizador
gptimer_handle_t temporizador;

// Función para configurar el temporizador
void init_timer(void) {
    timer_config(&temporizador, 80, false, false, false, 1000000);
}
