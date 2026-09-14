#ifdef FUELLSTANDSMESSER3_UNITY_BUILD

#include "MeasurementAndSensors.h"

// ============================================================================
// BME280
// ============================================================================

void bmeInit() {

  bmeOK = false;

  if (bme.begin(
        cfg.bmeAddress,
        &Wire)) {

    bmeOK = true;

    Serial.println(
      "BME280 OK"
    );

  } else {

    Serial.println(
      "BME280 FEHLER"
    );
  }
}

void readBME280() {

  if (!bmeOK) {
    return;
  }

  float t =
    bme.readTemperature();

  float h =
    bme.readHumidity();

  float p =
    bme.readPressure() / 100.0f;

  if (bmeValuesPlausible(t, h, p)) {
    temperature = t;
    humidity = h;
    pressure = p;
  } else if (debugEnabled(DEBUG_LEVEL_WARN)) {
    Serial.print("[BME] Ungueltiger Messwert verworfen: T=");
    Serial.print(t, 1);
    Serial.print(" C H=");
    Serial.print(h, 1);
    Serial.print(" % P=");
    Serial.print(p, 1);
    Serial.println(" hPa");
  }

  dewPoint = calculateDewPointC(temperature, humidity);

  if (debugEnabled(DEBUG_LEVEL_DETAIL)) {

    Serial.print("[BME] T=");
    Serial.print(temperature, 1);

    Serial.print(" C H=");
    Serial.print(humidity, 1);

    Serial.print(" % P=");
    Serial.print(pressure, 1);

    Serial.println(" hPa");
  }
}

// ============================================================================
// TOF CHIP IDENTIFICATION
// ============================================================================

bool readI2CRegister8(uint8_t address, uint8_t reg, uint8_t &value) {

  Wire.beginTransmission(address);
  Wire.write(reg);

  if (Wire.endTransmission(false) != 0) {
    return false;
  }

  if (Wire.requestFrom((int)address, 1) != 1) {
    return false;
  }

  value = Wire.read();
  return true;
}

bool readI2CRegister16(uint8_t address, uint16_t reg, uint8_t &value) {

  Wire.beginTransmission(address);
  Wire.write((uint8_t)(reg >> 8));
  Wire.write((uint8_t)(reg & 0xFF));

  if (Wire.endTransmission(false) != 0) {
    return false;
  }

  if (Wire.requestFrom((int)address, 1) != 1) {
    return false;
  }

  value = Wire.read();
  return true;
}

bool writeI2CRegister8(uint8_t address, uint16_t reg, uint8_t value) {

  Wire.beginTransmission(address);
  Wire.write((uint8_t)(reg >> 8));
  Wire.write((uint8_t)(reg & 0xFF));
  Wire.write(value);
  return Wire.endTransmission() == 0;
}

bool detectVL53L5CX() {

  uint8_t deviceId = 0;
  uint8_t revisionId = 0;

  // VL53L5CX exposes device/revision ID on page 0.
  // Expected IDs are 0xF0 / 0x02.
  if (!writeI2CRegister8(cfg.vlAddress, 0x7FFF, 0x00)) {
    return false;
  }

  bool ok = readI2CRegister8(cfg.vlAddress, 0x00, deviceId) &&
            readI2CRegister8(cfg.vlAddress, 0x01, revisionId);

  // Return to the normal page even when detection fails.
  writeI2CRegister8(cfg.vlAddress, 0x7FFF, 0x02);

  return ok && deviceId == 0xF0 && revisionId == 0x02;
}

uint8_t detectToFChip() {

  uint8_t model = 0;

  // VL53L0X: IDENTIFICATION_MODEL_ID (0xC0) = 0xEE
  if (readI2CRegister8(cfg.vlAddress, 0xC0, model) &&
      model == 0xEE) {
    return SENSOR_VL53L0X;
  }

  // VL53L1X: MODEL_ID (0x010F) = 0xEA
  if (readI2CRegister16(cfg.vlAddress, 0x010F, model) &&
      model == 0xEA) {
    return SENSOR_VL53L1X;
  }

  return SENSOR_AUTO;
}

