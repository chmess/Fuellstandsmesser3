#ifdef FUELLSTANDSMESSER3_UNITY_BUILD

// ============================================================================
// MQTT TOPICS
// ============================================================================

String mqttBaseTopic() {
  return String(cfg.mqttTopic);
}

String mqttTopic(
  const char *suffix
) {

  String topic =
    mqttBaseTopic();

  if (!topic.endsWith("/")) {
    topic += "/";
  }

  topic += suffix;

  return topic;
}


// ============================================================================
// HOME ASSISTANT MQTT DISCOVERY
// ============================================================================

static String haDeviceId() {
  char id[32];
  snprintf(id, sizeof(id), "fuellstandsmesser3_%08lx",
           (unsigned long)(ESP.getEfuseMac() & 0xFFFFFFFFUL));
  return String(id);
}

static bool publishHaSensorConfig(const char *objectId,
                                  const char *name,
                                  const char *jsonKey,
                                  const char *unit,
                                  const char *deviceClass,
                                  const char *stateClass,
                                  const char *icon = nullptr) {
  if (!mqttClient.connected()) return false;

  const String devId = haDeviceId();
  const String stateTopic = mqttTopic("data");
  const String availabilityTopic = mqttTopic("availability");

  String topic = "homeassistant/sensor/";
  topic += devId;
  topic += "/";
  topic += objectId;
  topic += "/config";

  String payload;
  payload.reserve(900);
  payload += "{";
  payload += "\"name\":\"";
  payload += jsonEscape(String(name));
  payload += "\",";
  payload += "\"unique_id\":\"";
  payload += devId;
  payload += "_";
  payload += objectId;
  payload += "\",";
  payload += "\"state_topic\":\"";
  payload += jsonEscape(stateTopic);
  payload += "\",";
  payload += "\"value_template\":\"{{ value_json.";
  payload += jsonKey;
  payload += " }}\",";
  payload += "\"availability_topic\":\"";
  payload += jsonEscape(availabilityTopic);
  payload += "\",";
  payload += "\"payload_available\":\"online\",";
  payload += "\"payload_not_available\":\"offline\",";

  if (unit && unit[0]) {
    payload += "\"unit_of_measurement\":\"";
    payload += jsonEscape(String(unit));
    payload += "\",";
  }
  if (deviceClass && deviceClass[0]) {
    payload += "\"device_class\":\"";
    payload += deviceClass;
    payload += "\",";
  }
  if (stateClass && stateClass[0]) {
    payload += "\"state_class\":\"";
    payload += stateClass;
    payload += "\",";
  }
  if (icon && icon[0]) {
    payload += "\"icon\":\"";
    payload += icon;
    payload += "\",";
  }

  payload += "\"device\":{";
  payload += "\"identifiers\":[\"";
  payload += devId;
  payload += "\"],";
  payload += "\"name\":\"Fuellstandsmesser3\",";
  payload += "\"manufacturer\":\"DIY\",";
  payload += "\"model\":\"ESP32 Fuellstandsmesser\",";
  payload += "\"sw_version\":\"";
  payload += FW_VERSION;
  payload += "\"";
  payload += "}";
  payload += "}";

  const bool ok = mqttClient.publish(topic.c_str(), payload.c_str(), true);

  if (debugEnabled(DEBUG_LEVEL_DETAIL)) {
    Serial.print("[HA] Discovery ");
    Serial.print(objectId);
    Serial.println(ok ? " OK" : " FEHLER");
  }
  return ok;
}

