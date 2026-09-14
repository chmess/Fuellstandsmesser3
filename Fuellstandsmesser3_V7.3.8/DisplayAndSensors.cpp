#ifdef FUELLSTANDSMESSER3_UNITY_BUILD

#include "DisplayAndSensors.h"

// ============================================================================
// LED
// ============================================================================

void requestLedBlink(uint8_t pulses) {

  if (pulses == 0) {
    return;
  }

  ledPulsesRequested += pulses;

  if (!ledActive) {

    ledActive = true;
    ledState = true;

    digitalWrite(
      USER_LED_PIN,
      LOW
    );

    ledPulsesRemaining =
      ledPulsesRequested;

    ledPulsesRequested = 0;

    ledTimer = millis();
  }
}

void ledTask() {

  if (!ledActive) {
    return;
  }

  uint32_t now = millis();

  if (now - ledTimer < LED_BLINK_TIME) {
    return;
  }

  ledTimer = now;

  if (ledState) {

    ledState = false;

    digitalWrite(
      USER_LED_PIN,
      HIGH
    );

  } else {

    if (ledPulsesRemaining > 1) {

      ledPulsesRemaining--;

      ledState = true;

      digitalWrite(
        USER_LED_PIN,
        LOW
      );

    } else {

      ledPulsesRemaining = 0;

      ledActive = false;
      ledState = false;

      digitalWrite(
        USER_LED_PIN,
        HIGH
      );

      if (ledPulsesRequested > 0) {

        ledActive = true;
        ledState = true;

        ledPulsesRemaining =
          ledPulsesRequested;

        ledPulsesRequested = 0;

        digitalWrite(
          USER_LED_PIN,
          LOW
        );
      }
    }
  }
}

// ============================================================================
// I2C SCAN
// ============================================================================

void i2cScan() {

  Serial.println();
  Serial.println("I2C Scan:");

  uint8_t found = 0;

  for (uint8_t address = 1;
       address < 127;
       address++) {

    Wire.beginTransmission(address);

    uint8_t error =
      Wire.endTransmission();

    if (error == 0) {

      Serial.print("0x");

      if (address < 16) {
        Serial.print("0");
      }

      Serial.println(
        address,
        HEX
      );

      found++;
    }

    yield();
  }

  if (found == 0) {
    Serial.println(
      "keine Geräte"
    );
  }

  Serial.println();
}

// ============================================================================
// OLED
// ============================================================================

void oledInit() {

  if (!cfg.oledEnabled) {

    Serial.println(
      "OLED deaktiviert"
    );

    oledOK = false;
    return;
  }

  if (display.begin(
        SSD1306_SWITCHCAPVCC,
        cfg.oledAddress)) {

    oledOK = true;

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 0);

    display.println(
      DEVICE_NAME
    );

    display.println(
      FW_VERSION
    );

    display.println(
      "Starting..."
    );

    display.display();

    Serial.println(
      "OLED OK"
    );

  } else {

    oledOK = false;

    Serial.println(
      "OLED FEHLER"
    );
  }
}

void drawTankGraphic(float percent) {

  int x = 4;
  int y = 18;
  int w = 35;
  int h = 38;

  display.drawRect(
    x,
    y,
    w,
    h,
    SSD1306_WHITE
  );

  display.drawLine(
    x - 4,
    y + 3,
    x,
    y + 3,
    SSD1306_WHITE
  );

  display.drawLine(
    x - 4,
    y + 12,
    x,
    y + 12,
    SSD1306_WHITE
  );

  display.drawLine(
    x - 4,
    y + 21,
    x,
    y + 21,
    SSD1306_WHITE
  );

  display.drawLine(
    x - 4,
    y + 30,
    x,
    y + 30,
    SSD1306_WHITE
  );

  display.drawLine(
    x - 4,
    y + 37,
    x,
    y + 37,
    SSD1306_WHITE
  );

  if (!isfinite(percent)) {
    return;
  }

  percent =
    constrain(
      percent,
      0.0f,
      100.0f
    );

  int fillHeight =
    (int)((h - 2) * percent / 100.0f);

  if (fillHeight > 0) {

    display.fillRect(
      x + 2,
      y + h - 1 - fillHeight,
      w - 4,
      fillHeight,
      SSD1306_WHITE
    );
  }
}


// ============================================================================
// OLED DESIGN 2.0
// Optimiert für SSD1306 128x64.
// Bei zweifarbigen OLEDs liegt die Kopfzeile typischerweise in der oberen
// Farbzone (z.B. gelb), während die Nutzdaten darunter dargestellt werden.
// ============================================================================

