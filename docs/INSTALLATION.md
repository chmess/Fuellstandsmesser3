# Fuellstandsmesser3 V7.4.0 – Installation und Betrieb

## Voraussetzungen

Die festgeschriebenen Abhängigkeiten stehen in
[`Fuellstandsmesser3_V7.4.0/sketch.yaml`](../Fuellstandsmesser3_V7.4.0/sketch.yaml)
und [`agent.json`](../agent.json).

Wesentliche Bibliotheken:

- Adafruit GFX 1.12.4
- Adafruit SSD1306 2.5.16
- Adafruit BME280 2.3.0
- Adafruit BMP280 2.6.8
- Adafruit Unified Sensor 1.1.15
- Adafruit VL53L0X 1.2.5
- Adafruit VL53L1X 3.1.2
- SparkFun VL53L5CX 1.0.3
- PubSubClient 2.8

Die DS3231-Unterstützung verwendet direkt `Wire` und benötigt keine weitere
Bibliothek.

## Build und Flash

```sh
arduino-agent doctor
arduino-agent scan --non-interactive
arduino-agent status Fuellstandsmesser3
arduino-agent backup Fuellstandsmesser3
arduino-agent build Fuellstandsmesser3
arduino-agent flash Fuellstandsmesser3
```

Serielle Diagnose: 115200 Baud. Das Öffnen des seriellen Ports kann beim
verwendeten ESP32-C3 einen USB-UART-Reset auslösen.

## Inbetriebnahme

1. Hardware gemäß [HARDWARE_WIRING.md](HARDWARE_WIRING.md) verdrahten.
2. Bootlog und I²C-Adressen kontrollieren.
3. Firmware flashen und Upload-Verifikation abwarten.
4. WLAN konfigurieren und Dashboard öffnen.
5. Tankgeometrie und Maße eintragen.
6. Leer-/Vollabstand kalibrieren.
7. Messwerte gegen einen bekannten Füllstand plausibilisieren.
8. Optional MQTT, Home Assistant und DS3231 verwenden.

## Zeitverhalten

- Mit gültiger DS3231 wird die Systemzeit bereits vor WLAN übernommen.
- Nach bestätigtem NTP-Sync wird die RTC aktualisiert.
- Ohne RTC oder bei ungültiger RTC wartet das Gerät wie bisher auf NTP.
- Messung, OLED und Webserver bleiben unabhängig von der RTC funktionsfähig.

## Wartung

- Updates per Web-OTA oder ArduinoOTA
- Systemseite auf Heap, Reset, RTC und Recovery prüfen
- CSV regelmäßig als zusätzliche Datensicherung exportieren
- Sensorfenster sauber halten
- nach mechanischen Änderungen neu kalibrieren

## Kompatibilität

Die MQTT-Topics `average` und `fuellhoehe` bleiben erhalten. `fuellhoehe`
bezeichnet aus Kompatibilitätsgründen weiterhin den Sensorabstand, nicht die
Flüssigkeitshöhe.
