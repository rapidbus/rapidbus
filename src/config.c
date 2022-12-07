#include "config.h"

void read_config(char *config_file, mqtt_conf_t *mqtt_config, task_t *tasks, vnet_t *vnets) {
  FILE *fp;
  char line[256];
  char *ch;
  uint8_t pos;
  uint8_t qi = 0; // query stanza instance
  uint8_t vi = 0; // vnet stanza instance

  /* opening file for reading */
  fp = fopen(config_file, "r");
  if (fp == NULL) {
    fprintf(stderr, "Error opening config file: %s\n", config_file);
    perror("");
    exit(9);
  }
  // getting list of queries from config file
  while (fgets(line, sizeof(line), fp) != NULL) {
    line[sizeof(line) - 1] = '\0'; // just to make sure
    if (line[0] == 'm') {
      for (size_t a = 0; a < strnlen(line, sizeof(line)); a++) {
        if (((uint8_t)line[a] < 44) || ((uint8_t)line[a] > 122)) {
          printf("Found strange character on position %zu => ending query line "
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
          printf("MQTTT config line mark: %s (pos %u)\n", ch, pos);
          break;
        case 1:
          printf("Sanitizing address: %s (%u)\n", ch, pos);
          strlcpy(mqtt_config->addr, ch, sizeof(mqtt_config->addr));
          break;
        case 2:
          printf("Sanitizing client ID: %s (%u)\n", ch, pos);
          strlcpy(mqtt_config->client_id, ch, sizeof(mqtt_config->client_id));
          break;
        case 3:
          printf("Sanitizing MQTT topic: %s (%u)\n", ch, pos);
          strlcpy(mqtt_config->topic, ch, sizeof(mqtt_config->topic));
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
      for (size_t a = 0; a < strnlen(line, sizeof(line)); a++) {
        if (((uint8_t)line[a] < 44) || ((uint8_t)line[a] > 122)) {
          printf("Found strange character on position %zu - ending query line "
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
          strlcpy(tasks[qi].node_name, ch, sizeof(tasks[qi].node_name));
          break;
        case 2:
          printf("Sanitizing network: %s (pos %u)\n", ch, pos);
          strlcpy(tasks[qi].vnet, ch, sizeof(tasks[qi].vnet));
          break;
        case 3:
          printf("Sanitizing query name: %s (pos: %u)\n", ch, pos);
          strlcpy(tasks[qi].query_name, ch, sizeof(tasks[qi].query_name));
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
            printf("Interpret as 32 bit float\n");
            tasks[qi].interpret_as = f32;
          } else if (strcmp("u32", ch) == 0) {
            printf("Interpret as 32 bit unsigned int\n");
            tasks[qi].interpret_as = u32;
          } else if (strcmp("u16", ch) == 0) {
            printf("Interpret as 16 bit unsigned int\n");
            tasks[qi].interpret_as = u16;
          } else if (strcmp("u8", ch) == 0) {
            printf("Interpret as 8 bit unsigned int\n");
            tasks[qi].interpret_as = u8;
          } else if (strcmp("s32", ch) == 0) {
            printf("Interpret as 32 bit signed int\n");
            tasks[qi].interpret_as = s32;
          } else if (strcmp("s16", ch) == 0) {
            printf("Interpret as 16 bit signed int\n");
            tasks[qi].interpret_as = s16;
          } else if (strcmp("s8", ch) == 0) {
            printf("Interpret as 8 bit signed int\n");
            tasks[qi].interpret_as = s8;
          } else {
            printf("Error in config file: Unknown type to interpret: %s\n", ch);
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
      for (size_t a = 0; a < strnlen(line, sizeof(line)); a++) {
        if (((uint8_t)line[a] < 44) || ((uint8_t)line[a] > 122)) {
          printf("Found strange character on position %zu - ending vnet line "
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
          strlcpy(vnets[vi].name, ch, sizeof(vnets[vi].name));
          break;
        case 2:
          printf("Sanitizing port path: %s (pos %u)\n", ch, pos);
          strlcpy(vnets[vi].port, ch, sizeof(vnets[vi].port));
          break;
        case 3:
          printf("Sanitizing baud: %s (pos: %u)\n", ch, pos);
          vnets[vi].baudrate = atoi(ch);
          break;
        case 4:
          printf("Sanitizing serial config: %s (%u)\n", ch, pos);
          strlcpy(vnets[vi].serial_config, ch, sizeof(vnets[vi].serial_config));
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
      if (pos != 5) {
        printf("Problem parsing config line! Needs 5 configuration positions for "
               "virtual network definition, but found %i\n",
               pos);
        exit(10);
      }
      vnets[vi].state = 1; // enable this task index
      vi++;
    } else {
      if (line[0] != '#') {
        printf("Unknown line in config file: %s\n", line);
        exit(9);
      }
    }
  }
  fclose(fp);
  printf("##################### START Config show\n");
  printf("MQTT config loaded from config file:\n");
  printf("  Address: %s\n  Client ID: %s\n  Pub topic: %s\n", mqtt_config->addr,
         mqtt_config->client_id, mqtt_config->topic);
  if (vi == 0) {
    printf("No serial virtual networks defined in config file! We need at least one!\n");
    exit(13);
  }
  printf("Virtual networks loaded from config file:\n");
  for (uint8_t i = 0; i < vi; i++) {
    printf("  Network name: %s\n  Network port: %s\n  Net baudrate: %u\n  "
           "Serial config: %s\n",
           vnets[i].name, vnets[i].port, vnets[i].baudrate, vnets[i].serial_config);
  }
  if (qi == 0) {
    printf("No sensor queries defined in config file! We need at least one!\n");
    exit(14);
  }
  printf("Tasks loaded from config file:\n");
  for (uint8_t a = 0; a < qi; a++) {
    printf("=>Task ID: %u\n  MODBUS ID: %u\n  MODBUS function: %u\n  Start "
           "register: %u\n  "
           "Number of words to read: %u\n  Query period [ms]: %i\n  "
           "Interpret as: %u\n",
           a, tasks[a].modbus_id, tasks[a].modbus_function, tasks[a].start_register,
           tasks[a].wcount, tasks[a].period_ms, tasks[a].interpret_as);
  }
  printf("##################### END Config show\n");
}
