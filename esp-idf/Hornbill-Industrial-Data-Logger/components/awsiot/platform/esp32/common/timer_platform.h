#ifndef PLATFORM_ESP_COMMON_TIMER_PLATFORM_H_
#define PLATFORM_ESP_COMMON_TIMER_PLATFORM_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "timer_interface.h"

struct Timer {
  uint32_t timeout;
  uint32_t start_timestamp;
};

#ifdef __cplusplus
}
#endif

#endif /* PLATFORM_ESP_COMMON_TIMER_PLATFORM_H_ */
