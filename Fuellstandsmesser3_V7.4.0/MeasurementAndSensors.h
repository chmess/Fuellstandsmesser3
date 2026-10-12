#pragma once
#include <Arduino.h>
void initializeSensors();
void reinitializeSensors(bool force);
bool tofDevicePresent();
bool calculateHeightFromLiters(float liters, float &heightMm);
float calculateTankCapacityLiters();
bool calculateDistanceFromLiters(float liters, float &distanceMm, float &heightMm);
bool calibrateFromKnownLiters(float liters, float measuredDistance,
                              float &newEmpty, float &newFull, float &heightMm);
void measurementTask();