static bool publishHaBinarySensorConfig(const char *objectId,
                                        const char *name,
                                        const char *jsonKey,
                                        const char *deviceClass,
                                        const char *icon = nullptr) {
  if (!mqttClient.connected()) return false;

  const String devId = haDeviceId();
  const String stateTopic = mqttTopic("data");
  const String availabilityTopic = mqttTopic("availability");

  String topic = "homeassistant/binary_sensor/";
  topic += devId;
  topic += "/";
  topic += objectId;
  topic += "/config";

  String payload;
  payload.reserve(800);
  payload += "{";
  payload += "\"name\":\"";
  payload += jsonEscape(String(name));
  payload += "\",";
  payload += "\"unique_id\":\"";
  payload += devId;
  payload += "_";
  payload += objectId;
  payload += "\",";
  payload += "\"state_topic\":\"";
  payload += jsonEscape(stateTopic);
  payload += "\",";
  payload += "\"value_template\":\"{{ 'OFF' if value_json.";
  payload += jsonKey;
  payload += " else 'ON' }}\",";
  payload += "\"availability_topic\":\"";
  payload += jsonEscape(availabilityTopic);
  payload += "\",";
  payload += "\"payload_available\":\"online\",";
  payload += "\"payload_not_available\":\"offline\",";
  payload += "\"payload_on\":\"ON\",";
  payload += "\"payload_off\":\"OFF\",";

  if (deviceClass && deviceClass[0]) {
    payload += "\"device_class\":\"";
    payload += deviceClass;
    payload += "\",";
  }
  if (icon && icon[0]) {
    payload += "\"icon\":\"";
    payload += icon;
    payload += "\",";
  }

  payload += "\"device\":{";
  payload += "\"identifiers\":[\"";
  payload += devId;
  payload += "\"],";
  payload += "\"name\":\"Fuellstandsmesser3\",";
  payload += "\"manufacturer\":\"DIY\",";
  payload += "\"model\":\"ESP32 Fuellstandsmesser\",";
  payload += "\"sw_version\":\"";
  payload += FW_VERSION;
  payload += "\"";
  payload += "}";
  payload += "}";

  return mqttClient.publish(topic.c_str(), payload.c_str(), true);
}

void publishHomeAssistantDiscovery() {
  if (!mqttClient.connected()) return;

  // Kernwerte zuerst. Jede Config wird retained publiziert, damit HA sie nach
  // einem Neustart des HA-Servers/Brokers erneut entdecken kann.
  publishHaSensorConfig("tank_liters", "Tankinhalt", "level_liters",
                        "L", nullptr, "measurement", "mdi:storage-tank");
  publishHaSensorConfig("tank_percent", "Tankfuellstand", "level_percent",
                        "%", nullptr, "measurement", "mdi:gauge");
  publishHaSensorConfig("distance", "Fuellhoehe Sensorabstand", "filtered_distance_mm",
                        "mm", "distance", "measurement", "mdi:ruler");
  publishHaSensorConfig("consumption_today", "Verbrauch heute", "consumption_today_l",
                        "L", nullptr, "total_increasing", "mdi:fire");
  publishHaSensorConfig("consumption_7d", "Verbrauch 7 Tage", "consumption_7d_l",
                        "L", nullptr, nullptr, "mdi:chart-line");
  publishHaSensorConfig("consumption_30d", "Verbrauch 30 Tage", "consumption_30d_l",
                        "L", nullptr, nullptr, "mdi:chart-line");
  publishHaSensorConfig("last_refill", "Letzte Nachfuellung", "last_refill_l",
                        "L", nullptr, nullptr, "mdi:truck-delivery");
  publishHaSensorConfig("wifi_rssi", "WLAN RSSI", "rssi",
                        "dBm", "signal_strength", "measurement", "mdi:wifi");
  publishHaSensorConfig("uptime", "Uptime", "uptime_s",
                        "s", "duration", nullptr, "mdi:timer-outline");
  publishHaSensorConfig("system_health", "System Health", "system_health",
                        "", nullptr, nullptr, "mdi:heart-pulse");
  publishHaSensorConfig("sensor_type", "ToF Sensortyp", "sensor_type",
                        "", nullptr, nullptr, "mdi:radar");
  publishHaBinarySensorConfig("sensor_problem", "ToF Sensor Problem", "sensor_ok",
                              "problem", "mdi:radar");

  if (bmeOK) {
    publishHaSensorConfig("temperature", "Temperatur", "temperature_c",
                          "°C", "temperature", "measurement", "mdi:thermometer");
    publishHaSensorConfig("humidity", "Luftfeuchtigkeit", "humidity_percent",
                          "%", "humidity", "measurement", "mdi:water-percent");
    publishHaSensorConfig("pressure", "Luftdruck", "pressure_hpa",
                          "hPa", "atmospheric_pressure", "measurement", "mdi:gauge");
    publishHaSensorConfig("dew_point", "Taupunkt", "dew_point_c",
                          "°C", "temperature", "measurement", "mdi:thermometer-water");
  }

  Serial.println("[HA] MQTT Discovery publiziert");
}

// ============================================================================
// MQTT JSON
// ============================================================================

