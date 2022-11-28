#ifndef MQTT_HEADER_FILE
#define MQTT_HEADER_FILE

#include "MQTTAsync.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MQTT_ADDRESS "tcp://broker.hivemq.com:1883"
#define MQTT_CLIENTID "RapidBus"
#define MQTT_TOPIC "RapidBus"
#define MQTT_PAYLOAD "Hello World!"
#define MQTT_QOS 0
#define MQTT_TIMEOUT 10000L

void mqtt_connlost(void *context, char *cause);
void mqtt_onDisconnect(void *context, MQTTAsync_successData *response);
void mqtt_onSend(void *context, MQTTAsync_successData *response);
void mqtt_onConnectFailure(void *context, MQTTAsync_failureData *response);
void mqtt_onConnect(void *context, MQTTAsync_successData *response);

#endif