static void oledHeader(const __FlashStringHelper* title,
                       const String& tag = String()) {
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print(title);

  if (tag.length() > 0) {
    int16_t x = 128 - (int16_t)(tag.length() * 6);
    if (x < 70) x = 70;
    display.setCursor(x, 0);
    display.print(tag);
  }

  display.drawLine(0, 11, 127, 11, SSD1306_WHITE);
}

static void oledLabelValue(
  int y,
  const __FlashStringHelper* label,
  float value,
  uint8_t decimals,
  const __FlashStringHelper* unit
) {
  display.setTextSize(1);
  display.setCursor(0, y);
  display.print(label);

  display.setTextSize(2);
  display.setCursor(48, y - 2);

  if (isfinite(value)) {
    display.print(value, decimals);
  } else {
    display.print(F("--.-"));
  }

  display.setTextSize(1);
  display.setCursor(105, y + 2);
  display.print(unit);
}

static void oledStatusPill(
  int x,
  int y,
  const __FlashStringHelper* label,
  bool ok
) {
  display.setTextSize(1);
  display.setCursor(x, y);
  display.print(label);
  display.print(ok ? F(" OK") : F(" ERR"));
}

static void oledBar(int x, int y, int w, int h, float percent) {
  display.drawRect(x, y, w, h, SSD1306_WHITE);

  if (!isfinite(percent)) return;

  percent = constrain(percent, 0.0f, 100.0f);
  int fw = (int)((w - 2) * percent / 100.0f);

  if (fw > 0) {
    display.fillRect(x + 1, y + 1, fw, h - 2, SSD1306_WHITE);
  }
}

static void oledMiniTank(int x, int y, int w, int h, float percent) {
  display.drawRect(x, y, w, h, SSD1306_WHITE);

  if (!isfinite(percent)) return;

  percent = constrain(percent, 0.0f, 100.0f);
  int fh = (int)((h - 2) * percent / 100.0f);

  if (fh > 0) {
    display.fillRect(
      x + 1,
      y + h - 1 - fh,
      w - 2,
      fh,
      SSD1306_WHITE
    );
  }
}

static void oledCenteredText(const String& s, int y, uint8_t size) {
  display.setTextSize(size);
  int16_t x1, y1;
  uint16_t w, h;
  display.getTextBounds(s, 0, y, &x1, &y1, &w, &h);
  int x = (128 - (int)w) / 2;
  if (x < 0) x = 0;
  display.setCursor(x, y);
  display.print(s);
}

static String oledCurrentDate() {
  time_t now = time(nullptr);
  if (now < 1000000000UL) return String(F("--.--.----"));

  struct tm localTm;
  localtime_r(&now, &localTm);
  char buf[11];
  strftime(buf, sizeof(buf), "%d.%m.%Y", &localTm);
  return String(buf);
}

static void oledDateHeader(const String& rightTag = String()) {
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.print(oledCurrentDate());

  if (rightTag.length() > 0) {
    int16_t x = 128 - (int16_t)(rightTag.length() * 6);
    if (x < 70) x = 70;
    display.setCursor(x, 0);
    display.print(rightTag);
  }

  display.drawLine(0, 11, 127, 11, SSD1306_WHITE);
}

// Grafische Tankdarstellung für die Hauptseite.
// Der Füllstand wird zusätzlich zu Prozent/Liter als echter Tankkörper
// dargestellt. Dadurch bleibt die Information auch auf zweifarbigen
// OLEDs (z.B. gelb/türkis) ohne Farbcodierung eindeutig.
static void oledTankGraphic(int x, int y, int w, int h, float percent) {
  display.drawRoundRect(x, y, w, h, 3, SSD1306_WHITE);

  // Tankdeckel / Anschluss
  int neckX = x + w / 2 - 5;
  display.drawRect(neckX, y - 4, 10, 4, SSD1306_WHITE);

  if (!isfinite(percent)) return;
  percent = constrain(percent, 0.0f, 100.0f);

  int innerH = h - 4;
  int fillH = (int)((innerH * percent) / 100.0f);

  if (fillH > 0) {
    display.fillRoundRect(x + 2, y + h - 2 - fillH,
                          w - 4, fillH, 2, SSD1306_WHITE);
  }

  // kleine Füllstandsmarke
  int markY = y + h - 2 - fillH;
  if (markY >= y + 2 && markY <= y + h - 3) {
    display.drawLine(x - 3, markY, x, markY, SSD1306_WHITE);
  }
}