String buildMqttJson() {

  String json;
  json.reserve(2200);

  json += "{";

  json += "\"version\":\"";
  json += jsonEscape(FW_VERSION);
  json += "\",";

  json += "\"device\":\"";
  json += jsonEscape(DEVICE_NAME);
  json += "\",";

  json += "\"sensor\":\"";
  json += jsonEscape(sensorName());
  json += "\",";

  json += "\"sensor_valid_zones\":";
  json += String(sensorValidZones);
  json += ",";

  json += "\"uptime_ms\":";
  json += String(millis());
  json += ",";

  json += "\"measurement_counter\":";
  json += String(measurementCounter);
  json += ",";

  json += "\"measurement_valid\":";
  json += measurementValid ? "true" : "false";
  json += ",";

  json += "\"raw_distance_mm\":";
  json += jsonFloat((float)rawDistance, 1);
  json += ",";

  json += "\"median_distance_mm\":";
  json += jsonFloat(medianDistance, 1);
  json += ",";

  json += "\"filtered_distance_mm\":";
  json += jsonFloat(filteredDistance, 1);
  json += ",";

  json += "\"level_percent\":";
  json += jsonFloat(tankPercent, 1);
  json += ",";

  json += "\"level_liters\":";
  json += jsonFloat(tankLiters, 1);
  json += ",";

  json += "\"consumption_today_l\":";
  json += jsonFloat(consumptionToday(), 1);
  json += ",";

  json += "\"consumption_7d_l\":";
  json += jsonFloat(consumptionWeek(), 1);
  json += ",";

  json += "\"consumption_30d_l\":";
  json += jsonFloat(consumptionMonth(), 1);
  json += ",";

  json += "\"consumption_365d_l\":";
  json += jsonFloat(consumption365Days(), 1);
  json += ",";

  json += "\"consumption_avg_7d_l_day\":";
  json += jsonFloat(consumptionAverage7Days(), 1);
  json += ",";

  json += "\"consumption_avg_30d_l_day\":";
  json += jsonFloat(consumptionAverage30Days(), 1);
  json += ",";

  json += "\"last_refill_l\":";
  json += jsonFloat(lastConfirmedRefillLiters, 1);
  json += ",";

  json += "\"last_refill_timestamp\":";
  json += String(lastRefillTimestamp);
  json += ",";

  json += "\"refill_event_count\":";
  json += String(refillEventCount);
  json += ",";

  json += "\"level_height_mm\":";
  json += jsonFloat(tankHeightMm, 1);
  json += ",";

  json += "\"liters_per_mm\":";
  json += jsonFloat(calculateLitersPerMm(), 2);
  json += ",";

  json += "\"level_state\":\"";
  json += jsonEscape(levelState());
  json += "\",";

  json += "\"temperature_c\":";
  json += jsonFloat(temperature, 1);
  json += ",";

  json += "\"humidity_percent\":";
  json += jsonFloat(humidity, 1);
  json += ",";

  json += "\"pressure_hpa\":";
  json += jsonFloat(pressure, 1);
  json += ",";

  json += "\"dew_point_c\":";
  json += jsonFloat(dewPoint, 1);
  json += ",";

  json += "\"date\":";
  {
    time_t now = time(nullptr);
    struct tm timeinfo;
    char dateBuffer[16];
    char timeBuffer[16];

    if (now > 0 && localtime_r(&now, &timeinfo) != nullptr) {
      strftime(dateBuffer, sizeof(dateBuffer), "%d.%m.%Y", &timeinfo);
      strftime(timeBuffer, sizeof(timeBuffer), "%H:%M:%S", &timeinfo);
    } else {
      strlcpy(dateBuffer, "--.--.----", sizeof(dateBuffer));
      strlcpy(timeBuffer, "--:--:--", sizeof(timeBuffer));
    }

    json += "\"";
    json += jsonEscape(String(dateBuffer));
    json += "\",";

    json += "\"time\":";
    json += "\"";
    json += jsonEscape(String(timeBuffer));
    json += "\",";
  }

  json += "\"sensor_status\":";
  json += String(sensorStatus);
  json += ",";

  json += "\"vl_ok\":";
  json += vlOK ? "true" : "false";
  json += ",";

  json += "\"bme280_ok\":";
  json += bmeOK ? "true" : "false";
  json += ",";

  json += "\"oled_ok\":";
  json += oledOK ? "true" : "false";
  json += ",";

  json += "\"wifi_connected\":";
  json +=
    WiFi.status() == WL_CONNECTED
      ? "true"
      : "false";
  json += ",";

  json += "\"mqtt_connected\":";
  json +=
    mqttClient.connected()
      ? "true"
      : "false";
  json += ",";

  json += "\"spiffs_ready\":"; json += (SPIFFS.totalBytes() > 0) ? "true" : "false"; json += ",";
  json += "\"history_ready\":"; json += (SPIFFS.totalBytes() > 0) ? "true" : "false"; json += ",";

  json += "\"rssi\":";

  if (WiFi.status() == WL_CONNECTED) {
    json += String(WiFi.RSSI());
  } else {
    json += "0";
  }

  json += ",";

  json += "\"geometry\":\"";
  json += jsonEscape(geometryName());
  json += "\",";

  json += "\"sample_count\":";
  json += String(cfg.sampleCount);
  json += ",";

  json += "\"median_enabled\":";
  json += cfg.medianEnabled ? "true" : "false";
  json += ",";

  json += "\"median_samples\":";
  json += String(cfg.medianSamples);
  json += ",";

  json += "\"measurement_interval_ms\":";
  json += String(cfg.measurementInterval);
  json += ",";

  json += "\"mqtt_interval_ms\":";
  json += String(cfg.mqttInterval);
  json += ",";

  json += "\"sensor_offset_mm\":";
  json += String(cfg.sensorOffset);
  json += ",";

  json += "\"empty_distance_mm\":";
  json += jsonFloat(
    cfg.emptyDistance,
    1
  );
  json += ",";

  json += "\"full_distance_mm\":";
  json += jsonFloat(
    cfg.fullDistance,
    1
  );
  json += ",";

  json += "\"low_limit_percent\":";
  json += jsonFloat(
    cfg.lowLevelPercent,
    1
  );
  json += ",";

  json += "\"high_limit_percent\":";
  json += jsonFloat(
    cfg.highLevelPercent,
    1
  );
  json += ",";

  json += "\"tank_height_mm\":";
  json += jsonFloat(
    cfg.tankHeight,
    1
  );
  json += ",";

  json += "\"system_health\":\"";
  json += overallHealthText();
  json += "\",";

  json += "\"uptime_s\":";
  json += String(millis() / 1000UL);
  json += ",";

  json += "\"sensor_type\":\"";
  json += sensorName();
  json += "\",";

  json += "\"sensor_ok\":";
  json += (vlOK && measurementValid) ? "true" : "false";

  json += "}";

  return json;
}

