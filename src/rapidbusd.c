#include "config.h"
#include "errors.h"
#include "modbus.h"
#include "mqtt.h"
#include "timers.h"
#include <bsd/string.h>
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

task_t tasks[MAX_TASKS_COUNT];
vnet_t vnets[MAX_VNETS_COUNT];

volatile MQTTAsync_token deliveredtoken;
int finished = 0;
MQTTAsync client;
uint8_t mqtt_connected = 0;
mqtt_conf_t mqtt_config;

timer_t timerid;
int ser;

// helper union to interpert 4 bytes in different types
union {
  float f;
  uint32_t u;
  int32_t i;
  uint8_t b[4];
} b4_helper;

void mqtt_connect_to(mqtt_conf_t *mqtt_config) {
  MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
  // MQTTAsync_token token;
  int rc;
  MQTTAsync_create(&client, mqtt_config->addr, mqtt_config->client_id, MQTTCLIENT_PERSISTENCE_NONE,
                   NULL);
  MQTTAsync_setCallbacks(client, NULL, mqtt_connlost, NULL, NULL);
  conn_opts.keepAliveInterval = 20;
  conn_opts.cleansession = 1;
  conn_opts.onSuccess = mqtt_onConnect;
  conn_opts.onFailure = mqtt_onConnectFailure;
  conn_opts.context = client;
  if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS) {
    printf("Failed to start connect, return code %d\n", rc);
    exit(ERROR_MQTT_CONNECT);
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
  options.c_cflag &= ~CSIZE; // make sure the two bit values for the length of data is reset

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

float get_modbus_data(uint8_t *modbus_request, uint8_t r_count, uint8_t *rb) {
  uint32_t wait_for_response_for_ms = 40;

  printf("Requesting data... ");
  for (uint8_t a = 0; a < r_count; a++) {
    printf(" %X", modbus_request[a]);
  }
  printf("\n");
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
  usleep(wait_for_response_for_ms * 1000);

  uint8_t bytes_avail;
  ioctl(ser, FIONREAD, &bytes_avail);
  if (!bytes_avail) {
    printf("PROBLEM: Sensor did not respond within %ums. Skipping read.\n",
           wait_for_response_for_ms);
    return -1;
  }
  int8_t readbytes = read(ser, rb, bytes_avail);
  printf("Received data  ...  ");
  for (uint8_t a = 0; a < readbytes; a++) {
    printf("%X ", rb[a]);
  }
  printf("\n");

  // check MODBUS CRC
  if (!checkModbusCRC(rb, readbytes - 2, rb[readbytes - 2], rb[readbytes - 1])) {
    printf("CRC not correct - skipping parsing the modbus packet.\n");
    return -1;
  };

  for (uint8_t a = 3; a < 3 + 4; a++) {
    b4_helper.b[a - 3] = rb[a];
  }
  printf("Value: %f\n", b4_helper.f);
  // memset(rb, 0, sizeof(rb));
  // return readbytes;
  return b4_helper.f;
}

void timer_callback(__attribute__((unused)) int sig) {
  char msg[1024];
  float float_value;
  uint8_t ret_modbus_data[1024];
  uint64_t ms = ts_millis();
  uint8_t modbus_request[] = {tasks[0].modbus_id,
                              tasks[0].modbus_function,
                              tasks[0].start_register >> 8,
                              tasks[0].start_register & 0xFF,
                              tasks[0].wcount >> 8,
                              tasks[0].wcount & 0xFF,
                              0x14,
                              0x09};

  stop_timer(&timerid);
  for (uint16_t a = 0; a < MAX_TASKS_COUNT; a++) {
    if (tasks[a].state == 1) {
      /*
            printf("Evaluating task ID %d for execution ... period_ms: %i last_run: "
                   "%li\n",
                   a, tasks[a].period_ms, tasks[a].last_run);
      */
      if ((uint64_t)tasks[a].period_ms <= ms - tasks[a].last_run) {
        printf("Decided to execute task ID %d query_name: %s @%lu period: %ims\n", a,
               tasks[a].query_name, ms, tasks[a].period_ms);
        if (tasks[a].interpret_as == f32) {
          float_value = get_modbus_data(modbus_request, sizeof(modbus_request), ret_modbus_data);
          sprintf(msg, MQTT_PAYLOAD_FLOAT, tasks[a].node_name, tasks[a].query_name, float_value,
                  ts_millis());
        } else {
          fprintf(stderr, "ERROR: Unsupported INTERPRET_AS (%i) for task %s ... skipping.\n",
                  tasks[a].interpret_as, tasks[a].query_name);
          continue;
        }
        msg[sizeof(msg) - 1] = '\0'; // just to be sure
        printf("%s\n", msg);
        mqtt_pubMsg(msg, strlen(msg));
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

  while ((opt = getopt(argc, argv, "c:h")) != -1)
    switch (opt) {
    case 'c':
      printf("Option -c (config file path) was provided with a value of \"%s\"\n", optarg);
      strlcpy(config_file, optarg, sizeof(config_file));
      break;
    case 'h':
      printf("RapidBus (from rapidbus.org) version: %s\n", RAPIDBUS_VERSION);
      printf("Usage: %s [-c config_file]\n", argv[0]);
      exit(EXIT_OK);
    default:
      fprintf(stderr, "Usage: %s [-c config_file]\n", argv[0]);
      exit(ERROR_USAGE);
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

  mqtt_connect_to(&mqtt_config);

  while (!mqtt_connected) {
    printf("Waiting for MQTT connection to broker...\n");
    sleep(1);
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
      sleep(5);
      if (mqtt_connected) {
        printf("Reconnected to MQTT broker! Restarting timer...\n");
        start_timer(&timerid);
      } else {
        printf("Reconnection to MQTT broker failed! Try again in 10s...\n");
        sleep(10);
      }
    }
  }

  close(ser);
}
