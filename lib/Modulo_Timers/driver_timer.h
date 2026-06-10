#ifndef DRIVER_TIMER_H
#define DRIVER_TIMER_H
#include "driver/timer.h"


void timer_config (gptimer_handle_t timer_id, int divider, bool auto_reload, bool alarm_en, bool intr_en, uint64_t alarm_value);
int get_timer_value(gptimer_handle_t timer_id);
bool is_timer_alarm_triggered(gptimer_handle_t timer_id);
bool stop_timer(gptimer_handle_t timer_id);
bool reset_timer_value(gptimer_handle_t timer_id);


#endif