// ============================================================================
// MQTT SERVER CONFIGURATION
// ============================================================================

bool configureMQTTServer() {

  if (cfg.mqttHost[0] == '\0') {
    mqttServerConfigured = false;
    return false;
  }

  if (!mqttServerConfigured) {
    mqttClient.setServer(
      cfg.mqttHost,
      cfg.mqttPort
    );

    // Das aktuelle Telemetrie-JSON ist größer als der kleine PubSubClient-Standardpuffer.
    if (!mqttClient.setBufferSize(MQTT_BUFFER_SIZE)) {
      if (debugEnabled(DEBUG_LEVEL_WARN)) {
        Serial.println("[MQTT] WARN: Puffergröße konnte nicht gesetzt werden");
      }
    } else if (debugEnabled(DEBUG_LEVEL_DETAIL)) {
      Serial.print("[MQTT] Puffer: ");
      Serial.print(MQTT_BUFFER_SIZE);
      Serial.println(" Bytes");
    }

    // Ein nicht erreichbarer Broker darf den Hauptloop nicht fuer
    // viele Sekunden blockieren. PubSubClient verwendet diesen Timeout fuer
    // den TCP/MQTT-Verbindungsaufbau.
    mqttClient.setSocketTimeout(MQTT_SOCKET_TIMEOUT_SEC);
    mqttClient.setKeepAlive(30);

    mqttServerConfigured = true;

    Serial.print("[MQTT] Server konfiguriert: ");
    Serial.print(cfg.mqttHost);
    Serial.print(":");
    Serial.println(cfg.mqttPort);
  }

  return true;
}

// ============================================================================
// MQTT CONNECT
// ============================================================================

