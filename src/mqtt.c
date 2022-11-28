#include "mqtt.h"
#include "MQTTAsync.h"
#include "timers.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern volatile MQTTAsync_token deliveredtoken;
extern int finished;
extern MQTTAsync client;

void mqtt_connlost(void *context, char *cause) {
  MQTTAsync_connectOptions conn_opts = MQTTAsync_connectOptions_initializer;
  int rc;
  printf("\nConnection lost\n");
  printf("     cause: %s\n", cause);
  printf("Reconnecting\n");
  conn_opts.keepAliveInterval = 20;
  conn_opts.cleansession = 1;
  if ((rc = MQTTAsync_connect(client, &conn_opts)) != MQTTASYNC_SUCCESS) {
    printf("Failed to start connect, return code %d\n", rc);
    finished = 1;
  }
}

void mqtt_onDisconnect(void *context, MQTTAsync_successData *response) {
  printf("Successful disconnection\n");
  finished = 1;
}

void mqtt_onSend(void *context, MQTTAsync_successData *response) {
  // MQTTAsync_disconnectOptions opts = MQTTAsync_disconnectOptions_initializer;
  // int rc;
  printf("Message with token value %d delivery confirmed\n", response->token);
  //    opts.onSuccess = mqtt_onDisconnect;
  //    opts.context = client;
  //    if ((rc = MQTTAsync_disconnect(client, &opts)) != MQTTASYNC_SUCCESS) {
  //        printf("Failed to start sendMessage, return code %d\n", rc);
  //        exit(EXIT_FAILURE);
  //    }
}

void mqtt_onConnectFailure(void *context, MQTTAsync_failureData *response) {
  printf("Connect failed, rc %d\n", response ? response->code : 0);
  finished = 1;
}

void mqtt_onConnect(void *context, MQTTAsync_successData *response) {
  int rc;
  MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
  MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
  printf("Successful connection to MQTT broker!\n");
  opts.onSuccess = mqtt_onSend;
  opts.context = client;
  pubmsg.payload = MQTT_PAYLOAD;
  pubmsg.payloadlen = strlen(MQTT_PAYLOAD);
  pubmsg.qos = MQTT_QOS;
  pubmsg.retained = 0;
  deliveredtoken = 0;
  rc = MQTTAsync_sendMessage(client, MQTT_TOPIC, &pubmsg, &opts);
  if (rc == MQTTASYNC_SUCCESS) {
    printf("mqtt_pubMsg: Message published OK!\n");
  } else if (rc == MQTTASYNC_DISCONNECTED) {
    printf("mqtt_pubMsg: Failed to start sendMessage - The client is "
           "disconnected. (retcode: %d)\n",
           rc);
    exit(EXIT_FAILURE);
  } else {
  }
}

void mqtt_pubMsg(char *message, uint16_t message_len) {
  int rc;
  MQTTAsync_responseOptions opts = MQTTAsync_responseOptions_initializer;
  MQTTAsync_message pubmsg = MQTTAsync_message_initializer;
  printf("Publishing message to MQTT broker!\n");
  opts.onSuccess = mqtt_onSend;
  opts.context = client;
  pubmsg.payload = message;
  pubmsg.payloadlen = message_len;
  pubmsg.qos = MQTT_QOS;
  pubmsg.retained = 0;
  deliveredtoken = 0;
  rc = MQTTAsync_sendMessage(client, MQTT_TOPIC, &pubmsg, &opts);
  if (rc == MQTTASYNC_SUCCESS) {
    printf("mqtt_pubMsg: Message published OK!");
  } else if (rc == MQTTASYNC_DISCONNECTED) {
    printf("mqtt_pubMsg: Failed to start sendMessage - The client is "
           "disconnected. (retcode: %d)\n",
           rc);
    exit(EXIT_FAILURE);
  } else {
  }
}
