#include "config.h"

void read_config(task_t *tasks) {
  FILE *fp;
  char line[256];
  char *ch;
  uint8_t pos;
  uint8_t qi = 0; // query stanza instance

  /* opening file for reading */
  fp = fopen("rapidbusd.conf", "r");
  if (fp == NULL) {
    perror("Error opening config file");
    exit(9);
  }
  // getting list of queries from config file
  while (fgets(line, 256, fp) != NULL) {
    if (line[0] == 'q') {
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
          printf("query mark %s position %i\n", ch, pos);
          break;
        case 1:
          printf("nodename %s %i\n", ch, pos);
          break;
        case 2:
          printf("network %s %i\n", ch, pos);
          break;
        case 3:
          printf("query name %s %i\n", ch, pos);
          break;
        case 4:
          printf("Sanitizing MODBUS ID %u (%s)...", pos, ch);
          tasks[qi].modbus_id = atoi(ch);
          printf("done.\n");
          break;
        case 5:
          printf("Sanitizing line position %u (%s)...", pos, ch);
          tasks[qi].modbus_function = atoi(ch);
          printf("done.\n");
          break;
        case 6:
          printf("Sanitizing line position %u (%s)...", pos, ch);
          tasks[qi].start_register = atoi(ch);
          printf("done.\n");
          break;
        case 7:
          printf("Sanitizing line position %u (%s)...", pos, ch);
          tasks[qi].wcount = atoi(ch);
          printf("done.\n");
          break;
        case 8:
          printf("interpret returned data as %s %i\n", ch, pos);
          break;
        case 9:
          printf("Sanitizing period %u (%s)...", pos, ch);
          tasks[qi].period_ms = atoi(ch);
          printf("done.\n");
          break;
        default:
          printf("We should never get here when parsing config file - probably "
                 "config file is incorrectly defined (maybe too many positions "
                 "in line?).\n");
          printf("Pos: %u\n", pos);
          exit(11);
        }
        ch = strtok(NULL, ",");
        pos++;
      }
      if (pos != 10) {
        printf(
            "Problem parsing config line! Needs 10 configuration positions for "
            "query definition, but found %i\n",
            pos);
        exit(10);
      }
      tasks[qi].state = 1; // enable this task index
      qi++;
    } else {
      continue;
    }
    printf("Task loaded from config file:\n");
    for (uint8_t a = 0; a < qi; a++) {
      printf("  Task ID: %u\n  MODBUS ID: %u\n  MODBUS function: %u\n  Start "
             "register:%u\n  "
             "Number of words to read: %u\n  Query period [ms]: %u\n",
             a, tasks[a].modbus_id, tasks[a].modbus_function,
             tasks[a].start_register, tasks[a].wcount, tasks[a].period_ms);
    }
  }
  fclose(fp);
}
