#ifdef FUELLSTANDSMESSER3_UNITY_BUILD

#include "Runtime.h"

// ============================================================================
// SCHNELLER TOF-HARDWARE-PROBE
// ============================================================================

// Prüft ausschließlich, ob am konfigurierten ToF-I2C-Port ein Gerät antwortet.
// Wichtig für die nicht blockierende Recovery: Wenn der VL53 abgezogen ist,
// werden die eigentlichen Treiber (begin/init/ranging) gar nicht erst gestartet.
bool tofDevicePresent() {

  Wire.beginTransmission(cfg.vlAddress);
  uint8_t error = Wire.endTransmission(true);

  return (error == 0);
}

// ============================================================================
// SENSOR-REINITIALISIERUNG
// ============================================================================

void reinitializeSensors(bool force) {

  if (sensorReinitInProgress) return;

  uint32_t now = millis();
  if (!force && lastSensorReinit != 0 &&
      now - lastSensorReinit < SENSOR_REINIT_INTERVAL) {
    return;
  }

  sensorReinitInProgress = true;
  lastSensorReinit = now;
  sensorReinitCount++;

  Serial.println();
  Serial.println("[SENSOR] Automatische Reinitialisierung gestartet");
  Serial.print("[SENSOR] Versuch #");
  Serial.println(sensorReinitCount);

  // Alte Messwerte sofort als ungültig markieren.
  resetFilters();
  sensorValidZones = 0;
  measurementValid = false;
  sensorFailureSince = 0;
  sensorDataPending = false;

  // Zuerst nur ein sehr kurzer I2C-Hardware-Probe.
  // Ist der VL53 abgezogen, verlassen wir die Recovery praktisch sofort.
  // Dadurch bleiben WLAN, Webserver, MQTT und OLED vollständig lauffähig.
  if (!tofDevicePresent()) {

    vlOK = false;
    cfg.vlSensorType = SENSOR_AUTO;
    systemState = STATE_SENSOR_ERROR;

    if (debugEnabled(DEBUG_LEVEL_WARN)) {
      Serial.println("[SENSOR] ToF nicht am I2C-Bus - Recovery uebersprungen");
    }

    sensorReinitInProgress = false;
    return;
  }

  // BME280 nur neu initialisieren, wenn er tatsächlich nicht bereit ist.
  // Ein funktionierender BME wird von einer ToF-Unterbrechung nicht angefasst.
  if (!bmeOK) {
    bmeInit();
  }

  // Erst jetzt die eigentliche ToF-Treiberinitialisierung.
  vlOK = false;
  tofInit();

  if (vlOK) {
    systemState = STATE_RUNNING;
    Serial.println("[SENSOR] Reinitialisierung erfolgreich");
  } else {
    systemState = STATE_SENSOR_ERROR;
    Serial.println("[SENSOR] Reinitialisierung fehlgeschlagen");
  }

  sensorReinitInProgress = false;
}


// ============================================================================
// SENSOR INITIALIZATION
// ============================================================================

void initializeSensors() {

  Wire.begin(
    cfg.sdaPin,
    cfg.sclPin
  );

  Wire.setClock(
    400000
  );

  // Begrenze blockierende I2C-Zugriffe bei abgezogenem/gestörtem Sensor.
  Wire.setTimeOut(SENSOR_I2C_TIMEOUT_MS);

  delay(100);

  i2cScan();

  oledInit();
  // Hauptseite unmittelbar nach OLED-Initialisierung zeichnen.
  lastOLED = millis();
  oledPage = 0;
  oledPageSequenceActive = false;
  oledTask();

  bmeInit();

  tofInit();
}

// ============================================================================
// SETUP
// ============================================================================


// =====================================================================
// NACHFUELL-ERKENNUNG
// =====================================================================
// Mindestanstieg fuer eine erkannte Nachfuellung.
// Werte unterhalb davon werden als normale Messschwankung behandelt.
// Auf einen 10.000-Liter-Heizoeltank abgestimmt.
// Nachfuellungen von ca. 1.000 bis 4.000 Litern sollen sicher erkannt
// werden, waehrend normale Messschwankungen und kleinere Pegelaenderungen
// nicht als Nachfuellung gewertet werden.
// Eine Fuellung muss ueber mehrere Messungen bestaetigt werden.
constexpr unsigned long REFILL_CONFIRM_MS = 60000UL;
// Plausible maximale Fuellgeschwindigkeit fuer einen 10.000-L-Tank.
// Ein einzelner unrealistisch grosser Sprung wird als Messfehler behandelt.
constexpr float REFILL_MAX_L_PER_MIN = 1500.0f;


