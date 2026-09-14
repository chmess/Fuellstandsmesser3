#pragma once

#include <Arduino.h>

String htmlEscape(const String &input);
String jsonEscape(const String &input);
String jsonFloat(float value, uint8_t decimals);
float calculateDewPointC(float temperatureC, float humidityPercent);