// ============================================================================
// VL53L5CX
// ============================================================================

bool initVL53L5CX() {

  if (debugEnabled(DEBUG_LEVEL_SENSOR)) {
    Serial.println("Initialisiere VL53L5CX...");
  }

  if (!vl53l5cxInitialized) {
    if (!vl53l5cx.begin(cfg.vlAddress, Wire)) {
      if (debugEnabled(DEBUG_LEVEL_ERROR)) {
        Serial.println("VL53L5CX FEHLER bei begin()");
      }
      return false;
    }
    vl53l5cxInitialized = true;
  } else {
    vl53l5cx.stopRanging();
    if (vl53l5cx.Dev == nullptr || vl53l5cx.VL53L5CX_i2c == nullptr) {
      vl53l5cxInitialized = false;
      return false;
    }
    if (vl53l5cx.Dev->platform.VL53L5CX_i2c == nullptr) {
      vl53l5cxInitialized = false;
      return false;
    }
    uint8_t st = vl53l5cx_init(vl53l5cx.Dev);
    if (st != 0) {
      if (debugEnabled(DEBUG_LEVEL_ERROR)) {
        Serial.print("VL53L5CX Reinit FEHLER, Status=");
        Serial.println(st);
      }
      return false;
    }
  }

  if (!vl53l5cx.setResolution(VL53L5CX_RESOLUTION_8X8)) {
    if (debugEnabled(DEBUG_LEVEL_ERROR)) {
      Serial.println("[VL53L5CX] 8x8 Auflösung konnte nicht gesetzt werden");
    }
    return false;
  }

  // 8x8 benötigt deutlich mehr I2C-Daten; 400 kHz ist eine robuste Wahl.
  Wire.setClock(400000);

  if (!vl53l5cx.startRanging()) {
    if (debugEnabled(DEBUG_LEVEL_ERROR)) {
      Serial.println("VL53L5CX Ranging Start FEHLER");
    }
    return false;
  }

  vl53l5cxValidZones = 0;
  if (debugEnabled(DEBUG_LEVEL_SENSOR)) {
    Serial.println("VL53L5CX OK - 8x8 / 64 Zonen / 400 kHz");
  }
  return true;
}

// ============================================================================
// VL53L0X
// ============================================================================

bool initVL53L0X() {

  if (debugEnabled(DEBUG_LEVEL_SENSOR)) {
    Serial.println(
      "Initialisiere VL53L0X..."
    );
  }

  if (vl53l0x.begin(
        cfg.vlAddress,
        false,
        &Wire,
        Adafruit_VL53L0X::VL53L0X_SENSE_DEFAULT)) {

    if (debugEnabled(DEBUG_LEVEL_SENSOR)) {
      Serial.println(
        "VL53L0X OK"
      );
    }

    return true;
  }

  if (debugEnabled(DEBUG_LEVEL_ERROR)) {
    Serial.println(
      "VL53L0X FEHLER"
    );
  }

  return false;
}

// ============================================================================
// VL53L1X
// ============================================================================

bool initVL53L1X() {

  if (debugEnabled(DEBUG_LEVEL_SENSOR)) {
    Serial.println(
      "Initialisiere VL53L1X..."
    );
  }

  if (!vl53l1x.begin(
        cfg.vlAddress,
        &Wire,
        false)) {

    if (debugEnabled(DEBUG_LEVEL_ERROR)) {
      Serial.println(
        "VL53L1X FEHLER"
      );
    }

    return false;
  }

  if (debugEnabled(DEBUG_LEVEL_SENSOR)) {
    Serial.println(
      "VL53L1X OK"
    );
  }

  if (!vl53l1x.setTimingBudget(50)) {

    if (debugEnabled(DEBUG_LEVEL_WARN)) {
      Serial.println(
        "[WARN] VL53L1X Timing Budget konnte nicht gesetzt werden"
      );
    }

  } else {

    if (debugEnabled(DEBUG_LEVEL_SENSOR)) {
      Serial.print(
        "VL53L1X Timing Budget: "
      );

      Serial.print(
        vl53l1x.getTimingBudget()
      );

      Serial.println(
        " ms"
      );
    }
  }

  if (!vl53l1x.startRanging()) {

    if (debugEnabled(DEBUG_LEVEL_ERROR)) {
      Serial.println(
        "VL53L1X Ranging Start FEHLER"
      );
    }

    return false;
  }

  if (debugEnabled(DEBUG_LEVEL_SENSOR)) {
    Serial.println(
      "VL53L1X Ranging gestartet"
    );
  }

  return true;
}


