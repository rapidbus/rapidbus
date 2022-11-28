#ifndef CONFIG_HEADER_FILE
#define CONFIG_HEADER_FILE

#include <stdint.h> /* types definition */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct query_t {
  char node_name[255];
  char query_name[255];
  uint8_t modbus_id;
  uint8_t modbus_function;
  uint8_t start_register;
  uint8_t wcount;
  int period_ms;
};

void read_config(struct query_t *query);

#endif
