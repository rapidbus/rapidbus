#ifndef CONFIG_HEADER_FILE
#define CONFIG_HEADER_FILE

#include <stdint.h> /* types definition */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum ias { f32, s32, u32, s16 };

typedef struct mqtt_conf_t {
  char addr[256];
  char client_id[256];
  char topic[512];
  uint8_t qos;
  uint32_t timeout;
} mqtt_conf_t;

typedef struct task_t {
  uint8_t state;
  uint64_t last_run;
  char vnet[256];
  char node_name[256];
  char query_name[256];
  uint8_t modbus_id;
  uint8_t modbus_function;
  uint8_t start_register;
  uint8_t wcount;
  uint8_t offset;
  uint8_t interpret_as;
  int period_ms;
} task_t;

typedef struct vnet_t {
  uint8_t state;
  char name[256];
  char port[256];
  uint16_t baudrate;
  char serial_config[256];
} vnet_t;

void read_config(mqtt_conf_t *mqtt_config, task_t *tasks, vnet_t *vnets);

#endif
