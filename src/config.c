#include "config.h"

void read_config(mqtt_conf_t *mqtt_config, task_t *tasks, vnet_t *vnets) {
  FILE *fp;
  char line[256];
  char *ch;
  uint8_t pos;
  uint8_t qi = 0; // query stanza instance
  uint8_t vi = 0; // vnet stanza instance

  /* opening file for reading */
  fp = fopen("rapidbusd.conf", "r");
  if (fp == NULL) {
    perror("Error opening config file");
    exit(9);
  }
  // getting list of queries from config file
  while (fgets(line, 256, fp) != NULL) {
    if (line[0] == 'm') {
      for (uint8_t a = 0; a < strlen(line); a++) {
        if (((uint8_t)line[a] < 44) || ((uint8_t)line[a] > 122)) {
          printf("Found strange character on position %u - ending query line "
                 "definition on this position!\n",
                 a);
          line[a] = '\0';
        }
      }
      printf("Parsing config line: %s\n", line);
      pos = 0;
      ch = strtok(line, ",");
      while (ch != NULL) {
        switch (pos) {
        case 0:
          printf("Query line mark: %s (pos %u)\n", ch, pos);
          break;
        case 1:
          printf("Sanitizing address: %s (%u)\n", ch, pos);
          strncpy(mqtt_config->addr, ch, sizeof(mqtt_config->addr) - 1);
          break;
        case 2:
          printf("Sanitizing client ID: %s (%u)\n", ch, pos);
          strncpy(mqtt_config->client_id, ch, sizeof(mqtt_config->client_id) - 1);
          break;
        case 3:
          printf("Sanitizing MQTT topic: %s (%u)\n", ch, pos);
          strncpy(mqtt_config->topic, ch, sizeof(mqtt_config->topic) - 1);
          break;
        default:
          printf("We should never get here when parsing config file - probably "
                 "config file is incorrectly defined (maybe too many positions "
                 "in line?). Pos: %u\n",
                 pos);
          exit(11);
        }
        ch = strtok(NULL, ",");
        pos++;
      }
      if (pos != 4) {
        printf("Problem parsing config line! Needs 4 configuration positions for "
               "query definition, but found %i\n",
               pos);
        exit(10);
      }
    } else if (line[0] == 'q') {
      for (uint8_t a = 0; a < strlen(line); a++) {
        if (((uint8_t)line[a] < 44) || ((uint8_t)line[a] > 122)) {
          printf("Found strange character on position %u - ending query line "
                 "definition on this position!\n",
                 a);
          line[a] = '\0';
        }
      }
      printf("Parsing config line: %s\n", line);
      pos = 0;
      ch = strtok(line, ",");
      while (ch != NULL) {
        switch (pos) {
        case 0:
          printf("Query line mark: %s (pos %u)\n", ch, pos);
          break;
        case 1:
          printf("Sanitizing nodename: %s (pos %u)\n", ch, pos);
          strncpy(tasks[qi].node_name, ch, sizeof(tasks[qi].node_name) - 1);
          break;
        case 2:
          printf("Sanitizing network: %s (pos %u)\n", ch, pos);
          strncpy(tasks[qi].vnet, ch, sizeof(tasks[qi].vnet) - 1);
          break;
        case 3:
          printf("Sanitizing query name: %s (pos: %u)\n", ch, pos);
          strncpy(tasks[qi].query_name, ch, sizeof(tasks[qi].query_name) - 1);
          break;
        case 4:
          printf("Sanitizing MODBUS ID: %s (%u)\n", ch, pos);
          tasks[qi].modbus_id = atoi(ch);
          break;
        case 5:
          printf("Sanitizing MODBUS funct: %s (%u)\n", ch, pos);
          tasks[qi].modbus_function = atoi(ch);
          break;
        case 6:
          printf("Sanitizing MODBUS start reg: %s (%u)\n", ch, pos);
          tasks[qi].start_register = atoi(ch);
          break;
        case 7:
          printf("Sanitizing MODBUS req. count: %s (%u)\n", ch, pos);
          tasks[qi].wcount = atoi(ch);
          break;
        case 8:
          printf("Sanitizing MODBUS response offset: %s (%u)\n", ch, pos);
          tasks[qi].offset = atoi(ch);
          break;
        case 9:
          printf("Interpret returned data as: %s (%u)\n", ch, pos);
          if (strcmp("f32", ch) == 0) {
            printf("Interpret as 32 bit float");
            tasks[qi].interpret_as = f32;
          } else {
            printf("Unknown type to interpret: %s", ch);
            exit(12);
          }
          break;
        case 10:
          printf("Sanitizing period %s (%u)\n", ch, pos);
          tasks[qi].period_ms = atoi(ch);
          break;
        default:
          printf("We should never get here when parsing config file - probably "
                 "config file is incorrectly defined (maybe too many positions "
                 "in line?). Pos: %u\n",
                 pos);
          exit(11);
        }
        ch = strtok(NULL, ",");
        pos++;
      }
      if (pos != 11) {
        printf("Problem parsing config line! Needs 10 configuration positions for "
               "query definition, but found %i\n",
               pos);
        exit(10);
      }
      tasks[qi].state = 1; // enable this task index
      qi++;
    } else if (line[0] == 'v') {
      // vnet definition line in config file
      for (uint8_t a = 0; a < strlen(line); a++) {
        if (((uint8_t)line[a] < 44) || ((uint8_t)line[a] > 122)) {
          printf("Found strange character on position %u - ending vnet line "
                 "definition on this position!\n",
                 a);
          line[a] = '\0';
        }
      }
      printf("Parsing config line: %s\n", line);
      pos = 0;
      ch = strtok(line, ",");
      while (ch != NULL) {
        switch (pos) {
        case 0:
          printf("Virtual network line mark: %s (pos %u)\n", ch, pos);
          break;
        case 1:
          printf("Sanitizing vnet name: %s (pos %u)\n", ch, pos);
          strncpy(vnets[vi].name, ch, sizeof(vnets[vi].name) - 1);
          break;
        case 2:
          printf("Sanitizing port path: %s (pos %u)\n", ch, pos);
          strncpy(vnets[vi].port, ch, sizeof(vnets[vi].port) - 1);
          break;
        case 3:
          printf("Sanitizing baud: %s (pos: %u)\n", ch, pos);
          vnets[vi].baudrate = atoi(ch);
          break;
        case 4:
          printf("Sanitizing serial config: %s (%u)\n", ch, pos);
          strncpy(vnets[vi].serial_config, ch, sizeof(vnets[vi].serial_config) - 1);
          break;
        default:
          printf("We should never get here when parsing config file - probably "
                 "config file is incorrectly defined (maybe too many positions "
                 "in line?). Pos: %u\n",
                 pos);
          exit(11);
        }
        ch = strtok(NULL, ",");
        pos++;
      }
      if (pos != 6) {
        printf("Problem parsing config line! Needs 5 configuration positions for "
               "virtual network definition, but found %i\n",
               pos);
        exit(10);
      }
      vnets[vi].state = 1; // enable this task index
      vi++;
    } else {
      continue;
    }
    printf("MQTT config loaded from config file:\n");
    printf("  Address: %s\n  Client ID: %s\n  Pub topic: %s\n", mqtt_config->addr,
           mqtt_config->client_id, mqtt_config->topic);
    printf("Virtual networks loaded from config file:\n");
    for (uint8_t i = 0; i < vi; i++) {
      printf("  Network name: %s\n  Network port: %s\n Net baudrate: %i\n  "
             "Serial config: %s\n",
             vnets[i].name, vnets[i].port, vnets[i].baudrate, vnets[i].serial_config);
    }
    printf("Tasks loaded from config file:\n");
    for (uint8_t a = 0; a < qi; a++) {
      printf("=>Task ID: %u\n  MODBUS ID: %u\n  MODBUS function: %u\n  Start "
             "register:%u\n  "
             "Number of words to read: %u\n  Query period [ms]: %i\n  "
             "Interpret as: %u\n",
             a, tasks[a].modbus_id, tasks[a].modbus_function, tasks[a].start_register,
             tasks[a].wcount, tasks[a].period_ms, tasks[a].interpret_as);
    }
  }
  fclose(fp);
}