bool connectMQTT() {

  if (!mqttEnabled) return false;

  if (WiFi.status() != WL_CONNECTED) {
    return false;
  }

  if (!configureMQTTServer()) {
    if (debugEnabled(DEBUG_LEVEL_WARN)) {
      Serial.println("[MQTT] Kein MQTT-Host konfiguriert");
    }
    return false;
  }

  // Nach WLAN-Wechsel kann der Client noch einen alten Socket halten.
  // Vor einem neuen Connect sicher trennen.
  if (mqttClient.connected()) {
    mqttClient.disconnect();
  }

  String clientId =
    String(DEVICE_NAME) +
    "-" +
    String((uint32_t)(ESP.getEfuseMac() & 0xFFFFFFFF), HEX);

  String willTopic =
    mqttTopic("availability");

  bool connected = false;

  systemState =
    STATE_MQTT_CONNECTING;

  if (debugEnabled(DEBUG_LEVEL_NORMAL)) {
    Serial.print(
      "[MQTT] Verbinde zu "
    );

    Serial.print(
      cfg.mqttHost
    );

    Serial.print(":");

    Serial.println(
      cfg.mqttPort
    );
  }

  if (cfg.mqttUser[0] != '\0') {

    connected =
      mqttClient.connect(
        clientId.c_str(),
        cfg.mqttUser,
        cfg.mqttPassword,
        willTopic.c_str(),
        0,
        true,
        "offline"
      );

  } else {

    connected =
      mqttClient.connect(
        clientId.c_str(),
        willTopic.c_str(),
        0,
        true,
        "offline"
      );
  }

  if (!connected) {

    if (debugEnabled(DEBUG_LEVEL_WARN)) {
      Serial.print("[MQTT] Verbindung fehlgeschlagen, rc=");
      Serial.println(mqttClient.state());

      switch (mqttClient.state()) {
        case -4: Serial.println("[MQTT] Timeout / Netzwerkproblem"); break;
        case -3: Serial.println("[MQTT] Verbindung getrennt"); break;
        case -2: Serial.println("[MQTT] Netzwerkverbindung fehlgeschlagen"); break;
        case -1: Serial.println("[MQTT] MQTT Client nicht verbunden"); break;
        case 1:  Serial.println("[MQTT] Falsche MQTT-Protokollversion"); break;
        case 2:  Serial.println("[MQTT] Client-ID abgelehnt"); break;
        case 3:  Serial.println("[MQTT] MQTT Server nicht erreichbar"); break;
        case 4:  Serial.println("[MQTT] Benutzer/Passwort abgelehnt"); break;
        case 5:  Serial.println("[MQTT] Nicht autorisiert"); break;
        default: break;
      }
    }

    systemState =
      STATE_MQTT_ERROR;

    // Bei laengerem Broker-Ausfall Connect-Versuche zunehmend
    // ausduennen. Messung/Historie laufen davon unabhaengig weiter.
    mqttReconnectDelay = min(mqttReconnectDelay * 2UL,
                             (uint32_t)MQTT_RECONNECT_MAX_INTERVAL);
    return false;
  }

  // Erfolgreiche Verbindung -> Reconnect-Backoff wieder auf Normalwert.
  mqttReconnectDelay = MQTT_RECONNECT_INTERVAL;

  if (mqttEverConnectedSinceBoot) mqttReconnectCountSinceBoot++;
  else mqttEverConnectedSinceBoot = true;

  mqttClient.publish(
    willTopic.c_str(),
    "online",
    true
  );

  systemState =
    STATE_MQTT_CONNECTED;

  if (debugEnabled(DEBUG_LEVEL_NORMAL)) {
    Serial.println(
      "[MQTT] Verbunden"
    );
  }

  String statusTopic =
    mqttTopic("status");

  String statusPayload =
    buildMqttJson();

  bool statusOk =
    mqttClient.publish(
      statusTopic.c_str(),
      statusPayload.c_str(),
      true
    );

  if (debugEnabled(DEBUG_LEVEL_DETAIL)) {
    Serial.print("[MQTT] Status-Payload: ");
    Serial.print(statusPayload.length());
    Serial.print(" Bytes / ");
    Serial.println(statusOk ? "OK" : "FEHLER");
  }

  // Home Assistant Discovery nach jeder erfolgreichen MQTT-Verbindung.
  // Kompatibilitäts-Topics average/fuellhoehe bleiben unveraendert bestehen.
  publishHomeAssistantDiscovery();

  return true;
}

// ============================================================================
// MQTT PUBLISH
// ============================================================================