static bool i2cAddressResponds(uint8_t address) {
  Wire.beginTransmission(address);
  return Wire.endTransmission(true) == 0;
}

static bool ensureToFAddress() {
  if (debugEnabled(DEBUG_LEVEL_SENSOR)) {
    Serial.print("[TOF] Konfigurierte I2C-Adresse: 0x");
    if (cfg.vlAddress < 0x10) Serial.print("0");
    Serial.println(cfg.vlAddress, HEX);
  }

  if (i2cAddressResponds(cfg.vlAddress)) {
    return true;
  }

  // Alle VL53L0X starten ab Werk auf 0x29. Falls eine alte/fehlerhafte
  // Konfiguration eine andere Adresse gespeichert hat, automatisch korrigieren.
  if (cfg.vlAddress != 0x29 && i2cAddressResponds(0x29)) {
    Serial.print("[TOF] Keine Antwort auf 0x");
    if (cfg.vlAddress < 0x10) Serial.print("0");
    Serial.print(cfg.vlAddress, HEX);
    Serial.println(" - Sensor auf Standardadresse 0x29 gefunden");
    cfg.vlAddress = 0x29;
    return true;
  }

  Serial.print("[TOF] Keine Antwort auf I2C-Adresse 0x");
  if (cfg.vlAddress < 0x10) Serial.print("0");
  Serial.println(cfg.vlAddress, HEX);
  return false;
}

// ============================================================================
// TOF INIT
// ============================================================================

