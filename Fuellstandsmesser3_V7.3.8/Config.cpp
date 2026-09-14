#ifdef FUELLSTANDSMESSER3_UNITY_BUILD

#include "Config.h"

// ============================================================================
// CONFIG DEFAULTS
// ============================================================================

void setDefaultConfig() {
  memset(&cfg, 0, sizeof(cfg));

  cfg.wifiSSID[0] = '\0';
  cfg.wifiPassword[0] = '\0';

  strlcpy(
    cfg.mqttHost,
    "McSmart",
    sizeof(cfg.mqttHost)
  );

  cfg.mqttPort = 1883;

  cfg.mqttUser[0] = '\0';
  cfg.mqttPassword[0] = '\0';

  strlcpy(
    cfg.mqttTopic,
    DEFAULT_MQTT_TOPIC,
    sizeof(cfg.mqttTopic)
  );

  // MQTT-Kompatibilität: konfigurierbare Plain-Text-Topics average/fuellhoehe.
  strlcpy(
    cfg.mqttAverageTopic,
    "average",
    sizeof(cfg.mqttAverageTopic)
  );

  strlcpy(
    cfg.mqttFuellhoeheTopic,
    "fuellhoehe",
    sizeof(cfg.mqttFuellhoeheTopic)
  );

  strlcpy(
    cfg.ntpServer,
    "pool.ntp.org",
    sizeof(cfg.ntpServer)
  );

  cfg.timezoneOffset = 3600;
  cfg.ntpEnabled = true;

  cfg.sdaPin = OLED_SDA_PIN;
  cfg.sclPin = OLED_SCL_PIN;

  cfg.oledAddress = OLED_ADDRESS;
  cfg.vlAddress = VL_ADDRESS;
  cfg.bmeAddress = BME_ADDRESS;

  // Der ToF-Sensortyp wird beim Start automatisch erkannt.
  cfg.vlSensorType = SENSOR_AUTO;

  cfg.measurementInterval = 20000;
  cfg.mqttInterval = 180000;

  cfg.sampleCount = 10;

  cfg.medianEnabled = true;
  cfg.medianSamples = 5;

  cfg.minDistance = 30;
  cfg.maxDistance = 4000;
  cfg.maxJump = 500;

  cfg.sensorOffset = 0;

  cfg.geometry = GEOMETRY_CYLINDER;

  cfg.diameter = 1000.0f;
  cfg.tankLength = 1000.0f;
  cfg.tankWidth = 1000.0f;
  cfg.tankHeight = 1500.0f;

  cfg.emptyDistance = 1500.0f;
  cfg.fullDistance = 100.0f;

  cfg.lowLevelPercent = 20.0f;
  cfg.highLevelPercent = 90.0f;

  cfg.debugLevel = DEFAULT_DEBUG_LEVEL;

  cfg.oledEnabled = true;
  cfg.oledPageInterval = 5000;
}

// ============================================================================
// CONFIG VALIDATION
// ============================================================================

