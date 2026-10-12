#ifdef FUELLSTANDSMESSER3_UNITY_BUILD

#include "Rtc.h"

namespace {

constexpr uint8_t DS3231_ADDRESS = 0x68;
constexpr uint8_t DS3231_REG_TIME = 0x00;
constexpr uint8_t DS3231_REG_CONTROL = 0x0E;
constexpr uint8_t DS3231_REG_STATUS = 0x0F;
constexpr uint8_t DS3231_REG_TEMPERATURE_MSB = 0x11;
constexpr uint32_t RTC_RECHECK_INTERVAL_MS = 60000UL;
constexpr uint32_t RTC_REPROBE_INTERVAL_MS = 300000UL;

bool rtcDetected = false;
bool rtcTimeValid = false;
bool rtcSeededSystem = false;
bool rtcOscillatorStopped = false;
volatile bool ntpSyncConfirmed = false;
volatile bool rtcWritePending = false;
uint8_t rtcReadFailureCount = 0;
uint32_t lastRtcCheckMs = 0;

uint8_t bcdToBinary(uint8_t value) {
  return (uint8_t)(((value >> 4) * 10U) + (value & 0x0FU));
}

uint8_t binaryToBcd(uint8_t value) {
  return (uint8_t)(((value / 10U) << 4) | (value % 10U));
}

bool rtcReadRegisters(uint8_t reg, uint8_t *data, size_t length) {
  Wire.beginTransmission(DS3231_ADDRESS);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) return false;

  const size_t received = Wire.requestFrom(DS3231_ADDRESS, length);
  if (received != length) {
    while (Wire.available()) Wire.read();
    return false;
  }

  for (size_t i = 0; i < length; ++i) data[i] = (uint8_t)Wire.read();
  return true;
}

bool rtcWriteRegisters(uint8_t reg, const uint8_t *data, size_t length) {
  Wire.beginTransmission(DS3231_ADDRESS);
  Wire.write(reg);
  for (size_t i = 0; i < length; ++i) Wire.write(data[i]);
  return Wire.endTransmission() == 0;
}

bool isLeapYear(int year) {
  return (year % 4 == 0) && ((year % 100 != 0) || (year % 400 == 0));
}

uint8_t daysInMonth(int year, int month) {
  static const uint8_t days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  if (month == 2 && isLeapYear(year)) return 29;
  return days[month - 1];
}

bool rtcLooksLikeDs3231() {
  uint8_t registers[5];
  if (!rtcReadRegisters(DS3231_REG_CONTROL, registers, sizeof(registers))) return false;

  // Statusbits 6,5,4 und 2 sind bei der DS3231 reserviert und immer 0.
  if ((registers[1] & 0x74U) != 0) return false;
  // Im Temperatur-LSB werden nur die beiden obersten Bits verwendet.
  if ((registers[4] & 0x3FU) != 0) return false;
  return true;
}

bool rtcReadCalendar(struct tm &calendar, bool &oscillatorStopped) {
  uint8_t raw[7];
  uint8_t status = 0;
  if (!rtcReadRegisters(DS3231_REG_TIME, raw, sizeof(raw)) ||
      !rtcReadRegisters(DS3231_REG_STATUS, &status, 1)) {
    return false;
  }

  const uint8_t second = bcdToBinary(raw[0] & 0x7FU);
  const uint8_t minute = bcdToBinary(raw[1] & 0x7FU);
  uint8_t hour = 0;
  if (raw[2] & 0x40U) {
    hour = bcdToBinary(raw[2] & 0x1FU);
    if (hour < 1 || hour > 12) return false;
    if (raw[2] & 0x20U) hour = (uint8_t)((hour % 12U) + 12U);
    else if (hour == 12U) hour = 0;
  } else {
    hour = bcdToBinary(raw[2] & 0x3FU);
  }

  const uint8_t day = bcdToBinary(raw[4] & 0x3FU);
  const uint8_t month = bcdToBinary(raw[5] & 0x1FU);
  const int year = 2000 + bcdToBinary(raw[6]) + ((raw[5] & 0x80U) ? 100 : 0);

  if (second > 59 || minute > 59 || hour > 23 || month < 1 || month > 12 ||
      year < 2020 || year > 2099 || day < 1 || day > daysInMonth(year, month)) {
    return false;
  }

  memset(&calendar, 0, sizeof(calendar));
  calendar.tm_sec = second;
  calendar.tm_min = minute;
  calendar.tm_hour = hour;
  calendar.tm_mday = day;
  calendar.tm_mon = month - 1;
  calendar.tm_year = year - 1900;
  oscillatorStopped = (status & 0x80U) != 0;
  return true;
}

bool calendarToEpochUtc(const struct tm &calendar, time_t &epochUtc) {
  const int year = calendar.tm_year + 1900;
  const int month = calendar.tm_mon + 1;
  int64_t days = 0;
  for (int y = 1970; y < year; ++y) days += isLeapYear(y) ? 366 : 365;
  for (int m = 1; m < month; ++m) days += daysInMonth(year, m);
  days += calendar.tm_mday - 1;

  const int64_t localEpoch = days * 86400LL + calendar.tm_hour * 3600LL +
                             calendar.tm_min * 60LL + calendar.tm_sec;
  const int64_t utcEpoch = localEpoch - (int64_t)cfg.timezoneOffset;
  if (utcEpoch < 1000000000LL || utcEpoch > 4102444799LL) return false;
  epochUtc = (time_t)utcEpoch;
  return true;
}

