#pragma once
#include <Arduino.h>
String buildMqttJson();
void publishHomeAssistantDiscovery();
bool configureMQTTServer();
bool connectMQTT();
void mqttPublish();
void mqttTask();

