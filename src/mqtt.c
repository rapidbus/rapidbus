#include "mqtt.h"
#include "MQTTAsync.h"
#include "config.h"
#include "timers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern volatile MQTTAsync_token deliveredtoken;
extern MQTTAsync client;
extern uint8_t mqtt_connected;
extern mqtt_conf_t mqtt_config;
extern uint8_t verbose;

void mqtt_connlost(__attribute__((unused)) void *context, char *cause) {
  MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
  int rc;
  printf("\nConnection lost\n");
  printf("     cause: %s\n", cause);
  printf("Reconnecting\n");
  conn_opts.struct_version = 6;
  conn_opts.keepAliveInterval = 20;
  conn_opts.cleansession = 1;
  rc = MQTTAsync_connect(client, &conn_opts);
  if (rc == MQTTASYNC_SUCCESS) {
    printf("MQTTASYNC_SUCCESS\n");
  } else if (rc == MQTTASYNC_BAD_STRUCTURE) {
    printf("MQTTASYNC_BAD_STRUCTURE (%d) returned, debug:\n", rc);
    printf("->struct_id      : %s\n", conn_opts.struct_id);
    printf("->struct_version : %d\n", conn_opts.struct_version);
    mqtt_connected = 0;
  } else {
    printf("Failed to start MQTTAsync_connect, return code: %d\n", rc);
    mqtt_connected = 0;
  }
}

void mqtt_onDisconnect(__attribute__((unused)) void *context, __attribute__((unused)) MQTTAsync_successData *response) {
  printf("Successful disconnection\n");
  mqtt_connected = 0;
}

void mqtt_onSend(__attribute__((unused)) void *context, MQTTAsync_successData *response) {
  if (verbose) {
    printf("Message with token value %d delivery confirmed\n", response->token);
  }
}

void mqtt_onConnectFailure(__attribute__((unused)) void *context, MQTTAsync_failureData *response) {
  printf("Connect failed, rc %d\n", response ? response->code : 0);
  mqtt_connected = 0;
}

void mqtt_onConnect(__attribute__((unused)) void *context, __attribute__((unused)) MQTTAsync_successData *response) {
  printf("Successful connection to MQTT broker!\n");
  mqtt_connected = 1;
}

void mqtt_pubMsg(char *message, uint16_t message_len) {
  int rc;
  MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
  MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
  if (verbose) {
    printf("Publishing message to MQTT broker!\n");
  }
  opts.onSuccess = mqtt_onSend;
  opts.context = client;
  pubmsg.payload = message;
  pubmsg.payloadlen = message_len;
  pubmsg.qos = MQTT_QOS;
  pubmsg.retained = 0;
  deliveredtoken = 0;
  rc = MQTTAsync_sendMessage(client, mqtt_config.topic, &pubmsg, &opts);
  if (rc == MQTTASYNC_SUCCESS) {
    if (verbose) {
      printf("mqtt_pubMsg: Message accepted for publication!\n");
    }
  } else if (rc == MQTTASYNC_DISCONNECTED) {
    printf("mqtt_pubMsg: Failed to start sendMessage - The client is "
           "disconnected. (retcode: %d)\n",
           rc);
    mqtt_connected = 0;
  } else {
    printf("mqtt_pubMsg: Ignoring unknown return code: %d)\n", rc);
  }
}
