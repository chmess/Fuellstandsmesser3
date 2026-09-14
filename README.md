# Fuellstandsmesser3

**ESP32-C3 Tankmonitor mit ToF/LiDAR, OLED, BME280, MQTT, Home Assistant, OTA und 10-Jahres-Historie.**

> Aktueller Stand: **V7.3.8** · Zielboard: **ESP32-C3 Super Mini** · Arduino Core: **3.3.11**

Fuellstandsmesser3 überwacht geschlossene Heizöl- und Flüssigkeitstanks berührungslos. Ein ToF-Sensor misst den Abstand zur Oberfläche; daraus berechnet die Firmware Füllhöhe, Prozent und Liter. Verbrauch, Nachfüllungen, Klimadaten und Systemzustand werden lokal angezeigt, im Flash gespeichert und über Web/MQTT bereitgestellt.

## Stärken

- **Berührungslos:** kein Schwimmer und kein Sensor im Medium.
- **Autark:** Messung läuft auch bei WLAN-, MQTT- oder NTP-Problemen weiter.
- **Langzeitfähig:** bis zu 3650 Tageswerte in redundanter SPIFFS-Historie.
- **Robust:** Filter, Plausibilitätsprüfung, ToF-Recovery und System-Health.
- **Smart-Home-ready:** MQTT, JSON-Telemetrie und Home-Assistant-Discovery.
- **Wartbar:** Web-OTA und ArduinoOTA.
- **Auswertbar:** Verbrauch, Nachfüllungen, CSV Import/Export und Monats-/Jahresvergleich.

## Funktionsübersicht

| Bereich | Funktionen |
|---|---|
| Füllstand | Abstand, Füllhöhe, %, Liter, Tankgeometrie, Kalibrierung |
| ToF | VL53L0X, VL53L1X, VL53L5CX |
| Klima | BME280: Temperatur, Feuchte, Druck, Taupunkt |
| Anzeige | SSD1306 OLED 128×64, mehrseitig |
| Historie | ½ / 1 / 5 / 10 Jahre, bis 3650 Tage persistent |
| Analyse | Verbrauch heute/7/30/365 Tage, Nachfüllungen, Monatsvergleich |
| Daten | CSV Import/Export, Testdaten, Datenquellen-Markierung |
| Netzwerk | WLAN, AP/STA, Webserver, mDNS, NTP |
| Smart Home | MQTT + Home Assistant MQTT Discovery |
| Update | Web-OTA + ArduinoOTA |
| Diagnose | Heap, Resetursache, Reconnect-/Recovery-Zähler, Health |

## Hardware

| Funktion | Hardware / Anschluss |
|---|---|
| Controller | ESP32-C3 Super Mini, 4 MB Flash |
| I²C | SDA GPIO4, SCL GPIO5 |
| ToF | Adresse 0x29 |
| OLED | SSD1306 128×64, Adresse 0x3C |
| Klima | BME280, Adresse 0x76 |
| Status-LED | GPIO8 |
| Display-/BOOT-Taste | GPIO9 |
| Seriell | 115200 Baud |

## Dokumentation

- [Projektpräsentation](docs/PROJEKTPRAESENTATION.md)
- [Hardware & Verdrahtung](docs/HARDWARE_WIRING.md)
- [Installation & Betrieb](docs/INSTALLATION.md)
- [Firmware-Architektur](Fuellstandsmesser3_V7.3.8/ARCHITEKTUR.md)
- [Bestehende Projektdokumente](Fuellstandsmesser3_V7.3.8/docs/)

## Build

    arduino-agent build Fuellstandsmesser3
    arduino-agent flash Fuellstandsmesser3

## Sicherheit

Hobby-/Entwicklungsprojekt. **Kein zertifiziertes Überfüll-, Leckage-, Sicherheits- oder Alarmsystem.** Nicht als alleinige Schutzfunktion eines Tanks verwenden.
