#include "config.h"
#include "errors.h"
#include "modbus.h"
#include "mqtt.h"
#include "timers.h"
#include <errno.h> /* Error number definitions */
#include <fcntl.h> /* File control definitions */
#include <getopt.h>
#include <stdint.h> /* types definition */
#include <stdio.h>  /* Standard input/output definitions */
#include <stdlib.h> /* Standard input/output definitions */
#include <string.h> /* String function definitions */
#include <sys/ioctl.h>
#include <termios.h> /* POSIX terminal control definitions */
#include <unistd.h>  /* UNIX standard function definitions */

#define MAX_TASKS_COUNT 256
#define MAX_VNETS_COUNT 16

#define CSV_PAYLOAD_FLOAT "%s,%s,%f,%lu,"

task_t tasks[MAX_TASKS_COUNT];
vnet_t vnets[MAX_VNETS_COUNT];

volatile MQTTAsync_token deliveredtoken;
int finished = 0;
MQTTAsync client;
uint8_t mqtt_connected = 0;
mqtt_conf_t mqtt_config;

uint8_t verbose = 0;
uint8_t data_to_file = 0;

FILE *data_file_fp; // file pointer to file where collected data will be written to if so requested by argument

timer_t timerid;
int ser;

// helper union to interpert 4 bytes in different types
union {
  float f;
  uint32_t u;
  int32_t i;
  uint8_t b[4];
} b4_helper;

void mqtt_connect_to(const mqtt_conf_t *mqtt_lconfig) {
  MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
  int rc;
  MQTTAsync_create(&client, mqtt_lconfig->addr, mqtt_lconfig->client_id, MQTTCLIENT_PERSISTENCE_NONE, NULL);
  MQTTAsync_setCallbacks(client, NULL, mqtt_connlost, NULL, NULL);
  conn_opts.struct_version = 6;
  conn_opts.keepAliveInterval = 20;
  conn_opts.cleansession = 1;
  conn_opts.onSuccess = mqtt_onConnect;
  conn_opts.onFailure = mqtt_onConnectFailure;
  conn_opts.context = client;
  rc = MQTTAsync_connect(client, &conn_opts);
  if (rc == MQTTASYNC_SUCCESS) {
    printf("MQTTASYNC_SUCCESS\n");
  } else if (rc == MQTTASYNC_BAD_STRUCTURE) {
    printf("MQTTASYNC_BAD_STRUCTURE (%d) returned, MQTTAsync_connectOptions debug:\n", rc);
    printf("->struct_id      : %s\n", conn_opts.struct_id);
    printf("->struct_version : %d\n", conn_opts.struct_version);
    mqtt_connected = 0;
  } else {
    printf("Failed to start MQTTAsync_connect, return code: %d\n", rc);
    mqtt_connected = 0;
  }
}

int open_port(void) {
  int fd; /* File descriptor for the port */
  char serial_port[256];

  // serial port will always be the same for all vnets!!!
  // this is because its difficult to manage access
  // to shared medium such as serial port
  strlcpy(serial_port, vnets[0].port, sizeof(serial_port));

  printf("About to open serial port %s\n", serial_port);
  fd = open(serial_port, O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd == -1) {
    printf("Failed opening serial port: %s\n", serial_port);
    perror("");
    exit(ERROR_OPEN_SERIAL_PORT);
  }

  printf("Setting up serial port\n");
  // Flush away any bytes previously read or written.
  int result = tcflush(fd, TCIOFLUSH);
  if (result) {
    perror("tcflush failed"); // just a warning, not a fatal error
  }

  if (fcntl(fd, F_SETFL, FNDELAY) == -1) {
    printf("Error setting up serial interface. Error number: %i\n", errno);
    exit(ERROR_SETUP_SERIAL_PORT);
  }

  struct termios options;

  /*
   * Get the current options for the port...
   */
  tcgetattr(fd, &options);

  options.c_iflag &= ~(INLCR | IGNCR | ICRNL | IXON | IXOFF);
  options.c_oflag &= ~(ONLCR | OCRNL | OPOST);
  options.c_lflag &= ~(ECHO | ECHONL | ECHOE | ICANON | ISIG | IEXTEN);

  options.c_cflag |= (CLOCAL | CREAD); // Ignore modem control lines; Enable receiver.
  options.c_cflag &= ~CSIZE;           // make sure the two bit values for the length of data is reset

  if (strcmp("8N1", vnets[0].serial_config) == 0) {
    options.c_cflag &= ~CSTOPB; // disable TWO stop bits (left with one)
    options.c_cflag &= ~PARENB; // disable parity
    options.c_cflag |= CS8;     // Set character size mask (8bits)
  } else {
    printf("Unknow settings for serial interface for network %s - dont know what \"%s\" means. Fix "
           "in config file.\n",
           vnets[0].name, vnets[0].serial_config);
    exit(ERROR_CONFIG_FILE);
  }

  // Set up timeouts: Calls to read() will return as soon as there is
  // at least one byte available or when 100 ms has passed.
  // options.c_cc[VTIME] = 1;
  // options.c_cc[VMIN] = 0;

  printf("Setting baud for virtual network %s to %u.\n", vnets[0].name, vnets[0].baudrate);
  switch (vnets[0].baudrate) {
  case 4800:
    cfsetospeed(&options, B4800);
    break;
  case 9600:
    cfsetospeed(&options, B9600);
    break;
  case 19200:
    cfsetospeed(&options, B19200);
    break;
  case 38400:
    cfsetospeed(&options, B38400);
    break;
  case 115200:
    cfsetospeed(&options, B115200);
    break;
  default:
    fprintf(stderr,
            "warning: Baud rate %u is not supported by RapidBus. Change setting forvirtuan network "
            "%s in config file!.\n",
            vnets[0].baudrate, vnets[0].name);
    exit(ERROR_BAUD_NOT_SUPPORTED);
  }
  cfsetispeed(&options, cfgetospeed(&options));

  result = tcsetattr(fd, TCSANOW, &options);
  if (result) {
    perror("tcsetattr failed");
    close(fd);
    exit(ERROR_TCSETATTR_FAILED);
  }
  printf("Done setting up serial interface.\n");

  return (fd);
}