// =====================================================================
// STARTUP-DIAGNOSE
// =====================================================================
void printStartupDiagnostics() {
  Serial.println();
  Serial.println(F("=================================================="));
  Serial.println(F(" Fuellstandsmesser V7.3.8"));
  Serial.println(F(" Hardware: ESP32-C3 Super Mini"));
  Serial.print(F(" Arduino ESP32 Core: "));
#if defined(ESP_ARDUINO_VERSION_MAJOR)
  Serial.print(ESP_ARDUINO_VERSION_MAJOR);
  Serial.print('.');
  Serial.print(ESP_ARDUINO_VERSION_MINOR);
  Serial.print('.');
  Serial.println(ESP_ARDUINO_VERSION_PATCH);
#else
  Serial.println(F("unbekannt"));
#endif
  Serial.print(F(" Chip: "));
  Serial.println(ESP.getChipModel());
  Serial.print(F(" Chip Revision: "));
  Serial.println(ESP.getChipRevision());
  Serial.print(F(" CPU: "));
  Serial.print(ESP.getCpuFreqMHz());
  Serial.println(F(" MHz"));
  Serial.print(F(" Flash: "));
  Serial.print(ESP.getFlashChipSize() / (1024UL * 1024UL));
  Serial.println(F(" MB"));
  Serial.print(F(" Freier Heap: "));
  Serial.print(ESP.getFreeHeap());
  Serial.println(F(" Bytes"));
  Serial.print(F(" Heap Minimum: "));
  Serial.print(ESP.getMinFreeHeap());
  Serial.println(F(" Bytes"));
  Serial.print(F(" Largest Block: "));
  Serial.print(ESP.getMaxAllocHeap());
  Serial.println(F(" Bytes"));
  Serial.print(F(" Heap-Status: "));
  Serial.println(heapHealthText(ESP.getMinFreeHeap(), ESP.getMaxAllocHeap()));
  Serial.println(F(" Partition: bitte Arduino IDE / Boarddefinition pruefen"));
  Serial.println(F(" Erwartet: Minimal SPIFFS"));
  Serial.println(F(" Hardware V3.1: SDA=GPIO4, SCL=GPIO5, LED=GPIO8; Display-Taste Default GPIO9"));
  Serial.println(F(" I2C: OLED=0x3C, VL53=0x29, BME280=0x76"));
  Serial.println(F(" ToF: automatische Erkennung VL53L0X / VL53L1X / VL53L5CX"));
  Serial.println(F("=================================================="));
  Serial.println();
}


void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(3000);

  bootResetReason = esp_reset_reason();
  updatePersistentRestartCount();
  lowestLargestBlockSinceBoot = ESP.getMaxAllocHeap();

  Serial.print(F("[BOOT] Reset-Ursache: "));
  Serial.print(resetReasonText(bootResetReason));
  Serial.print(F(" (Code "));
  Serial.print((int)bootResetReason);
  Serial.println(F(")"));
  if (resetReasonIsCritical(bootResetReason)) {
    Serial.println(F("[WARN] Kritische Reset-Ursache erkannt"));
  }
  Serial.print(F("[BOOT] Neustartzaehler: "));
  Serial.println(persistentRestartCount);
  Serial.print(F("[BOOT] System-Health: "));
  Serial.println(overallHealthText());

  printStartupDiagnostics();
  loadHistoryStore();

  Serial.println("[BOOT] Serial 115200 aktiv - ESP32-C3 Super Mini");
  Serial.println();
  Serial.println();
  Serial.println(
    "========================================"
  );
  Serial.println(
    " Fuellstandsmesser3"
  );
  Serial.print(" FW: ");
  Serial.println(
    FW_VERSION
  );
  Serial.println(
    " ESP32-C3 Super Mini"
  );
#if defined(ESP_ARDUINO_VERSION_STR)
  Serial.print(" Arduino ESP32 Core: ");
  Serial.println(ESP_ARDUINO_VERSION_STR);
#elif defined(ESP_ARDUINO_VERSION_MAJOR)
  Serial.print(" Arduino ESP32 Core Major: ");
  Serial.println(ESP_ARDUINO_VERSION_MAJOR);
#else
  Serial.println(" Arduino ESP32 Core: unbekannt");
#endif
  Serial.print(" Build: ");
  Serial.print(__DATE__);
  Serial.print(" ");
  Serial.println(__TIME__);
  Serial.println(" Hardware-Pins: SDA=GPIO4, SCL=GPIO5");
  Serial.println(" I2C: OLED=0x3C, VL53=0x29, BME280=0x76");
  Serial.println(
    "========================================"
  );

  pinMode(
    USER_LED_PIN,
    OUTPUT
  );

  digitalWrite(
    USER_LED_PIN,
    HIGH
  );

  pinMode(
    BOOT_PIN,
    INPUT_PULLUP
  );

  systemState =
    STATE_BOOT;

  loadConfig();
  healthMeasurementIntervalMs = cfg.measurementInterval;

  // Display-Taste getrennt laden, damit alte Config-Bloecke erhalten bleiben.
  loadMqttEnabled();
  loadOtaPassword();
  loadDisplayButtonPin();
  pinMode(displayButtonPin, INPUT_PULLUP);
  Serial.print("[CONFIG] Display-Taste GPIO ");
  Serial.println(displayButtonPin);

  resetFilters();

  initializeSensors();

  // V7.3.8: kurze rein visuelle OLED-Bootanimation.
  oledBootTankAnimation();

  initConsumptionTracking();

  startWiFi();

  Serial.println(
    "[BOOT] Setup abgeschlossen"
  );
}