void mqttPublish() {

  if (!mqttClient.connected()) {
    return;
  }

  String topic =
    mqttTopic("data");

  String payload =
    buildMqttJson();

  if (debugEnabled(DEBUG_LEVEL_DETAIL)) {
    Serial.print("[MQTT] Topic: ");
    Serial.println(topic);
    Serial.print("[MQTT] Payload: ");
    Serial.print(payload.length());
    Serial.println(" Bytes");
  }

  bool ok =
    mqttClient.publish(
      topic.c_str(),
      payload.c_str(),
      false
    );

  if (ok) {

    // 3x bei erfolgreichem MQTT Publish
    requestLedBlink(3);

    if (debugEnabled(DEBUG_LEVEL_DETAIL)) {
      Serial.println(
        "[MQTT] Publish OK"
      );
    }

  } else {

    if (debugEnabled(DEBUG_LEVEL_WARN)) {
      Serial.println(
        "[MQTT] Publish FEHLER"
      );
    }
  }

  // --------------------------------------------------------------------------
  // MQTT-Kompatibilität
  //
  // average    = aktueller Tankinhalt in Litern
  // fuellhoehe = aktueller, gefilterter ToF-Sensorabstand in mm
  //
  // Die Topic-Namen sind in der MQTT-Konfiguration frei einstellbar.
  // --------------------------------------------------------------------------

  if (mqttClient.connected()) {

    if (cfg.mqttAverageTopic[0] != '\0' &&
        isfinite(tankLiters)) {

      String topicAverage =
        mqttTopic(cfg.mqttAverageTopic);

      String payloadAverage =
        jsonFloat(tankLiters, 1);

      bool averageOk =
        mqttClient.publish(
          topicAverage.c_str(),
          payloadAverage.c_str(),
          true
        );

      if (debugEnabled(DEBUG_LEVEL_DETAIL)) {
        Serial.print("[MQTT] Compat average: ");
        Serial.print(topicAverage);
        Serial.print(" = ");
        Serial.print(payloadAverage);
        Serial.println(averageOk ? " OK" : " FEHLER");
      }
    }

    if (cfg.mqttFuellhoeheTopic[0] != '\0' &&
        isfinite(filteredDistance)) {

      String topicFuellhoehe =
        mqttTopic(cfg.mqttFuellhoeheTopic);

      String payloadFuellhoehe =
        jsonFloat(filteredDistance, 1);

      bool fuellhoeheOk =
        mqttClient.publish(
          topicFuellhoehe.c_str(),
          payloadFuellhoehe.c_str(),
          true
        );

      if (debugEnabled(DEBUG_LEVEL_DETAIL)) {
        Serial.print("[MQTT] Compat fuellhoehe: ");
        Serial.print(topicFuellhoehe);
        Serial.print(" = ");
        Serial.print(payloadFuellhoehe);
        Serial.println(fuellhoeheOk ? " OK" : " FEHLER");
      }
    }
  }
}

// ============================================================================
// MQTT TASK
// ============================================================================

void mqttTask() {

  if (!mqttEnabled) {
    if (mqttClient.connected()) mqttClient.disconnect();
    return;
  }

  if (WiFi.status() != WL_CONNECTED) {
    return;
  }

  // Host/Port können über das Webinterface geändert worden sein.
  // configureMQTTServer() setzt die aktuelle Serveradresse vor dem Connect.
  mqttClient.loop();

  uint32_t now = millis();

  if (!mqttClient.connected()) {

    if (now - lastMqttAttempt < mqttReconnectDelay) {
      return;
    }

    lastMqttAttempt = now;

    if (connectMQTT()) {
      lastMqtt = now;
    }

    return;
  }

  if (now - lastMqtt <
      cfg.mqttInterval) {
    return;
  }

  lastMqtt = now;

  mqttPublish();
}

// ============================================================================
// WIFI AP / STA - ROBUST INITIALISIERUNG
// ============================================================================

bool ensureWebServerObject() {

  if (server != nullptr) {
    return true;
  }

  // WebServer erst erzeugen, wenn das Netzwerk bereits initialisiert ist.
  // Dadurch vermeiden wir statische Netzwerk-/FreeRTOS-Abhängigkeiten beim Boot.
  server = new WebServer(80);

  if (server == nullptr) {
    if (debugEnabled(DEBUG_LEVEL_ERROR)) {
      Serial.println("[WEB] FEHLER: WebServer konnte nicht angelegt werden");
    }
    return false;
  }

  if (debugEnabled(DEBUG_LEVEL_DETAIL)) {
    Serial.println("[WEB] WebServer-Objekt angelegt");
  }
  return true;
}