// Kurze rein visuelle Bootanimation. Sensorik, Historie und Netzwerklogik
// werden dadurch nicht verändert.
static void oledBootTankAnimation() {
  if (!cfg.oledEnabled || !oledOK) return;

  for (uint8_t p = 0; p <= 100; p += 20) {
    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(19, 0);
    display.print(F("FUELLSTAND"));
    oledTankGraphic(48, 18, 32, 40, (float)p);
    display.setCursor(5, 27);
    display.print(p);
    display.print(F("%"));
    display.display();
    delay(110);
    yield();
  }
  delay(120);
}

static void oledShowEasterEgg() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(24, 0);
  display.print(F("GEHEIMMODUS"));
  oledTankGraphic(49, 19, 30, 38, 73.0f);
  display.setCursor(0, 22);
  display.print(F("5x GPIO"));
  display.print(displayButtonPin);
  display.setCursor(0, 36);
  display.print(F("Tank sagt:"));
  display.setCursor(0, 50);
  display.print(F("Alles im Pegel!"));
  display.display();
}

void oledPageTank() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  String state = levelState();
  oledDateHeader(state);

  // Großer Tankinhalt
  display.setTextSize(2);
  display.setCursor(0, 17);
  if (measurementValid && isfinite(tankLiters)) {
    display.print(tankLiters, 1);
  } else {
    display.print(F("---.-"));
  }
  display.setTextSize(1);
  display.setCursor(0, 35);
  display.print(F("LITER"));

  // Große grafische Tankanzeige
  oledTankGraphic(88, 18, 28, 34, tankPercent);

  // Prozent und Sensorabstand
  display.setTextSize(2);
  display.setCursor(0, 45);
  if (measurementValid && isfinite(tankPercent)) {
    display.print(tankPercent, 1);
    display.print(F("%"));
  } else {
    display.print(F("--.-%"));
  }

  display.setTextSize(1);
  display.setCursor(58, 47);
  display.print(F("Abst."));
  display.setCursor(58, 57);
  if (isfinite(filteredDistance)) display.print(filteredDistance, 0);
  else display.print(F("---"));
  display.print(F(" mm"));

  display.display();
}


void oledPageMeasurement() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  oledHeader(F("MESSUNG"), vlOK ? F("TOF OK") : F("TOF ERR"));

  display.setTextSize(1);
  display.setCursor(0, 15);
  display.print(F("ABSTAND"));

  display.setTextSize(2);
  display.setCursor(48, 13);
  if (isfinite(filteredDistance)) {
    display.print(filteredDistance, 1);
  } else {
    display.print(F("---.-"));
  }
  display.setTextSize(1);
  display.setCursor(108, 18);
  display.print(F("mm"));

  display.setCursor(0, 31);
  display.print(F("RAW "));
  if (isfinite(rawDistance)) display.print(rawDistance, 0);
  else display.print(F("---"));
  display.print(F("   MED "));
  if (isfinite(medianDistance)) display.print(medianDistance, 1);
  else display.print(F("---"));

  display.setCursor(0, 43);
  display.print(F("TANKHOEHE "));
  display.print(tankHeightMm, 0);
  display.print(F(" mm"));

  oledBar(0, 56, 128, 7, tankPercent);

  display.display();
}



void oledPageEnvironment() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  oledHeader(F("UMGEBUNG"), F("BME280"));

  display.setTextSize(1);
  display.setCursor(0, 16);
  display.print(F("TEMP"));

  display.setTextSize(2);
  display.setCursor(35, 14);
  if (isfinite(temperature)) display.print(temperature, 1);
  else display.print(F("--.-"));
  display.setTextSize(1);
  display.setCursor(87, 19);
  display.print(F("C"));

  display.setCursor(0, 33);
  display.print(F("FEUCHTE"));
  display.setTextSize(2);
  display.setCursor(52, 31);
  if (isfinite(humidity)) display.print(humidity, 1);
  else display.print(F("--.-"));
  display.setTextSize(1);
  display.setCursor(105, 36);
  display.print(F("%"));

  display.setCursor(0, 48);
  display.print(F("DRUCK "));
  if (isfinite(pressure)) display.print(pressure, 1);
  else display.print(F("---.-"));
  display.print(F(" hPa"));

  display.setCursor(0, 59);
  display.print(F("TAUPUNKT "));
  if (isfinite(dewPoint)) display.print(dewPoint, 1);
  else display.print(F("--.-"));
  display.print(F(" C"));

  display.display();
}



