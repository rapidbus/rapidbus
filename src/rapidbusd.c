#include "modbus.h"
#include "mqtt.h"
#include "timers.h"
#include <errno.h>  /* Error number definitions */
#include <fcntl.h>  /* File control definitions */
#include <stdint.h> /* types definition */
#include <stdio.h>  /* Standard input/output definitions */
#include <stdlib.h> /* Standard input/output definitions */
#include <string.h> /* String function definitions */
#include <sys/ioctl.h>
#include <termios.h> /* POSIX terminal control definitions */
#include <termios.h> /* for changing baud etc */
#include <unistd.h>  /* UNIX standard function definitions */

volatile MQTTAsync_token deliveredtoken;
int finished = 0;
MQTTAsync client;

timer_t timerid;
int ser;

union {
  float f;
  uint8_t b[4];
} float_helper;

struct ml_task ml_tasks[MAX_TASKS_COUNT];

void mqtt_connet_to() {
  MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
  MQTTAsync_token token;
  int rc;
  MQTTAsync_create(&client, MQTT_ADDRESS, MQTT_CLIENTID,
                   MQTTCLIENT_PERSISTENCE_NONE, NULL);
  MQTTAsync_setCallbacks(client, NULL, mqtt_connlost, NULL, NULL);
  conn_opts.keepAliveInterval = 20;
  conn_opts.cleansession = 1;
  conn_opts.onSuccess = mqtt_onConnect;
  conn_opts.onFailure = mqtt_onConnectFailure;
  conn_opts.context = client;
  if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS) {
    printf("Failed to start connect, return code %d\n", rc);
    exit(EXIT_FAILURE);
  }
}

int open_port(void) {
  int fd; /* File descriptor for the port */
  char serial_port[] = "/dev/ttyUSB1";
  // char serial_port[] = "/dev/tty.usbserial-DM0105NG";

  printf("About to open serial port %s\n", serial_port);
  fd = open(serial_port, O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd == -1) {
    printf("Serial port %s\n", serial_port);
    perror("open_port: Unable to open serial port.");
    exit(1);
  }

  printf("Setting up serial port\n");
  // Flush away any bytes previously read or written.
  int result = tcflush(fd, TCIOFLUSH);
  if (result) {
    perror("tcflush failed"); // just a warning, not a fatal error
  }

  fcntl(fd, F_SETFL, FNDELAY);

  struct termios options;

  /*
   * Get the current options for the port...
   */
  tcgetattr(fd, &options);

  options.c_iflag &= ~(INLCR | IGNCR | ICRNL | IXON | IXOFF);
  options.c_oflag &= ~(ONLCR | OCRNL | OPOST);
  options.c_lflag &= ~(ECHO | ECHONL | ECHOE | ICANON | ISIG | IEXTEN);

  // set 8N1 transfer
  /*
   * Enable the receiver and set local mode...
   */
  options.c_cflag |= (CLOCAL | CREAD);
  options.c_cflag &= ~(PARENB | CSTOPB | CSIZE);
  options.c_cflag |= CS8;

  // Set up timeouts: Calls to read() will return as soon as there is
  // at least one byte available or when 100 ms has passed.
  // options.c_cc[VTIME] = 1;
  // options.c_cc[VMIN] = 0;

  cfsetospeed(&options, B38400);
  cfsetispeed(&options, cfgetospeed(&options));

  result = tcsetattr(fd, TCSANOW, &options);
  if (result) {
    perror("tcsetattr failed");
    close(fd);
    return -1;
  }
  printf("Done setting up serial interface.\n");

  return (fd);
}

void get_modbus_data(uint8_t *modbus_request, uint8_t r_count) {
  uint8_t rb[10];
  uint32_t wait_for_response_for_ms = 40;

  // printf("Requesting data...\n");
  // flushing old data
  int result = tcflush(ser, TCIOFLUSH);
  if (result) {
    perror("PROBLEM!! tcflush failed"); // just a warning, not a fatal error
    return;
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
    printf("PROBLEM: Sensor did not respond within %ims. Skipping read.\n",
           wait_for_response_for_ms);
    return;
  }
  int8_t readbytes = read(ser, rb, bytes_avail);
  for (uint8_t a = 0; a < readbytes; a++) {
    printf("%X ", rb[a]);
  }

  // check MODBUS CRC
  if (!checkModbusCRC(rb, readbytes - 2, rb[readbytes - 2],
                      rb[readbytes - 1])) {
    printf("CRC not correct - skipping parsing the modbus packet.\n");
    return;
  };

  for (uint8_t a = 3; a < 3 + 4; a++) {
    float_helper.b[a - 3] = rb[a];
  }
  printf("Value: %f\n", float_helper.f);
  memset(rb, 0, sizeof(rb));
}

void timer_callback(int sig) {
  uint64_t ms = ts_millis();
  uint8_t modbus_request[] = {0x01, 0x03, 0x00, 0x09, 0x00, 0x02, 0x14, 0x09};

  // printf("Sig called: %i %li\n", sig, ms);
  for (uint8_t a = 0; a < MAX_TASKS_COUNT; a++) {
    if (ml_tasks[a].state == 1) {
      // printf(" Task ID %d period_ms: %i last_run: %li\n", a,
      //       ml_tasks[a].period_ms, ml_tasks[a].last_run);
      if (ml_tasks[a].period_ms <= ms - ml_tasks[a].last_run) {
        printf("!!! Running task ID %d name: %s @%li ", a, ml_tasks[a].name,
               ms);
        get_modbus_data(modbus_request, sizeof(modbus_request));
        mqtt_pubMsg();
        ml_tasks[a].last_run = ms;
      }
    }
  }
}

int main() {
  mqtt_connet_to();

  start_timer(&timerid);
  (void)signal(SIGALRM, timer_callback);
  ser = open_port();
  printf("Port opened\n");

  // initial struct def values
  for (uint8_t a = 0; a < MAX_TASKS_COUNT; a++) {
    ml_tasks[a].state = 0;
  }

  printf("Initialize default config options\n");

  strcpy(ml_tasks[0].name, "task1");
  ml_tasks[0].state = 1;
  ml_tasks[0].period_ms = 500;
  ml_tasks[0].last_run = ts_millis();

  while (1) {
  }

  close(ser);
}
