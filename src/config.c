#include "config.h"

void read_config(struct query_t *query) {
  FILE *fp;
  char line[256];
  char *ch;
  uint8_t pos;

  /* opening file for reading */
  fp = fopen("rapidbusd.conf", "r");
  if (fp == NULL) {
    perror("Error opening config file");
    exit(9);
  }
  // getting list of queries from config file
  while (fgets(line, 256, fp) != NULL) {
    if (line[0] != 'q') {
      continue;
    }
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
        printf("aha pos %i\n", pos);
        break;
      case 1:
        printf("aha pos %i\n", pos);
        break;
      case 2:
        printf("aha pos %i\n", pos);
        break;
      case 3:
        printf("Sanitizing line position %u (%s)...", pos, ch);
        query->modbus_id = atoi(ch);
        printf("done.\n");
        break;
      case 4:
        printf("Sanitizing line position %u (%s)...", pos, ch);
        query->modbus_function = atoi(ch);
        printf("done.\n");
        break;
      case 5:
        printf("Sanitizing line position %u (%s)...", pos, ch);
        query->start_register = atoi(ch);
        printf("done.\n");
        break;
      case 6:
        printf("Sanitizing line position %u (%s)...", pos, ch);
        query->wcount = atoi(ch);
        printf("done.\n");
        break;
      case 7:
        printf("Sanitizing line position %u (%s)...", pos, ch);
        query->period_ms = atoi(ch);
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
    if (pos != 8) {
      printf("Problem parsing config line! Needs 8 configuration positions for "
             "query definition, but found %i\n",
             pos);
      exit(10);
    }
    printf("Query loaded from config file:\n");
    printf("  MODBUS ID: %u\n  MODBUS function: %u\n  Start register:%u\n  "
           "Number of words to read: %u\n  Query period: %u\n",
           query->modbus_id, query->modbus_function, query->start_register,
           query->wcount, query->period_ms);
  }
  fclose(fp);
}
