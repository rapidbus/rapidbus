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

/**
void timer_callback(int sig) {
  uint64_t ms = ts_millis();
  printf("Sig called: %i %li\n", sig, ms);
  for (uint8_t a = 0; a < MAX_TASKS_COUNT; a++) {
    if (ml_tasks[a].state == 1) {
      printf(" Task ID %d period_ms: %i last_run: %li\n", a,
             ml_tasks[a].period_ms, ml_tasks[a].last_run);
      if (ml_tasks[a].period_ms <= ms - ml_tasks[a].last_run) {
        printf("!!! Task ID %d name: %s\n", a, ml_tasks[a].name);
        ml_tasks[a].last_run = ms;
      }
    }
  }
}
**/

/**
int main(int ac, char **av) {
  // initial struct def values
  for (uint8_t a = 0; a < MAX_TASKS_COUNT; a++) {
    ml_tasks[a].state = 0;
  }

  printf("Initialize default options\n");

  strcpy(ml_tasks[0].name, "task1");
  ml_tasks[0].state = 1;
  ml_tasks[0].period_ms = 50;
  ml_tasks[0].last_run = ts_millis();

  strcpy(ml_tasks[1].name, "task2");
  ml_tasks[1].state = 1;
  ml_tasks[1].period_ms = 500;
  ml_tasks[2].last_run = ts_millis();

  strcpy(ml_tasks[2].name, "task3");
  ml_tasks[2].state = 1;
  ml_tasks[2].period_ms = 5000;
  ml_tasks[2].last_run = ts_millis();

  (void)signal(SIGALRM, timer_callback);
  start_timer();
  while (1)
    ;
}
**/