bool rtcSeedSystemClock() {
  struct tm calendar;
  bool oscillatorStopped = false;
  time_t epochUtc = 0;
  if (!rtcReadCalendar(calendar, oscillatorStopped)) return false;

  rtcOscillatorStopped = oscillatorStopped;
  if (oscillatorStopped || !calendarToEpochUtc(calendar, epochUtc)) return false;

  struct timeval tv = {epochUtc, 0};
  if (settimeofday(&tv, nullptr) != 0) return false;
  rtcSeededSystem = true;
  return true;
}

bool rtcWriteSystemClock() {
  const time_t epochUtc = time(nullptr);
  if (epochUtc < 1000000000UL) return false;

  const time_t localEpoch = epochUtc + (time_t)cfg.timezoneOffset;
  struct tm calendar;
  if (gmtime_r(&localEpoch, &calendar) == nullptr) return false;

  const int year = calendar.tm_year + 1900;
  if (year < 2000 || year > 2199) return false;

  int64_t days = 0;
  for (int y = 1970; y < year; ++y) days += isLeapYear(y) ? 366 : 365;
  for (int m = 1; m <= calendar.tm_mon; ++m) days += daysInMonth(year, m);
  days += calendar.tm_mday - 1;
  const uint8_t weekday = (uint8_t)(((days + 4) % 7) + 1); // Sonntag=1

  uint8_t raw[7];
  raw[0] = binaryToBcd((uint8_t)calendar.tm_sec);
  raw[1] = binaryToBcd((uint8_t)calendar.tm_min);
  raw[2] = binaryToBcd((uint8_t)calendar.tm_hour); // 24-Stunden-Modus
  raw[3] = binaryToBcd(weekday);
  raw[4] = binaryToBcd((uint8_t)calendar.tm_mday);
  raw[5] = binaryToBcd((uint8_t)(calendar.tm_mon + 1));
  if (year >= 2100) raw[5] |= 0x80U;
  raw[6] = binaryToBcd((uint8_t)(year % 100));
  if (!rtcWriteRegisters(DS3231_REG_TIME, raw, sizeof(raw))) return false;

  uint8_t status = 0;
  if (rtcReadRegisters(DS3231_REG_STATUS, &status, 1)) {
    status &= (uint8_t)~0x80U;
    rtcWriteRegisters(DS3231_REG_STATUS, &status, 1);
  }

  rtcOscillatorStopped = false;
  rtcTimeValid = true;
  return true;
}

void rtcProbeAndInitialize(bool seedSystem) {
  rtcDetected = rtcLooksLikeDs3231();
  rtcTimeValid = false;
  rtcOscillatorStopped = false;
  if (!rtcDetected) return;

  struct tm calendar;
  bool oscillatorStopped = false;
  if (rtcReadCalendar(calendar, oscillatorStopped)) {
    rtcOscillatorStopped = oscillatorStopped;
    time_t unused = 0;
    rtcTimeValid = !oscillatorStopped && calendarToEpochUtc(calendar, unused);
  }

  if (seedSystem && rtcTimeValid) rtcTimeValid = rtcSeedSystemClock();
}

} // namespace

void rtcInit() {
  rtcProbeAndInitialize(true);
  lastRtcCheckMs = millis();

  if (!rtcDetected) {
    Serial.println(F("[RTC] Keine DS3231 erkannt - NTP-Betrieb bleibt aktiv"));
    return;
  }

  Serial.print(F("[RTC] DS3231 erkannt, Zeit "));
  if (rtcTimeValid) Serial.println(F("gueltig - Systemzeit uebernommen"));
  else if (rtcOscillatorStopped) Serial.println(F("ungueltig (Oszillator-Stopp) - warte auf NTP"));
  else Serial.println(F("ungueltig - warte auf NTP"));
}

void rtcNotifyNtpSync() {
  ntpSyncConfirmed = true;
  rtcWritePending = true;
}

void rtcTask() {
  const uint32_t nowMs = millis();

  if (rtcWritePending) {
    rtcWritePending = false;
    if (rtcDetected && rtcWriteSystemClock()) {
      rtcReadFailureCount = 0;
      Serial.println(F("[RTC] DS3231 mit NTP-Zeit synchronisiert"));
    }
  }

  const uint32_t interval = rtcDetected ? RTC_RECHECK_INTERVAL_MS : RTC_REPROBE_INTERVAL_MS;
  if ((uint32_t)(nowMs - lastRtcCheckMs) < interval) return;
  lastRtcCheckMs = nowMs;

  if (!rtcDetected) {
    rtcProbeAndInitialize(time(nullptr) < 1000000000UL);
    if (rtcDetected) Serial.println(F("[RTC] DS3231 nachtraeglich erkannt"));
    return;
  }

  struct tm calendar;
  bool oscillatorStopped = false;
  if (!rtcReadCalendar(calendar, oscillatorStopped)) {
    if (++rtcReadFailureCount >= 3) {
      rtcDetected = false;
      rtcTimeValid = false;
      Serial.println(F("[RTC] DS3231 nicht mehr erreichbar - Betrieb ohne RTC"));
    }
    return;
  }

  rtcReadFailureCount = 0;
  rtcOscillatorStopped = oscillatorStopped;
  time_t unused = 0;
  rtcTimeValid = !oscillatorStopped && calendarToEpochUtc(calendar, unused);

  if (ntpSyncConfirmed && !rtcTimeValid) rtcWriteSystemClock();
}

bool rtcAvailable() {
  return rtcDetected;
}

bool rtcHasValidTime() {
  return rtcDetected && rtcTimeValid;
}

bool ntpHasSynced() {
  return ntpSyncConfirmed;
}

const char* activeTimeSource() {
  if (ntpSyncConfirmed) return "NTP";
  if (rtcSeededSystem && rtcTimeValid) return "DS3231";
  if (time(nullptr) >= 1000000000UL) return "System";
  return "Keine";
}

#endif // FUELLSTANDSMESSER3_UNITY_BUILD
