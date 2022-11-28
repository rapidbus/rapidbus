#ifndef MQTT_HEADER_FILE
#define MQTT_HEADER_FILE

#include "MQTTAsync.h"
#include <stdint.h> /* types definition */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MQTT_ADDRESS "tcp://broker.hivemq.com:1883"
#define MQTT_CLIENTID "hackcli"
#define MQTT_TOPIC "HackCli"
#define MQTT_PAYLOAD "{\"ident\": \"r1\",\"value\": %f, \"ts\": %li}"
#define MQTT_QOS 0
#define MQTT_TIMEOUT 10000L

void mqtt_connlost(void *context, char *cause);
void mqtt_onDisconnect(void *context, MQTTAsync_successData *response);
void mqtt_onSend(void *context, MQTTAsync_successData *response);
void mqtt_onConnectFailure(void *context, MQTTAsync_failureData *response);
void mqtt_onConnect(void *context, MQTTAsync_successData *response);
void mqtt_pubMsg(char *message, uint16_t message_len);

#endif