void validateConfig() {

  if (cfg.measurementInterval < 100 ||
      cfg.measurementInterval > 600000UL) {
    cfg.measurementInterval = 20000;
  }

  if (cfg.mqttInterval < 1000 ||
      cfg.mqttInterval > 3600000UL) {
    cfg.mqttInterval = 180000;
  }

  if (cfg.sampleCount < 1 ||
      cfg.sampleCount > MAX_SAMPLES) {
    cfg.sampleCount = 10;
  }

  if (cfg.medianSamples < 3 ||
      cfg.medianSamples > MAX_MEDIAN_SAMPLES) {
    cfg.medianSamples = 5;
  }

  if ((cfg.medianSamples % 2) == 0) {
    cfg.medianSamples--;

    if (cfg.medianSamples < 3) {
      cfg.medianSamples = 3;
    }
  }

  if (cfg.vlSensorType != SENSOR_VL53L0X &&
      cfg.vlSensorType != SENSOR_VL53L1X &&
      cfg.vlSensorType != SENSOR_VL53L5CX &&
      cfg.vlSensorType != SENSOR_AUTO) {
    cfg.vlSensorType = SENSOR_AUTO;
  }

  if (cfg.geometry != GEOMETRY_CYLINDER &&
      cfg.geometry != GEOMETRY_BOX) {
    cfg.geometry = GEOMETRY_CYLINDER;
  }

  if (cfg.tankHeight <= 0.0f) {
    cfg.tankHeight = 1500.0f;
  }

  if (cfg.diameter <= 0.0f) {
    cfg.diameter = 1000.0f;
  }

  if (cfg.tankLength <= 0.0f) {
    cfg.tankLength = 1000.0f;
  }

  if (cfg.tankWidth <= 0.0f) {
    cfg.tankWidth = 1000.0f;
  }

  if (cfg.emptyDistance <= cfg.fullDistance) {
    cfg.emptyDistance = 1500.0f;
    cfg.fullDistance = 100.0f;
  }

  if (cfg.lowLevelPercent < 0.0f ||
      cfg.lowLevelPercent > 100.0f) {
    cfg.lowLevelPercent = 20.0f;
  }

  if (cfg.highLevelPercent < 0.0f ||
      cfg.highLevelPercent > 100.0f) {
    cfg.highLevelPercent = 90.0f;
  }

  if (cfg.highLevelPercent <= cfg.lowLevelPercent) {
    cfg.lowLevelPercent = 20.0f;
    cfg.highLevelPercent = 90.0f;
  }

  if (cfg.debugLevel > DEBUG_LEVEL_SENSOR) {
    cfg.debugLevel = DEBUG_LEVEL_SENSOR;
  }

  if (cfg.oledPageInterval < 1000 ||
      cfg.oledPageInterval > 3600000UL) {
    cfg.oledPageInterval = 5000;
  }

  if (cfg.mqttPort == 0) {
    cfg.mqttPort = 1883;
  }

  if (cfg.sdaPin > 39) {
    cfg.sdaPin = OLED_SDA_PIN;
  }

  if (cfg.sclPin > 39) {
    cfg.sclPin = OLED_SCL_PIN;
  }

  if (cfg.oledAddress == 0 ||
      cfg.oledAddress > 0x7F) {
    cfg.oledAddress = OLED_ADDRESS;
  }

  if (cfg.vlAddress == 0 ||
      cfg.vlAddress > 0x7F) {
    cfg.vlAddress = VL_ADDRESS;
  }

  if (cfg.bmeAddress == 0 ||
      cfg.bmeAddress > 0x7F) {
    cfg.bmeAddress = BME_ADDRESS;
  }

  if (cfg.ntpServer[0] == '\0') {
    strlcpy(
      cfg.ntpServer,
      "pool.ntp.org",
      sizeof(cfg.ntpServer)
    );
  }

  if (cfg.mqttTopic[0] == '\0') {
    strlcpy(
      cfg.mqttTopic,
      DEFAULT_MQTT_TOPIC,
      sizeof(cfg.mqttTopic)
    );
  }

  if (cfg.maxDistance <= cfg.minDistance) {
    cfg.minDistance = 30;
    cfg.maxDistance = 4000;
  }

  if (cfg.maxJump == 0) {
    cfg.maxJump = 500;
  }

  // MQTT topic migration
  if (strcmp(cfg.mqttTopic, OLD_DEVICE_NAME) == 0 ||
      strstr(cfg.mqttTopic, OLD_DEVICE_NAME) != nullptr) {

    String topic = String(cfg.mqttTopic);

    topic.replace(
      OLD_DEVICE_NAME,
      DEVICE_NAME
    );

    strlcpy(
      cfg.mqttTopic,
      topic.c_str(),
      sizeof(cfg.mqttTopic)
    );

    Serial.println(
      "[CONFIG] Altes MQTT Topic migriert"
    );
  }
}

// ============================================================================
// MQTT EIN/AUS (separate Preference, Config-Struct bleibt kompatibel)
// ============================================================================
static void loadMqttEnabled() {
  preferences.begin("fuellstand", false);
  mqttEnabled = preferences.getBool("mqttEnabled", true);
  preferences.end();
}

static void saveMqttEnabled() {
  preferences.begin("fuellstand", false);
  preferences.putBool("mqttEnabled", mqttEnabled);
  preferences.end();
}

