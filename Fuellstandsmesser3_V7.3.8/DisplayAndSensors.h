#pragma once
#include <Arduino.h>
void requestLedBlink(uint8_t pulses);
void ledTask();
void i2cScan();
void oledInit();
void oledTask();
void bmeInit();
void readBME280();