int8_t get_modbus_data(uint8_t *modbus_request, uint8_t r_count, uint8_t *ret) {
  uint32_t wait_for_response_for_ms = 40;

  if (verbose) {
    printf("Requesting data... ");
    for (uint8_t a = 0; a < r_count; a++) {
      printf(" %X", modbus_request[a]);
    }
    printf("\n");
  }
  // flushing old data
  int result = tcflush(ser, TCIOFLUSH);
  if (result) {
    perror("PROBLEM!! tcflush failed - looks like problem with serial subsystem on OS!");
    exit(ERROR_TCFLUSH_FAILED);
  }
  int8_t n = write(ser, modbus_request, r_count);
  if (n != r_count) {
    printf("write() of %u bytes failed!\n", r_count);
    perror("write() failed!\n");
  }
  nanosleep((const struct timespec[]){{0, wait_for_response_for_ms * 1000000L}}, NULL);

  uint16_t bytes_avail;
  uint8_t rx_buffer[1024];

  ioctl(ser, FIONREAD, &bytes_avail);
  if (!bytes_avail) {
    if (verbose) {
      printf("PROBLEM: Sensor did not respond within %ums. Skipping read.\n", wait_for_response_for_ms);
    }
    return -2;
  }

  if (bytes_avail > sizeof(rx_buffer)) {
    printf("PROBLEM: Sensor sent more than %zu bytes. Skipping read.\n", sizeof(rx_buffer));
    return -1;
  }
  int16_t readbytes = read(ser, rx_buffer, bytes_avail);

  if (readbytes == -1) {
    printf("PROBLEM: Reading from serial interface returned error. Skipping read.\n");
    return -1;
  }
  if (verbose) {
    printf("Received data  ...  ");
    for (int16_t a = 0; a < readbytes; a++) {
      printf("%X ", rx_buffer[a]);
    }
    printf("\n");
  }

  // check MODBUS CRC
  if (!checkModbusCRC(rx_buffer, readbytes - 2, rx_buffer[readbytes - 2], rx_buffer[readbytes - 1])) {
    printf("CRC not correct - skipping parsing the modbus packet.\n");
    return -1;
  };

  // copy the data to the return buffer
  for (uint8_t a = 3; a < 3 + 4; a++) {
    ret[a - 3] = rx_buffer[a];
  }

  // clear the buffer to prevent data from leaking
  memset(rx_buffer, 0, sizeof(rx_buffer));
  return 1;
}

