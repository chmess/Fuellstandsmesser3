# Fuellstandsmesser3 V7.4.0

Ein robuster, eigenständiger Tankmonitor für den ESP32-C3: berührungslose
Füllstandsmessung, OLED, Web-Dashboard, MQTT/Home Assistant, optionale DS3231
und zehn Jahre lokale Historie.

![Dashboard V7.4.0](docs/screenshots/v7.4.0-dashboard.png)

## Was V7.4.0 auszeichnet

- **Optionale DS3231:** gültige RTC-Zeit steht schon vor WLAN/NTP bereit;
  bestätigte NTP-Zeit wird zurück in die Uhr geschrieben.
- **BME280 oder BMP280:** automatische Chip-Erkennung. Beim BMP280 bleiben
  Feuchte und Taupunkt korrekt leer.
- **Robuster VL53L0X-Betrieb:** kontinuierliche, nichtblockierende Messung und
  begrenzte Recovery-Zugriffe.
- **Überwachter Webserver:** Client-Timeout, Loop-Supervisor sowie Heap-,
  Reset- und Reconnect-Diagnose.
- **RTC im Dashboard:** grün bei gültiger Uhr, orange bei ungültiger Zeit und
  grau, wenn die optionale RTC fehlt.

## Oberfläche

| Historie und Analyse | Systemdiagnose |
|---|---|
| ![Historie](docs/screenshots/v7.4.0-history.png) | ![Systemstatus](docs/screenshots/v7.4.0-systemstatus.png) |

| Einstellungen | Live-API |
|---|---|
| ![Einstellungen](docs/screenshots/v7.4.0-settings.png) | `GET /api/status` liefert Messung, Sensoren, RTC, Zeitquelle und Health |

## Funktionen

| Bereich | Funktionen |
|---|---|
| Füllstand | Abstand, Füllhöhe, Prozent, Liter, Geometrie und Kalibrierung |
| ToF | VL53L0X, VL53L1X und VL53L5CX |
| Umwelt | BME280 oder BMP280, Temperatur, Druck, optional Feuchte/Taupunkt |
| Zeit | NTP plus optionale DS3231 auf `0x68` |
| Anzeige | SSD1306 OLED 128×64, mehrseitig |
| Historie | ½ / 1 / 5 / 10 Jahre, bis zu 3650 Tageswerte |
| Analyse | Verbrauch, Monatsvergleich und Nachfüllereignisse |
| Daten | CRC-geschützte SPIFFS-Banken, CSV-Import und -Export |
| Netzwerk | WLAN, AP/STA, Webserver, mDNS und OTA |
| Smart Home | MQTT, JSON-Telemetrie und Home-Assistant-Discovery |
| Diagnose | Heap, Resetursache, Recovery-Zähler und System-Health |

## Hardware

| Funktion | Hardware / Anschluss |
|---|---|
| Controller | ESP32-C3 Super Mini, 4 MB Flash |
| I²C | SDA GPIO4, SCL GPIO5 |
| ToF | `0x29` |
| OLED | SSD1306, `0x3C` |
| Umwelt | BME280 oder BMP280, `0x76` |
| RTC optional | DS3231, `0x68` |
| EEPROM optional | AT24C32, häufig `0x57`; derzeit nicht beschrieben |
| Status-LED | GPIO8 |
| Display-/BOOT-Taste | GPIO9 |

## Schnellstart

```sh
arduino-agent doctor
arduino-agent scan --non-interactive
arduino-agent status Fuellstandsmesser3
arduino-agent build Fuellstandsmesser3
arduino-agent flash Fuellstandsmesser3
```

Ziel: `esp32:esp32:esp32c3`, ESP32 Arduino Core `3.3.11`, 115200 Baud.
Die festgeschriebenen Abhängigkeiten stehen in
[`sketch.yaml`](Fuellstandsmesser3_V7.4.0/sketch.yaml) und [`agent.json`](agent.json).

## Fertige Firmware

Die geprüfte OTA-Datei `Fuellstandsmesser3-V7.4.0.bin` steht im
[GitHub-Release V7.4.0](https://github.com/chmess/Fuellstandsmesser3/releases/tag/v7.4.0).
Der SHA-256-Prüfwert ist in den Release Notes dokumentiert.

## Dokumentation

- [Projektpräsentation V7.4.0](docs/PROJEKTPRAESENTATION_V7.4.0.md)
- [Release Notes V7.4.0](docs/RELEASE_NOTES_V7.4.0.md)
- [Hardware und Verdrahtung](docs/HARDWARE_WIRING.md)
- [Installation und Betrieb](docs/INSTALLATION.md)
- [Firmware-Architektur](Fuellstandsmesser3_V7.4.0/ARCHITEKTUR.md)
- [Firmware-interne Projektdokumente](Fuellstandsmesser3_V7.4.0/docs/)
- [Änderungshistorie](CHANGELOG.md)

## Sprachen

Deutsch ist die Standard-Websprache. Englisch wird zur Buildzeit über
`FUELLSTANDSMESSER3_LANGUAGE=1` gewählt. JSON-Felder, MQTT-Topics und
Persistenzformate bleiben sprachunabhängig.

## Sicherheit

Hobby-/Entwicklungsprojekt. **Kein zertifiziertes Überfüll-, Leckage-,
Sicherheits- oder Alarmsystem.** Nicht als alleinige Schutzfunktion eines Tanks
verwenden.
