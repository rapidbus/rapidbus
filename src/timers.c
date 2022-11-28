#include "timers.h"

uint64_t ts_millis() {
  struct timespec t;
  clock_gettime(CLOCK_REALTIME, &t);
  return t.tv_sec * 1000 + (t.tv_nsec + 500000) / 1000000;
}

void start_timer(timer_t *timerid) {
  struct itimerspec value;

  value.it_value.tv_sec = 0;
  value.it_value.tv_nsec = 100 * 1000 * 1000;

  value.it_interval.tv_sec = value.it_value.tv_sec;
  value.it_interval.tv_nsec = value.it_value.tv_nsec;

  timer_create(CLOCK_REALTIME, NULL, timerid);

  timer_settime(*timerid, 0, &value, NULL);
}

void stop_timer(timer_t *timerid) {
  struct itimerspec value;

  value.it_value.tv_sec = 0;
  value.it_value.tv_nsec = 0;

  value.it_interval.tv_sec = 0;
  value.it_interval.tv_nsec = 0;

  timer_settime(*timerid, 0, &value, NULL);
}