// ============================================================================
// LOOP
// ============================================================================


// =====================================================================
// AUTOMATISCHE NACHFUELL-ERKENNUNG
// =====================================================================
//
// Ein Anstieg des berechneten Tankinhalts wird nicht als negativer
// Verbrauch verbucht. Erst wenn der Anstieg ueber mehrere Messungen
// stabil ist, wird er als Nachfuellung bestaetigt.
//
// Die Funktion liefert true, wenn eine neue Nachfuellung bestaetigt wurde.
// =====================================================================
bool updateRefillDetection(float currentLiters) {
  static float previousLiters = NAN;
  static unsigned long previousMs = 0;
  static float refillBaselineLiters = NAN;
  static float refillPeakLiters = NAN;
  static unsigned long refillCandidateStartMs = 0;

  const unsigned long now = millis();

  if (!isfinite(currentLiters) || currentLiters < 0.0f) {
    return false;
  }

  if (!isfinite(previousLiters)) {
    previousLiters = currentLiters;
    previousMs = now;
    return false;
  }

  const float deltaUp = currentLiters - previousLiters;
  const unsigned long dt = (now >= previousMs) ? (now - previousMs) : 0UL;

  // Kandidat nach zu langer Zeit verwerfen. Dadurch kann langsame Drift
  // nicht über Stunden/Tage zu einer scheinbaren Nachfüllung anwachsen.
  if (refillCandidateStartMs != 0 &&
      now - refillCandidateStartMs > REFILL_MAX_CANDIDATE_MS) {
    refillCandidateStartMs = 0;
    refillBaselineLiters = NAN;
    refillPeakLiters = NAN;
  }

  // Kandidat beginnt mit dem Füllstand VOR dem ersten plausiblen Anstieg.
  if (refillCandidateStartMs == 0 && deltaUp > 0.0f) {
    bool plausibleStep = true;
    if (dt > 0) {
      const float minutes = dt / 60000.0f;
      if (minutes > 0.0f && (deltaUp / minutes) > REFILL_MAX_L_PER_MIN) {
        plausibleStep = false;
      }
    }

    if (plausibleStep) {
      refillCandidateStartMs = now;
      refillBaselineLiters = previousLiters;
      refillPeakLiters = currentLiters;
    }
  }

  if (refillCandidateStartMs != 0) {
    // Nicht positive Teilbewegungen aufsummieren. Maßgeblich ist der
    // höchste erreichte Tankinhalt relativ zum Ausgangswert. Damit wird
    // Messrauschen während des Tankens nicht mehrfach als Liter gezählt.
    if (!isfinite(refillPeakLiters) || currentLiters > refillPeakLiters) {
      refillPeakLiters = currentLiters;
    }

    const float netRise = (isfinite(refillBaselineLiters) && isfinite(refillPeakLiters))
                            ? refillPeakLiters - refillBaselineLiters
                            : 0.0f;

    // Nach der Mindest-Bestätigungszeit kann die Nachfüllung bestätigt
    // werden, auch wenn der Messwert inzwischen konstant ist.
    if (netRise >= REFILL_MIN_LITERS &&
        (now - refillCandidateStartMs) >= REFILL_CONFIRM_MS) {
      confirmedRefillPendingLiters = netRise;

      Serial.print(F("[REFILL] Nachfuellung erkannt: +"));
      Serial.print(netRise, 1);
      Serial.println(F(" L"));

      refillCandidateStartMs = 0;
      refillBaselineLiters = NAN;
      refillPeakLiters = NAN;
      previousLiters = currentLiters;
      previousMs = now;
      return true;
    }

    // Ein kleiner Rückgang während des Tankens ist Messrauschen und darf
    // den Kandidaten nicht beenden. Erst wenn wir deutlich unter das Peak
    // fallen und die Mindestmenge noch nicht erreicht ist, wird verworfen.
    const float peakDrop = isfinite(refillPeakLiters) ? (refillPeakLiters - currentLiters) : 0.0f;
    if (peakDrop > REFILL_MIN_LITERS && netRise < REFILL_MIN_LITERS) {
      refillCandidateStartMs = 0;
      refillBaselineLiters = NAN;
      refillPeakLiters = NAN;
    }
  }

  previousLiters = currentLiters;
  previousMs = now;
  return false;
}

void loop() {
  updateMemoryDiagnostics();

  webTask();

  wifiTask();

  ntpTask();

  otaTask();

  measurementTask();

  // Die Tageshistorie speichert beim Tageswechsel; SPIFFS hält bis zu 3650 Tage.

  mqttTask();

  oledTask();

  ledTask();

  serialDebugTask();

  yield();
}


#endif // FUELLSTANDSMESSER3_UNITY_BUILD