// ============================================================================
// ARDUINO OTA-PASSWORT (separate Preference, Config-Struct bleibt kompatibel)
// Leeres Passwort bedeutet: ArduinoOTA ohne Authentifizierung.
static void loadOtaPassword() {
  preferences.begin("fuellstand", false);
  otaPassword = preferences.getString("otaPassword", "");
  preferences.end();
}

static void saveOtaPassword() {
  preferences.begin("fuellstand", false);
  preferences.putString("otaPassword", otaPassword);
  preferences.end();
}

// ============================================================================
// DISPLAY-TASTE (separate Preference, Config-Struct bleibt kompatibel)
// ============================================================================

static void loadDisplayButtonPin() {
  preferences.begin("fuellstand", false);
  uint32_t stored = preferences.getUInt("displayPin", BOOT_PIN);
  preferences.end();

  // ESP32-Familie: GPIO-Nummern bis 48 abdecken. Ungueltige Werte -> Default.
  if (stored > 48) stored = BOOT_PIN;
  displayButtonPin = (uint8_t)stored;
}

static void saveDisplayButtonPin() {
  if (displayButtonPin > 48) displayButtonPin = BOOT_PIN;
  preferences.begin("fuellstand", false);
  preferences.putUInt("displayPin", displayButtonPin);
  preferences.end();
}

// ============================================================================
// CONFIG SAVE / LOAD
// ============================================================================

void saveConfig() {

  validateConfig();

  preferences.begin("fuellstand", false);

  preferences.putBytes(
    "config",
    &cfg,
    sizeof(cfg)
  );

  preferences.end();

  Serial.println(
    "[CONFIG] Konfiguration gespeichert"
  );
}

bool loadConfig() {

  setDefaultConfig();

  preferences.begin("fuellstand", false);

  size_t len =
    preferences.getBytesLength("config");

  if (len == sizeof(cfg)) {

    preferences.getBytes(
      "config",
      &cfg,
      sizeof(cfg)
    );

    preferences.end();

    validateConfig();

    saveConfig();

    Serial.println(
      "[CONFIG] Konfiguration geladen"
    );

    return true;
  }

  preferences.end();

  validateConfig();
  saveConfig();

  Serial.println(
    "[CONFIG] Keine Konfiguration gefunden - Defaults"
  );

  return false;
}

void factoryReset() {

  Serial.println(
    "[CONFIG] Werkseinstellungen"
  );

  preferences.begin("fuellstand", false);
  preferences.clear();
  preferences.end();

  setDefaultConfig();
  saveConfig();
}

// ============================================================================
// STATUS STRINGS
// ============================================================================

String stateToString() {

  switch (systemState) {

    case STATE_BOOT:
      return "BOOT";

    case STATE_SETUP_AP:
      return "SETUP_AP";

    case STATE_WIFI_CONNECTING:
      return "WIFI_CONNECTING";

    case STATE_WIFI_CONNECTED:
      return "WIFI_CONNECTED";

    case STATE_MQTT_CONNECTING:
      return "MQTT_CONNECTING";

    case STATE_MQTT_CONNECTED:
      return "MQTT_CONNECTED";

    case STATE_RUNNING:
      return "RUNNING";

    case STATE_SENSOR_ERROR:
      return "SENSOR_ERROR";

    case STATE_WIFI_ERROR:
      return "WIFI_ERROR";

    case STATE_MQTT_ERROR:
      return "MQTT_ERROR";
  }

  return "UNKNOWN";
}

String sensorName() {

  if (cfg.vlSensorType == SENSOR_VL53L0X) return "VL53L0X";
  if (cfg.vlSensorType == SENSOR_VL53L1X) return "VL53L1X";
  if (cfg.vlSensorType == SENSOR_VL53L5CX) return "VL53L5CX";
  return "AUTO";
}

String geometryName() {

  if (cfg.geometry == GEOMETRY_BOX) {
    return "Quader";
  }

  return "Zylinder";
}

String levelState() {

  if (!measurementValid ||
      !isfinite(tankPercent)) {
    return "UNKNOWN";
  }

  if (tankPercent <= cfg.lowLevelPercent) {
    return "LOW";
  }

  if (tankPercent >= cfg.highLevelPercent) {
    return "HIGH";
  }

  return "NORMAL";
}



#endif // FUELLSTANDSMESSER3_UNITY_BUILD