void startWebServer() {

  if (webServerStarted) {
    return;
  }

  if (WiFi.status() != WL_CONNECTED && !apMode) {
    if (debugEnabled(DEBUG_LEVEL_WARN)) {
      Serial.println("[WEB] Start abgebrochen: kein aktives WLAN");
    }
    return;
  }

  if (!ensureWebServerObject()) {
    return;
  }

  // Routen nur einmal registrieren. begin() erst nach aktiver Netzwerk-Schnittstelle.
  if (!webServerConfigured) {
    setupWebServer();
  }

  server->begin();
  webServerStarted = true;

  if (debugEnabled(DEBUG_LEVEL_DETAIL)) {
    Serial.println("[WEB] Server gestartet");
  }
}

void stopWebServer() {

  if (!webServerStarted || server == nullptr) {
    webServerStarted = false;
    return;
  }

  server->stop();
  webServerStarted = false;

  if (debugEnabled(DEBUG_LEVEL_DETAIL)) {
    Serial.println("[WEB] Server gestoppt");
  }
}

void startAccessPoint() {

  // Alte STA-Verbindung sauber beenden, bevor der AP gestartet wird.
  wifiConnecting = false;
  mdnsOK = false;

  if (WiFi.status() == WL_CONNECTED) {
    MDNS.end();
  }

  stopWebServer();

  WiFi.disconnect(false, false);
  delay(20);

  apMode = true;
  systemState = STATE_SETUP_AP;

  // Wenn Zugangsdaten vorhanden sind, AP und STA parallel betreiben.
  // Der Setup-AP bleibt erreichbar, waehrend im Hintergrund das konfigurierte
  // WLAN erneut gesucht wird.
  const bool haveStaConfig = cfg.wifiSSID[0] != '\0';
  WiFi.mode(haveStaConfig ? WIFI_AP_STA : WIFI_AP);
  WiFi.setSleep(false);
  WiFi.setAutoReconnect(true);

  String ssid = String(DEVICE_NAME) + "-Setup";

  bool apStarted = false;

  if (strlen(AP_PASSWORD) >= 8) {
    apStarted = WiFi.softAP(ssid.c_str(), AP_PASSWORD);
  } else {
    apStarted = WiFi.softAP(ssid.c_str());
  }

  if (!apStarted) {
    if (debugEnabled(DEBUG_LEVEL_ERROR)) {
      Serial.println("[WIFI] FEHLER: Setup-AP konnte nicht gestartet werden");
    }
    return;
  }

  delay(100);

  IPAddress apIP = WiFi.softAPIP();

  dnsServer.start(
    DNS_PORT,
    "*",
    apIP
  );

  if (debugEnabled(DEBUG_LEVEL_NORMAL)) {
    Serial.println();
    Serial.println("[WIFI] Setup AP gestartet");
    Serial.print("[WIFI] SSID: ");
    Serial.println(ssid);
    Serial.print("[WIFI] IP: ");
    Serial.println(apIP);
  }

  // Wichtig: Webserver erst NACH WiFi.mode()/softAP() starten.
  startWebServer();

  if (haveStaConfig) {
    WiFi.begin(cfg.wifiSSID, cfg.wifiPassword);
    lastApStaAttempt = millis();
    if (debugEnabled(DEBUG_LEVEL_NORMAL)) {
      Serial.println("[WIFI] Setup-AP bleibt aktiv; STA-Reconnect im Hintergrund");
    }
  }
}

void stopAccessPoint() {

  if (!apMode) {
    return;
  }

  dnsServer.stop();

  stopWebServer();

  WiFi.softAPdisconnect(true);

  apMode = false;
}

void startWiFi() {

  // Kein WLAN konfiguriert -> lokaler Setup-AP.
  if (cfg.wifiSSID[0] == '\0') {
    startAccessPoint();
    return;
  }

  apMode = false;
  wifiConnecting = true;
  wifiConnectedSince = 0;
  mdnsOK = false;
  otaOK = false;
  ntpStarted = false;

  // Vor dem Umschalten von AP auf STA alles sauber beenden.
  dnsServer.stop();
  stopWebServer();

  WiFi.softAPdisconnect(true);
  WiFi.disconnect(false, false);
  delay(20);

  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.setAutoReconnect(true);
  WiFi.setHostname(DEVICE_HOSTNAME);

  // Keine dauerhafte Speicherung durch WiFi.begin().
  WiFi.persistent(false);

  WiFi.begin(
    cfg.wifiSSID,
    cfg.wifiPassword
  );

  systemState = STATE_WIFI_CONNECTING;
  lastWifiAttempt = millis();

  if (debugEnabled(DEBUG_LEVEL_NORMAL)) {
    Serial.println();
    Serial.print("[WIFI] Verbinde mit ");
    Serial.println(cfg.wifiSSID);
  }
}

