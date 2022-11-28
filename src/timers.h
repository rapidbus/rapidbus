#ifndef TIMERS_HEADER_FILE
#define TIMERS_HEADER_FILE

#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

struct ml_task {
  char name[128];
  uint8_t state;
  uint64_t last_run;
  uint32_t period_ms;
};

#define MAX_TASKS_COUNT 10

uint64_t ts_millis();
void start_timer(timer_t *timerid);

#endif