void tofInit() {

  vlOK = false;
  cfg.vlSensorType = SENSOR_AUTO;

  if (debugEnabled(DEBUG_LEVEL_SENSOR)) {
    Serial.println("Erkenne ToF-Sensor automatisch...");
  }

  if (!ensureToFAddress()) {
    systemState = STATE_SENSOR_ERROR;
    return;
  }

  if (debugEnabled(DEBUG_LEVEL_SENSOR)) {
    Serial.print("[TOF] Verwende I2C-Adresse: 0x");
    if (cfg.vlAddress < 0x10) Serial.print("0");
    Serial.println(cfg.vlAddress, HEX);
  }

  // Autoerkennung: L0X/L1X immer vollstaendig pruefen, bevor irgendein L5CX-spezifischer
  // Page-Zugriff ausgefuehrt wird. Das ist besonders wichtig fuer den
  // VL53L0X, dessen Registermodell nur 8-Bit-Adressen verwendet.
  uint8_t model8 = 0;
  bool l0IdRead = readI2CRegister8(cfg.vlAddress, 0xC0, model8);

  if (debugEnabled(DEBUG_LEVEL_SENSOR)) {
    Serial.print("[TOF] L0X ID 0xC0: ");
    if (l0IdRead) {
      Serial.print("0x");
      Serial.println(model8, HEX);
    } else {
      Serial.println("nicht lesbar");
    }
  }

  if (l0IdRead && model8 == 0xEE) {
    if (debugEnabled(DEBUG_LEVEL_SENSOR)) {
      Serial.println("[TOF] Chip-ID: VL53L0X");
    }
    if (initVL53L0X()) {
      cfg.vlSensorType = SENSOR_VL53L0X;
      vlOK = true;
      if (debugEnabled(DEBUG_LEVEL_SENSOR)) {
        Serial.println("[TOF] Automatisch erkannt: VL53L0X");
      }
      return;
    }
  }

  uint8_t model16 = 0;
  bool l1IdRead = readI2CRegister16(cfg.vlAddress, 0x010F, model16);

  if (debugEnabled(DEBUG_LEVEL_SENSOR)) {
    Serial.print("[TOF] L1X ID 0x010F: ");
    if (l1IdRead) {
      Serial.print("0x");
      Serial.println(model16, HEX);
    } else {
      Serial.println("nicht lesbar");
    }
  }

  if (l1IdRead && model16 == 0xEA) {
    if (debugEnabled(DEBUG_LEVEL_SENSOR)) {
      Serial.println("[TOF] Chip-ID: VL53L1X");
    }
    if (initVL53L1X()) {
      cfg.vlSensorType = SENSOR_VL53L1X;
      vlOK = true;
      if (debugEnabled(DEBUG_LEVEL_SENSOR)) {
        Serial.println("[TOF] Automatisch erkannt: VL53L1X");
      }
      return;
    }
  }

  // IDs nicht eindeutig: zuerst die beiden klassischen Einzelzonen-Sensoren
  // per Treiber testen. L0X absichtlich zuerst, da er in der aktuellen
  // Hardware steckt und dieser Test keine L5CX-Page-Register beschreibt.
  if (debugEnabled(DEBUG_LEVEL_SENSOR)) {
    Serial.println("[TOF] Chip-ID nicht eindeutig - teste L0X/L1X Treiber...");
  }

  if (initVL53L0X()) {
    cfg.vlSensorType = SENSOR_VL53L0X;
    vlOK = true;
    if (debugEnabled(DEBUG_LEVEL_SENSOR)) {
      Serial.println("[TOF] Erkannt ueber VL53L0X-Treiber");
    }
    return;
  }

  if (initVL53L1X()) {
    cfg.vlSensorType = SENSOR_VL53L1X;
    vlOK = true;
    if (debugEnabled(DEBUG_LEVEL_SENSOR)) {
      Serial.println("[TOF] Erkannt ueber VL53L1X-Treiber");
    }
    return;
  }

  // Erst als letzter Schritt den VL53L5CX-spezifischen Page-Zugriff verwenden.
  if (detectVL53L5CX()) {
    if (debugEnabled(DEBUG_LEVEL_SENSOR)) {
      Serial.println("[TOF] Chip-ID: VL53L5CX (0xF0/0x02)");
    }
    if (initVL53L5CX()) {
      cfg.vlSensorType = SENSOR_VL53L5CX;
      vlOK = true;
      if (debugEnabled(DEBUG_LEVEL_SENSOR)) {
        Serial.println("[TOF] Automatisch erkannt: VL53L5CX");
      }
      return;
    }
  }

  if (debugEnabled(DEBUG_LEVEL_ERROR)) {
    Serial.println("[TOF] Kein VL53L0X/VL53L1X/VL53L5CX verfuegbar");
    Serial.println("ToF-Sensor nicht verfuegbar");
  }
  systemState = STATE_SENSOR_ERROR;
}

// ============================================================================
// TOF READ
// ============================================================================

bool readToF(uint16_t &distance) {

  sensorDataPending = false;

  if (!vlOK) {
    return false;
  }

  if (cfg.vlSensorType == SENSOR_VL53L5CX) {

    if (!vl53l5cx.isDataReady()) {
      sensorDataPending = true;
      return false;
    }

    if (!vl53l5cx.getRangingData(&vl53l5cxData)) {
      return false;
    }

    // Robuster Distanzwert aus den 64 Zonen: gültige Zielstatus 5/9,
    // anschließend Median der gültigen Zonen. Das reduziert Ausreißer durch
    // Schaum, Reflexionen und einzelne ungültige Pixel.
    uint16_t distances[64];
    uint8_t count = 0;

    for (uint8_t i = 0; i < 64; i++) {
      int16_t d = vl53l5cxData.distance_mm[i];
      uint8_t st = vl53l5cxData.target_status[i];

      if (d > 0 && d <= 4000 && (st == 5 || st == 9)) {
        distances[count++] = (uint16_t)d;
      }
    }

    vl53l5cxValidZones = count;
    sensorValidZones = count;
    sensorStatus = (count > 0) ? 0 : 255;

    if (count == 0) {
      return false;
    }

    for (uint8_t i = 1; i < count; i++) {
      uint16_t v = distances[i];
      int8_t j = (int8_t)i - 1;
      while (j >= 0 && distances[j] > v) {
        distances[j + 1] = distances[j];
        j--;
      }
      distances[j + 1] = v;
    }

    distance = distances[count / 2];
    return true;
  }

  if (cfg.vlSensorType ==
      SENSOR_VL53L0X) {

    VL53L0X_RangingMeasurementData_t measure;

    VL53L0X_Error result =
      vl53l0x.rangingTest(
        &measure,
        false
      );

    if (result != 0) {
      return false;
    }

    sensorStatus =
      measure.RangeStatus;

    if (measure.RangeStatus != 0) {
      return false;
    }

    distance =
      measure.RangeMilliMeter;

    return true;
  }

  if (!vl53l1x.dataReady()) {
    sensorDataPending = true;
    return false;
  }

  int16_t d =
    vl53l1x.distance();

  sensorStatus =
    (uint8_t)vl53l1x.vl_status;

  vl53l1x.clearInterrupt();

  if (d <= 0) {
    return false;
  }

  distance =
    (uint16_t)d;

  return true;
}

