#pragma once

void rtcInit();
void rtcTask();
void rtcNotifyNtpSync();
bool rtcAvailable();
bool rtcHasValidTime();
bool ntpHasSynced();
const char* activeTimeSource();
