#ifndef TIMERS_HEADER_FILE
#define TIMERS_HEADER_FILE

#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

uint64_t ts_millis(void);
void start_timer(timer_t *timerid);
void stop_timer(timer_t *timerid);

#endif