void oledPageNetwork() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  oledHeader(F("NETZWERK"), WiFi.status() == WL_CONNECTED ? F("ONLINE") : F("OFFLINE"));

  if (WiFi.status() == WL_CONNECTED) {
    display.setTextSize(1);
    display.setCursor(0, 16);
    display.print(F("WLAN"));
    display.setTextSize(2);
    display.setCursor(45, 13);
    display.print(F("OK"));

    display.setTextSize(1);
    display.setCursor(0, 31);
    display.print(F("IP "));
    display.print(WiFi.localIP());

    display.setCursor(0, 43);
    display.print(F("RSSI "));
    display.print(WiFi.RSSI());
    display.print(F(" dBm"));

    display.setCursor(0, 56);
    display.print(F("MQTT "));
    display.print(mqttClient.connected() ? F("OK") : F("AUS"));

    display.setCursor(72, 56);
    display.print(F("mDNS "));
    display.print(mdnsOK ? F("OK") : F("AUS"));

  } else if (apMode) {

    oledCenteredText(F("SETUP-AP"), 17, 2);
    display.setTextSize(1);
    display.setCursor(0, 38);
    display.print(F("SSID "));
    display.print(DEVICE_NAME);
    display.print(F("-Setup"));

    display.setCursor(0, 52);
    display.print(F("IP "));
    display.print(WiFi.softAPIP());

  } else {

    oledCenteredText(F("WLAN OFFLINE"), 22, 2);
    display.setTextSize(1);
    display.setCursor(28, 50);
    display.print(F("System laeuft weiter"));
  }

  display.display();
}



void oledPageSystem() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  oledHeader(F("SYSTEM"), String(FW_VERSION));

  display.setTextSize(1);
  display.setCursor(0, 16);
  display.print(F("MESSUNGEN"));
  display.setTextSize(2);
  display.setCursor(67, 14);
  display.print(measurementCounter);

  display.setTextSize(1);
  display.setCursor(0, 31);
  display.print(F("TOF"));
  display.setCursor(30, 31);
  display.print(vlOK ? F("OK") : F("FEHLER"));

  display.setCursor(70, 31);
  display.print(F("BME"));
  display.setCursor(99, 31);
  display.print(bmeOK ? F("OK") : F("FEHLER"));

  display.setCursor(0, 44);
  display.print(F("SENSORFEHLER "));
  display.print(sensorErrorCount);

  display.setCursor(0, 56);
  display.print(F("REINIT "));
  display.print(sensorReinitCount);

  display.display();
}




// ============================================================================
// OLED DESIGN 2.0 – ZUSAETZLICHE SEITEN
// ============================================================================

void oledPageDateTimeV6() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  oledHeader(F("UHRZEIT"), F("NTP"));

  time_t now = time(nullptr);

  if (now >= 1000000000UL) {
    struct tm localTm;
    localtime_r(&now, &localTm);

    char timeBuf[12];
    char dateBuf[16];
    char dayBuf[16];
    strftime(timeBuf, sizeof(timeBuf), "%H:%M:%S", &localTm);
    strftime(dateBuf, sizeof(dateBuf), "%d.%m.%Y", &localTm);
    strftime(dayBuf, sizeof(dayBuf), "%A", &localTm);

    oledCenteredText(String(timeBuf), 17, 3);
    oledCenteredText(String(dayBuf), 45, 1);
    oledCenteredText(String(dateBuf), 55, 1);
  } else {
    oledCenteredText(F("--:--:--"), 18, 3);
    oledCenteredText(F("warte auf NTP-Sync"), 50, 1);
  }

  display.display();
}


void oledPageTankDataV6() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  oledHeader(F("VERBRAUCH"), F("TANK"));

  display.setTextSize(2);
  display.setCursor(0, 14);
  if (isfinite(tankPercent)) display.print(tankPercent, 1);
  else display.print(F("--.-"));
  display.print(F("%"));

  display.setTextSize(1);
  display.setCursor(0, 35);
  display.print(F("HEUTE"));
  display.setCursor(46, 35);
  display.print(consumptionToday(), 1);
  display.print(F(" L"));

  display.setCursor(0, 47);
  display.print(F("7 TAGE"));
  display.setCursor(46, 47);
  display.print(consumptionWeek(), 1);
  display.print(F(" L"));

  display.setCursor(0, 59);
  display.print(F("30 TAGE"));
  display.setCursor(46, 59);
  display.print(consumptionMonth(), 1);
  display.print(F(" L"));

  display.setCursor(88, 35);
  display.print(F("INHALT"));
  display.setCursor(88, 47);
  if (isfinite(tankLiters)) display.print(tankLiters, 0);
  else display.print(F("---"));
  display.print(F("L"));

  display.display();
}



