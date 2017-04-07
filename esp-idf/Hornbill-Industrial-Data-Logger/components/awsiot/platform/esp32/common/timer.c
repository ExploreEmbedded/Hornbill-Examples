#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stddef.h>
#include <sys/types.h>
#include <stdint.h>
#include <stdbool.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "timer_platform.h"

bool has_timer_expired(Timer *timer) {
  if (left_ms(timer) > 0)
    return false;
  else
    return true;
}

void countdown_ms(Timer *timer, uint32_t timeout) {
  timer->timeout = timeout;
  timer->start_timestamp = xTaskGetTickCount() / portTICK_PERIOD_MS;
}

uint32_t left_ms(Timer *timer) {
  uint32_t current_timestamp = xTaskGetTickCount() / portTICK_PERIOD_MS;
  uint32_t time_diff_ms = current_timestamp - timer->start_timestamp;
  if (timer->timeout > time_diff_ms)
    return timer->timeout - time_diff_ms;
  else
    return 0;
}

void countdown_sec(Timer *timer, uint32_t timeout) {
  timer->timeout = timeout * 1000;
  timer->start_timestamp = xTaskGetTickCount() / portTICK_PERIOD_MS;
}

void init_timer(Timer *timer) {
  timer->timeout = 0;
  timer->start_timestamp = 0;
}

#ifdef __cplusplus
}
#endif