void timer_callback(__attribute__((unused)) int sig) {
  char mqtt_msg[1024];
  char csv_msg[1024];
  float float_value = 0;
  int8_t retval;
  uint64_t ms = ts_millis();
  uint8_t modbus_request[] = {0, 0, 0, 0, 0, 0, 0, 0};

  stop_timer(&timerid);
  for (uint16_t a = 0; a < MAX_TASKS_COUNT; a++) {
    if (tasks[a].state == 1) {
      /*
            printf("Evaluating task ID %d for execution ... period_ms: %i last_run: "
                   "%li\n",
                   a, tasks[a].period_ms, tasks[a].last_run);
      */
      if ((uint64_t)tasks[a].period_ms <= ms - tasks[a].last_run) {
        if (verbose) {
          printf("Decided to execute task ID %d query_name: %s @%lu period: %ims\n", a, tasks[a].query_name, ms,
                 tasks[a].period_ms);
        }
        modbus_request[0] = tasks[a].modbus_id;
        modbus_request[1] = tasks[a].modbus_function;
        modbus_request[2] = tasks[a].start_register >> 8;
        modbus_request[3] = tasks[a].start_register & 0xFF;
        modbus_request[4] = tasks[a].wcount >> 8;
        modbus_request[5] = tasks[a].wcount & 0xFF;
        getModbusCRC(modbus_request, sizeof(modbus_request) - 2, &modbus_request[6], &modbus_request[7]);

        uint8_t ret_data[tasks[a].wcount * 2];

        retval = get_modbus_data(modbus_request, sizeof(modbus_request), ret_data);
        if (retval != 1) {
          printf("Unable to read query '%s' from sensor '%s'. Use -v argument for more info.\n", tasks[a].query_name,
                 tasks[a].node_name);
          tasks[a].last_run = ms;
          continue;
        }

        if (tasks[a].interpret_as == f32) {
          memcpy(b4_helper.b, ret_data, sizeof(b4_helper.b));
          float_value = b4_helper.f;
          sprintf(mqtt_msg, MQTT_PAYLOAD_FLOAT, tasks[a].node_name, tasks[a].query_name, float_value, ts_millis());
          sprintf(csv_msg, CSV_PAYLOAD_FLOAT, tasks[a].node_name, tasks[a].query_name, float_value, ts_millis());
        } else {
          fprintf(stderr, "ERROR: Unsupported INTERPRET_AS (%i) for task %s ... skipping.\n", tasks[a].interpret_as,
                  tasks[a].query_name);
          tasks[a].last_run = ms;
          continue;
        }
        mqtt_msg[sizeof(mqtt_msg) - 1] = '\0'; // just to be sure that there is a proper string ending
        csv_msg[sizeof(csv_msg) - 1] = '\0';   // just to be sure that there is a proper string ending
        printf("%s\n", mqtt_msg);
        if (data_to_file) {
          if (verbose) {
            printf("Appending line to file: %s\n", csv_msg);
            fprintf(data_file_fp, "%s\n", csv_msg);
            fflush(NULL);
          }
        }
        mqtt_pubMsg(mqtt_msg, strnlen(mqtt_msg, sizeof(mqtt_msg)));
        tasks[a].last_run = ms;
      }
    }
  }
  start_timer(&timerid);
}

int main(int argc, char *argv[]) {

  int opt;
  char config_file[256];

  strlcpy(config_file, "/etc/rapidbusd.conf", sizeof(config_file));

  while ((opt = getopt(argc, argv, "c:hsv")) != -1)
    switch (opt) {
    case 'c':
      printf("Option -c (config file path) was provided with a value of \"%s\"\n", optarg);
      strlcpy(config_file, optarg, sizeof(config_file));
      break;
    case 'h':
      printf("RapidBus (from rapidbus.org) version: %s\nMODBUS RTU to MQTT bridge\n", RAPIDBUS_VERSION);
      printf("Usage: %s [-c config_file] [-v]\n", argv[0]);
      printf("  -c <config_file>   Path to config file. Default: /etc/rapidbusd.conf\n"
             "  -v                 Be verbose in output. Default: not set\n");
      exit(EXIT_OK);
    case 's':
      printf("Saving data to file enabled\n");
      data_to_file = 1;
      break;
    case 'v':
      printf("Verbose output enabled\n");
      verbose = 1;
      break;
    default:
      fprintf(stderr, "Usage: %s [-c config_file] [-v]\n", argv[0]);
      exit(ERROR_USAGE);
    }

  if (data_to_file) {
    if ((data_file_fp = fopen("data_file.csv", "a")) == NULL) {
      printf("Unable to open data file for append operation. fopen() error number: %i\n", errno);
      exit(ERROR_OPEN_FILE_FAILED);
    }
  }

  printf("Initial struct def values for config (max tasks: %i)\n", MAX_TASKS_COUNT);
  for (uint16_t a = 0; a < MAX_TASKS_COUNT; a++) {
    tasks[a].state = 0;
  }

  printf("Initial struct def values for virtual networks (max networks: %i)\n", MAX_VNETS_COUNT);
  for (uint16_t a = 0; a < MAX_VNETS_COUNT; a++) {
    vnets[a].state = 0;
  }

  printf("Initialize configuration\n");
  read_config(config_file, &mqtt_config, tasks, vnets);

  while (!mqtt_connected) {
    mqtt_connect_to(&mqtt_config);
    sleep(5);
    if (mqtt_connected) {
      printf("Connected to MQTT broker!\n");
    } else {
      printf("Connection to MQTT broker failed! Try again in 10s...\n");
      sleep(10);
    }
  }

  start_timer(&timerid);
  (void)signal(SIGALRM, timer_callback);
  ser = open_port();
  printf("Port opened\n");

  while (1) {
    while (!mqtt_connected) {
      stop_timer(&timerid);
      printf("Disconnected from MQTT broker! Try to re-initialize "
             "connection...\n");
      mqtt_connect_to(&mqtt_config);
      sleep(10);
      if (mqtt_connected) {
        printf("Reconnected to MQTT broker! Restarting timer...\n");
        start_timer(&timerid);
      } else {
        printf("Reconnection to MQTT broker failed! Try again...\n");
      }
    }
    sleep(10); // this will be interrupted by SIGALRM
  }

  close(ser);
}