// ============================================================================
// FILTER RESET
// ============================================================================

void resetFilters() {

  memset(
    averageBuffer,
    0,
    sizeof(averageBuffer)
  );

  memset(
    medianBuffer,
    0,
    sizeof(medianBuffer)
  );

  averageIndex = 0;
  averageCount = 0;

  medianIndex = 0;
  medianCount = 0;

  rawDistanceFiltered = NAN;
  medianDistance = NAN;
  filteredDistance = NAN;
}

// ============================================================================
// MEDIAN
// ============================================================================

float applyMedian(float value) {

  uint8_t count =
    cfg.medianSamples;

  if (count < 3) {
    return value;
  }

  if (count > MAX_MEDIAN_SAMPLES) {
    count = MAX_MEDIAN_SAMPLES;
  }

  medianBuffer[
    medianIndex
  ] = value;

  medianIndex++;

  if (medianIndex >= count) {
    medianIndex = 0;
  }

  if (medianCount < count) {
    medianCount++;
  }

  float temp[
    MAX_MEDIAN_SAMPLES
  ];

  for (uint8_t i = 0;
       i < medianCount;
       i++) {

    temp[i] =
      medianBuffer[i];
  }

  for (uint8_t i = 0;
       i < medianCount;
       i++) {

    for (uint8_t j = i + 1;
         j < medianCount;
         j++) {

      if (temp[j] < temp[i]) {

        float t =
          temp[i];

        temp[i] =
          temp[j];

        temp[j] =
          t;
      }
    }
  }

  return temp[
    medianCount / 2
  ];
}

// ============================================================================
// MOVING AVERAGE
// ============================================================================

float applyMovingAverage(float value) {

  uint8_t count =
    cfg.sampleCount;

  if (count < 1) {
    return value;
  }

  if (count > MAX_SAMPLES) {
    count = MAX_SAMPLES;
  }

  averageBuffer[
    averageIndex
  ] = value;

  averageIndex++;

  if (averageIndex >= count) {
    averageIndex = 0;
  }

  if (averageCount < count) {
    averageCount++;
  }

  double sum = 0.0;

  for (uint8_t i = 0;
       i < averageCount;
       i++) {

    sum += averageBuffer[i];
  }

  return (float)(
    sum / averageCount
  );
}

// ============================================================================
// TANK CALCULATION
// ============================================================================