void wifiTask() {

  if (apMode) {
    if (webServerStarted) {
      dnsServer.processNextRequest();
    }

    // Ohne konfigurierte SSID bleibt es bei einem reinen Setup-AP.
    if (cfg.wifiSSID[0] == '\0') return;

    const uint32_t now = millis();
    const wl_status_t apStaStatus = WiFi.status();

    if (apStaStatus == WL_CONNECTED) {
      if (wifiEverConnectedSinceBoot) wifiReconnectCountSinceBoot++;
      else wifiEverConnectedSinceBoot = true;

      if (debugEnabled(DEBUG_LEVEL_NORMAL)) {
        Serial.println("[WIFI] WLAN im Hintergrund wieder verbunden");
        Serial.print("[WIFI] IP: ");
        Serial.println(WiFi.localIP());
      }

      // AP und Captive DNS beenden, STA-Webdienste sauber neu starten.
      dnsServer.stop();
      stopWebServer();
      WiFi.softAPdisconnect(true);
      apMode = false;
      wifiConnecting = false;
      wifiConnectedSince = now;
      systemState = STATE_WIFI_CONNECTED;

      startWebServer();
      setupMDNS();
      setupNTP();
      setupOTA();
      mqttServerConfigured = false;
      lastMqttAttempt = 0;
      mqttReconnectDelay = MQTT_RECONNECT_INTERVAL;
      return;
    }

    if (now - lastApStaAttempt >= WIFI_AP_STA_RETRY_INTERVAL) {
      lastApStaAttempt = now;
      if (debugEnabled(DEBUG_LEVEL_WARN)) {
        Serial.println("[WIFI] Setup-AP aktiv - erneuter STA-Verbindungsversuch");
      }
      WiFi.begin(cfg.wifiSSID, cfg.wifiPassword);
    }
    return;
  }

  wl_status_t status = WiFi.status();

  if (status == WL_CONNECTED) {

    if (wifiConnecting) {

      wifiConnecting = false;
      wifiConnectedSince = millis();

      if (wifiEverConnectedSinceBoot) wifiReconnectCountSinceBoot++;
      else wifiEverConnectedSinceBoot = true;

      if (debugEnabled(DEBUG_LEVEL_NORMAL)) {
        Serial.println();
        Serial.println("[WIFI] Verbunden");
        Serial.print("[WIFI] IP: ");
        Serial.println(WiFi.localIP());
        Serial.print("[WIFI] RSSI: ");
        Serial.print(WiFi.RSSI());
        Serial.println(" dBm");
      }

      systemState = STATE_WIFI_CONNECTED;

      // Netzwerk ist jetzt garantiert aktiv -> Webserver starten.
      startWebServer();

      setupMDNS();
      setupNTP();
      setupOTA();
    }

    return;
  }

  uint32_t now = millis();

  if (wifiConnecting) {

    if (now - lastWifiAttempt > WIFI_CONNECT_TIMEOUT) {

      if (debugEnabled(DEBUG_LEVEL_WARN)) {
        Serial.println("[WIFI] Timeout - starte Setup-AP");
      }

      wifiConnecting = false;
      systemState = STATE_WIFI_ERROR;

      startAccessPoint();
      return;
    }

    return;
  }

  if (now - lastWifiAttempt < WIFI_RECONNECT_INTERVAL) {
    return;
  }

  lastWifiAttempt = now;

  if (debugEnabled(DEBUG_LEVEL_WARN)) {
    Serial.println("[WIFI] Verbindung verloren - neuer Verbindungsversuch");
  }

  // MQTT-Socket bei WLAN-Verlust sauber schließen.
  if (mqttClient.connected()) {
    mqttClient.disconnect();
  }

  // MQTT-Server nach einem WLAN-Wechsel erneut konfigurieren.
  mqttServerConfigured = false;

  // mDNS/OTA nicht weiter als aktiv behandeln.
  if (mdnsOK) {
    MDNS.end();
    mdnsOK = false;
  }

  otaOK = false;

  // Webserver vor dem erneuten STA-Aufbau sauber stoppen.
  stopWebServer();

  WiFi.disconnect(false, false);
  delay(20);

  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.setAutoReconnect(true);

  WiFi.begin(
    cfg.wifiSSID,
    cfg.wifiPassword
  );

  wifiConnecting = true;
  systemState = STATE_WIFI_CONNECTING;
}



#endif // FUELLSTANDSMESSER3_UNITY_BUILD
