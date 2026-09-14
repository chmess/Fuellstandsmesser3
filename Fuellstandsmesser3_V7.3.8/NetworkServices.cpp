#ifdef FUELLSTANDSMESSER3_UNITY_BUILD

#include "NetworkServices.h"

// ============================================================================
// mDNS
// ============================================================================

void setupMDNS() {

  mdnsOK = false;

  if (WiFi.status() != WL_CONNECTED) {
    return;
  }

  if (MDNS.begin(
        DEVICE_HOSTNAME)) {

    MDNS.addService(
      "http",
      "tcp",
      80
    );

    mdnsOK = true;

    if (debugEnabled(DEBUG_LEVEL_DETAIL)) {
      Serial.print(
        "[mDNS] http://"
      );

      Serial.print(
        DEVICE_HOSTNAME
      );

      Serial.println(
        ".local/"
      );
    }

  } else {

    if (debugEnabled(DEBUG_LEVEL_WARN)) {
      Serial.println(
        "[mDNS] FEHLER"
      );
    }
  }
}

// ============================================================================
// NTP
// ============================================================================

void setupNTP() {

  ntpStarted = false;

  if (!cfg.ntpEnabled) {
    return;
  }

  if (WiFi.status() != WL_CONNECTED) {
    return;
  }

  configTime(
    cfg.timezoneOffset,
    0,
    cfg.ntpServer
  );

  ntpStarted = true;
  lastNtpSync = millis();

  if (debugEnabled(DEBUG_LEVEL_DETAIL)) {
    Serial.print(
      "[NTP] Server: "
    );

    Serial.println(
      cfg.ntpServer
    );

    Serial.print(
      "[NTP] Offset: "
    );

    Serial.println(
      cfg.timezoneOffset
    );
  }
}

void ntpTask() {

  if (!cfg.ntpEnabled) {
    ntpStarted = false;
    return;
  }

  if (WiFi.status() != WL_CONNECTED) {
    ntpStarted = false;
    return;
  }

  uint32_t now = millis();

  if (now - lastNtpSync < NTP_RETRY_INTERVAL && ntpStarted) {
    return;
  }

  time_t epoch = time(nullptr);

  if (epoch < 1000000000UL) {

    configTime(
      cfg.timezoneOffset,
      0,
      cfg.ntpServer
    );

    ntpStarted = true;
    lastNtpSync = now;

    if (debugEnabled(DEBUG_LEVEL_DETAIL)) {
      Serial.println("[NTP] Synchronisation erneut gestartet");
    }

    return;
  }

  lastNtpSync = now;

  if (debugEnabled(DEBUG_LEVEL_SENSOR)) {
    Serial.print("[NTP] Zeit synchronisiert: ");
    Serial.println(epoch);
  }
}

// ============================================================================
// OTA
// ============================================================================

void setupOTA() {

  otaOK = false;

  if (WiFi.status() != WL_CONNECTED) {
    return;
  }

  ArduinoOTA.setHostname(
    DEVICE_HOSTNAME
  );

  if (otaPassword.length() > 0) {
    ArduinoOTA.setPassword(otaPassword.c_str());
  }

  ArduinoOTA.onStart([]() {

    updateRunning = true;

    String type;

    if (ArduinoOTA.getCommand() ==
        U_FLASH) {

      type = "sketch";

    } else {

      type = "filesystem";
    }

    if (debugEnabled(DEBUG_LEVEL_DETAIL)) {
      Serial.print(
        "[OTA] Start "
      );

      Serial.println(
        type
      );
    }
  });

  ArduinoOTA.onEnd([]() {

    updateRunning = false;

    if (debugEnabled(DEBUG_LEVEL_DETAIL)) {
      Serial.println(
        "\n[OTA] Ende"
      );
    }
  });

  ArduinoOTA.onProgress(
    [](unsigned int progress,
       unsigned int total) {

      static uint32_t lastPrint = 0;

      uint32_t now = millis();

      if (now - lastPrint >= 500) {

        lastPrint = now;

        if (debugEnabled(DEBUG_LEVEL_DETAIL)) {
          Serial.printf(
            "[OTA] %u%%\n",
            (progress * 100) / total
          );
        }
      }
    }
  );

  ArduinoOTA.onError(
    [](ota_error_t error) {

      updateRunning = false;

      if (debugEnabled(DEBUG_LEVEL_ERROR)) {
        Serial.printf(
          "[OTA] Fehler[%u]\n",
          error
        );
      }
    }
  );

  ArduinoOTA.begin();

  otaOK = true;

  if (debugEnabled(DEBUG_LEVEL_DETAIL)) {
    Serial.println(
      "[OTA] Bereit"
    );
  }
}

void otaTask() {

  if (!otaOK ||
      WiFi.status() != WL_CONNECTED) {
    return;
  }

  ArduinoOTA.handle();
}



#endif // FUELLSTANDSMESSER3_UNITY_BUILD