void calculateTank(float distance) {

  if (!isfinite(distance)) {
    tankPercent = NAN;
    tankHeightMm = NAN;
    tankLiters = NAN;
    return;
  }

  const float denominator = cfg.emptyDistance - cfg.fullDistance;

  if (denominator <= 0.0f || cfg.tankHeight <= 0.0f) {
    tankPercent = NAN;
    tankHeightMm = NAN;
    tankLiters = NAN;
    return;
  }

  // Ein gemeinsamer Füllstandsfaktor ist die alleinige Grundlage
  // für Höhe, Liter und Prozent. Dadurch können % und Liter nicht
  // mehr aus zwei unterschiedlichen Berechnungen stammen.
  float fraction =
    (cfg.emptyDistance - distance) / denominator;

  fraction = constrain(fraction, 0.0f, 1.0f);

  tankHeightMm = fraction * cfg.tankHeight;

  double volumeM3 = 0.0;

  if (cfg.geometry == GEOMETRY_CYLINDER) {
    const double radiusM = ((double)cfg.diameter / 2.0) / 1000.0;
    const double heightM = (double)tankHeightMm / 1000.0;

    if (radiusM <= 0.0) {
      tankPercent = NAN;
      tankLiters = NAN;
      return;
    }

    volumeM3 = PI * radiusM * radiusM * heightM;

  } else {
    const double lengthM = (double)cfg.tankLength / 1000.0;
    const double widthM  = (double)cfg.tankWidth  / 1000.0;

    if (lengthM <= 0.0 || widthM <= 0.0) {
      tankPercent = NAN;
      tankLiters = NAN;
      return;
    }

    // Beim Rechtecktank wird tankHeightMm bereits durch fraction bestimmt.
    volumeM3 = lengthM * widthM * ((double)tankHeightMm / 1000.0);
  }

  tankLiters = (float)(volumeM3 * 1000.0);

  const float capacityLiters = calculateTankCapacityLiters();

  if (isfinite(tankLiters) &&
      isfinite(capacityLiters) &&
      capacityLiters > 0.0f) {

    // Prozent immer aus exakt dem berechneten Literwert.
    tankPercent =
      constrain((tankLiters / capacityLiters) * 100.0f, 0.0f, 100.0f);

  } else {
    tankPercent = NAN;
  }
}


// ============================================================================
// NACHFUELL-EVENTS
// ============================================================================

void clearRefillEvents() {
  for (uint8_t i = 0; i < REFILL_EVENT_COUNT; i++) {
    refillEvents[i].timestamp = 0;
    refillEvents[i].liters = 0.0f;
  }
  refillEventCount = 0;
  lastConfirmedRefillLiters = 0.0f;
  lastRefillTimestamp = 0;
}

#ifndef REFILL_MIN_LITERS
#define REFILL_MIN_LITERS 150.0f
#endif

void addRefillEvent(float liters) {
  if (!isfinite(liters) || liters < REFILL_MIN_LITERS) return;

  time_t now;
  time(&now);
  uint32_t timestamp = (now > 100000) ? (uint32_t)now : millis();

  uint8_t limit = (refillEventCount < REFILL_EVENT_COUNT)
                    ? refillEventCount
                    : (REFILL_EVENT_COUNT - 1);

  for (int i = limit; i > 0; i--) {
    refillEvents[i] = refillEvents[i - 1];
  }

  refillEvents[0].timestamp = timestamp;
  refillEvents[0].liters = liters;

  if (refillEventCount < REFILL_EVENT_COUNT) refillEventCount++;

  lastConfirmedRefillLiters = liters;
  lastRefillTimestamp = timestamp;

  Serial.print(F("[REFILL] Ereignis gespeichert: +"));
  Serial.print(liters, 1);
  Serial.println(F(" L"));

  saveRefillEvents();
}

void saveRefillEvents() {
  preferences.begin("fuellstand", false);
  preferences.putBytes("refills", refillEvents, sizeof(refillEvents));
  preferences.putUChar("refill_cnt", refillEventCount);
  preferences.end();
}

void loadRefillEvents() {
  clearRefillEvents();

  preferences.begin("fuellstand", true);

  size_t len = preferences.getBytesLength("refills");
  if (len == sizeof(refillEvents)) {
    preferences.getBytes("refills", refillEvents, sizeof(refillEvents));
    uint8_t count = preferences.getUChar("refill_cnt", 0);
    refillEventCount = min(count, (uint8_t)REFILL_EVENT_COUNT);

    if (refillEventCount > 0) {
      lastConfirmedRefillLiters = refillEvents[0].liters;
      lastRefillTimestamp = refillEvents[0].timestamp;
    }
  }

  preferences.end();
}

