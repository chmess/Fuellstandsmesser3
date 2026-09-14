# Fuellstandsmesser3 – Installation & Betrieb

## Voraussetzungen

Versionierte Abhängigkeiten stehen in Fuellstandsmesser3_V7.3.8/sketch.yaml.

Wesentliche Libraries:
- Adafruit GFX
- Adafruit SSD1306
- Adafruit BME280
- Adafruit Unified Sensor
- Adafruit VL53L0X
- Adafruit VL53L1X
- SparkFun VL53L5CX
- PubSubClient

## Build und Flash

    arduino-agent build Fuellstandsmesser3
    arduino-agent flash Fuellstandsmesser3

Serielle Diagnose: 115200 Baud.

## Inbetriebnahme

1. Hardware gemäß HARDWARE_WIRING.md verdrahten.
2. ToF, OLED und BME280 am I²C-Bus prüfen.
3. Firmware flashen.
4. Bootlog auf erkannte Sensoren prüfen.
5. WLAN konfigurieren.
6. Tankgeometrie und Maße eintragen.
7. Leer-/Vollabstand kalibrieren.
8. Messwerte gegen bekannten Füllstand plausibilisieren.
9. optional MQTT und Home Assistant aktivieren.

## Wartung

- Updates per Web-OTA oder ArduinoOTA
- Systemseite für Heap, Reset und Recovery prüfen
- CSV regelmäßig als zusätzliche Datensicherung exportieren
- Sensorfenster sauber halten
- nach mechanischen Änderungen neu kalibrieren

## Kompatibilität

Die Topics average und fuellhoehe bleiben für bestehende Installationen erhalten. fuellhoehe bedeutet Sensorabstand, nicht Flüssigkeitshöhe.