void oledPageStatusV6() {
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);

  oledHeader(F("STATUS"), String(levelState()));

  oledStatusPill(0, 16, F("WLAN"), WiFi.status() == WL_CONNECTED);
  oledStatusPill(68, 16, F("MQTT"), mqttClient.connected());

  oledStatusPill(0, 29, F("TOF"), vlOK);
  oledStatusPill(68, 29, F("BME"), bmeOK);

  display.setTextSize(1);
  display.setCursor(0, 42);
  display.print(F("FEHLER "));
  display.print(sensorErrorCount);

  display.setCursor(68, 42);
  display.print(F("REINIT "));
  display.print(sensorReinitCount);

  display.setCursor(0, 55);
  display.print(F("UPTIME "));
  display.print(millis() / 60000UL);
  display.print(F(" min"));

  display.display();
}



void oledTask() {
  static bool firstMainPage = true;

  if (!cfg.oledEnabled || !oledOK) {
    return;
  }

  const uint32_t now = millis();

  if (oledEasterEggActive) {
    if ((int32_t)(oledEasterEggUntil - now) > 0) return;
    oledEasterEggActive = false;
    oledPage = 0;
    oledPageSequenceActive = false;
    lastOLED = now;
    oledPageTank();
  }

  const bool pressed = (digitalRead(displayButtonPin) == LOW);

  // Entprellung: Zustandswechsel erst nach 40 ms übernehmen.
  if (pressed != (oledButtonLastState == LOW)) {
    oledButtonChangedAt = now;
    oledButtonLastState = pressed ? LOW : HIGH;
  }

  const bool debouncedPressed =
      pressed && ((now - oledButtonChangedAt) >= 40);

  // Eine neue Tastendruck-Flanke schaltet IMMER genau eine Seite weiter.
  if (debouncedPressed && !oledButtonHandled) {

    oledButtonHandled = true;

    // Fünf schnelle Druckflanken innerhalb 2,5 Sekunden aktivieren das Easter Egg.
    if (oledFastClickCount == 0 || (now - oledFastClickWindowStart) > 2500UL) {
      oledFastClickWindowStart = now;
      oledFastClickCount = 1;
    } else {
      ++oledFastClickCount;
    }

    if (oledFastClickCount >= 5) {
      oledFastClickCount = 0;
      oledEasterEggActive = true;
      oledEasterEggUntil = now + 5000UL;
      oledPageSequenceActive = false;
      oledPage = 0;
      oledShowEasterEgg();
      return;
    }

    // Von der Hauptseite auf Seite 1 wechseln.
    // Danach bei jedem weiteren Tastendruck Seite 2, 3, 4 ...
    if (!oledPageSequenceActive) {
      oledPageSequenceActive = true;
      oledPage = 1;
    } else {
      oledPage++;

      if (oledPage >= OLED_PAGE_COUNT) {
        // Nach der letzten Seite wieder zur Hauptseite.
        oledPage = 0;
        oledPageSequenceActive = false;
      }
    }

    oledPageShownAt = now;
    lastOLED = now;

    switch (oledPage) {
      case 0:
        oledPageTank();
        break;

      case 1:
        oledPageMeasurement();
        break;

      case 2:
        oledPageEnvironment();
        break;

      case 3:
        oledPageNetwork();
        break;

      case 4:
        oledPageSystem();
        break;

      case 5:
        oledPageDateTimeV6();
        break;

      case 6:
        oledPageTankDataV6();
        break;

      case 7:
        oledPageStatusV6();
        break;

      default:
        oledPage = 0;
        oledPageSequenceActive = false;
        oledPageTank();
        break;
    }
  }

  // Erst nach Loslassen ist der Taster wieder bereit.
  if (!pressed) {
    oledButtonHandled = false;
  }

  // Zusatzseite automatisch beenden, wenn die konfigurierte
  // Anzeigedauer abgelaufen ist. Danach dauerhaft Hauptseite.
  if (oledPageSequenceActive &&
      (now - oledPageShownAt >= cfg.oledPageInterval)) {

    oledPage = 0;
    oledPageSequenceActive = false;
    lastOLED = now;
    oledPageTank();
  }

  // Hauptseite regelmäßig aktualisieren.
  if (!oledPageSequenceActive &&
      (firstMainPage || (now - lastOLED >= cfg.oledPageInterval))) {

    firstMainPage = false;
    lastOLED = now;
    oledPage = 0;
    oledPageTank();
  }
}



#endif // FUELLSTANDSMESSER3_UNITY_BUILD