String formatRefillDate(uint32_t timestamp) {
  if (timestamp == 0) return "-";

  time_t t = (time_t)timestamp;
  int64_t localSeconds = (int64_t)t + (int64_t)cfg.timezoneOffset;
  time_t localTime = (time_t)localSeconds;
  struct tm tmLocal;
  gmtime_r(&localTime, &tmLocal);

  char buf[24];
  snprintf(buf, sizeof(buf), "%02d.%02d.%04d %02d:%02d",
           tmLocal.tm_mday, tmLocal.tm_mon + 1, tmLocal.tm_year + 1900,
           tmLocal.tm_hour, tmLocal.tm_min);
  return String(buf);
}

// ============================================================================
// VERBRAUCHSBERECHNUNG
// ============================================================================

float calculateTankCapacityLiters() {
  double volumeM3 = 0.0;

  if (cfg.geometry == GEOMETRY_CYLINDER) {
    double radiusM = (double)cfg.diameter / 2000.0;
    double heightM = (double)cfg.tankHeight / 1000.0;
    volumeM3 = PI * radiusM * radiusM * heightM;
  } else {
    volumeM3 =
      ((double)cfg.tankLength / 1000.0) *
      ((double)cfg.tankWidth / 1000.0) *
      ((double)cfg.tankHeight / 1000.0);
  }

  return (float)(volumeM3 * 1000.0);
}

// Liter je Millimeter Füllhöhe. Bei den aktuell unterstützten vertikalen
// Tankgeometrien ist der Querschnitt über die Höhe konstant.
float calculateLitersPerMm() {
  if (cfg.tankHeight <= 0) return NAN;
  const float capacity = calculateTankCapacityLiters();
  if (!isfinite(capacity) || capacity <= 0.0f) return NAN;
  return capacity / (float)cfg.tankHeight;
}

// ============================================================================
// MEASUREMENT
// ============================================================================

