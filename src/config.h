#ifndef CONFIG_HEADER_FILE
#define CONFIG_HEADER_FILE

#include <stdint.h> /* types definition */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct task_t {
  uint8_t state;
  uint64_t last_run;
  int period_ms;
  char node_name[255];
  char query_name[255];
  uint8_t modbus_id;
  uint8_t modbus_function;
  uint8_t start_register;
  uint8_t wcount;
} task_t;

void read_config(task_t *tasks);

#endif