void measurementTask() {

  uint32_t now = millis();

  if (now - lastMeasurement <
      cfg.measurementInterval) {
    return;
  }

  lastMeasurement = now;

  readBME280();

  uint16_t distance = 0;

  if (!readToF(distance)) {

    if (sensorFailureSince == 0) {
      sensorFailureSince = now;
    }

    // ToF-Bereitschaft:
    // VL53L1X/VL53L5CX melden dataReady()==false auch im normalen Betrieb
    // kurzzeitig. Solange dieser Zustand kuerzer als SENSOR_FAIL_TIMEOUT
    // dauert, ist das kein Fehler und erhoeht auch nicht den Fehlerzaehler.
    if (sensorDataPending &&
        now - sensorFailureSince < SENSOR_FAIL_TIMEOUT) {
      return;
    }

    sensorErrorCount++;

    if (now - sensorFailureSince >= SENSOR_FAIL_TIMEOUT) {

      measurementValid = false;
      systemState = STATE_SENSOR_ERROR;

      if (debugEnabled(DEBUG_LEVEL_WARN)) {
        Serial.print("[WARN] ToF-Sensor nicht bereit/fehlerhaft, Fehler #");
        Serial.println(sensorErrorCount);
      }

      if (!sensorReinitInProgress &&
          (lastSensorReinit == 0 || now - lastSensorReinit >= SENSOR_REINIT_INTERVAL)) {
        reinitializeSensors(false);
      }
    }

    return;
  }

  sensorFailureSince = 0;
  sensorDataPending = false;

  int32_t corrected =
    (int32_t)distance +
    cfg.sensorOffset;

  if (corrected < 0) {
    corrected = 0;
  }

  if (corrected > 65535) {
    corrected = 65535;
  }

  distance =
    (uint16_t)corrected;

  rawDistance =
    distance;

  // Min/max check
  if (distance < cfg.minDistance ||
      distance > cfg.maxDistance) {

    if (debugEnabled(DEBUG_LEVEL_WARN)) {

      Serial.print(
        "[WARN] Messwert außerhalb Bereich: "
      );

      Serial.print(
        distance
      );

      Serial.println(
        " mm"
      );
    }

    return;
  }

  // Jump check
  if (measurementValid &&
      isfinite(filteredDistance)) {

    float jump =
      fabsf(
        (float)distance -
        filteredDistance
      );

    if (jump > cfg.maxJump) {

      if (debugEnabled(DEBUG_LEVEL_WARN)) {

        Serial.print(
          "[WARN] Sprung verworfen: "
        );

        Serial.print(
          distance
        );

        Serial.print(
          " mm / Sprung="
        );

        Serial.print(
          jump,
          1
        );

        Serial.println(
          " mm"
        );
      }

      return;
    }
  }

  rawDistanceFiltered =
    distance;

  if (cfg.medianEnabled) {

    medianDistance =
      applyMedian(
        (float)distance
      );

  } else {

    medianDistance =
      (float)distance;
  }

  filteredDistance =
    applyMovingAverage(
      medianDistance
    );

  calculateTank(
    filteredDistance
  );

  measurementValid = true;

  lastValidMeasurement =
    now;

  measurementCounter++;

  // Erst nach einer gueltigen, gefilterten Messung erkennen.
  updateRefillDetection(tankLiters);
  if (confirmedRefillPendingLiters > 0.0f) {
    const float refill = confirmedRefillPendingLiters;

    // ToF-Fehlerbehandlung:
    // Eine Nachfuellung wird dem Kalendertag zugeordnet, an dem sie bestaetigt
    // wurde. Falls waehrend des Nachfuellkandidaten Mitternacht ueberschritten
    // wurde, muss der alte Tag ZUERST ohne diese Nachfuellung abgeschlossen
    // werden. Andernfalls wuerde die Menge faelschlich dem Vortag zugerechnet.
    const uint32_t currentDayKey = historyDayKeyNow();

    if (currentDayKey != 0) {
      if (!isfinite(historyDayStartLiters)) {
        // Der aktuelle Messwert enthaelt die Nachfuellung bereits. Fuer eine
        // konsistente Verbrauchsformel Start + Nachfuellung - Ende wird der
        // Tagesstart daher um die erkannte Nachfuellmenge zurueckgerechnet.
        startHistoryDay(max(0.0f, tankLiters - refill));
      } else if (historyLastDayKey != 0 && currentDayKey != historyLastDayKey) {
        // Vortag mit dem letzten Messwert VOR dem neuen Kalendertag beenden.
        closeHistoryDay(historyLastLiters);

        // Neuer Tag beginnt rechnerisch vor der bestaetigten Nachfuellung.
        startHistoryDay(max(0.0f, tankLiters - refill));
      }
    }

    addRefillEvent(refill);
    historyPendingRefill += refill;
    confirmedRefillPendingLiters = 0.0f;

    // Eine bestaetigte Nachfuellung beeinflusst direkt die
    // Verbrauchsrechnung und wird deshalb sofort persistent gesichert.
    historyStoreDirty = true;
    if (saveHistoryStore()) historyLastLiveSaveMs = millis();
  }

  updateDailyHistory(tankLiters);

  // Frische der letzten gueltigen ToF-Messung fuer Health/Recovery ueberwachen.
  lastValidMeasurementMs = millis();

  systemState =
    STATE_RUNNING;

  // 2x bei gültiger Messung
  requestLedBlink(2);

  if (debugEnabled(DEBUG_LEVEL_DETAIL)) {

    Serial.print(
      "[MEAS] Sensor="
    );

    Serial.print(
      sensorName()
    );

    Serial.print(
      " RAW="
    );

    Serial.print(
      rawDistance
    );

    Serial.print(
      " MED="
    );

    Serial.print(
      medianDistance,
      1
    );

    Serial.print(
      " AVG="
    );

    Serial.print(
      filteredDistance,
      1
    );

    Serial.print(
      " mm LEVEL="
    );

    Serial.print(
      tankPercent,
      1
    );

    Serial.print(
      " % "
    );

    Serial.print(
      tankLiters,
      1
    );

    Serial.print(
      " L STATE="
    );

    Serial.println(
      levelState()
    );
  }
}



#endif // FUELLSTANDSMESSER3_UNITY_BUILD
