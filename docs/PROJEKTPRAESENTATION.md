# Fuellstandsmesser3 – Projektpräsentation

## Idee

Fuellstandsmesser3 macht aus einem ESP32-C3 einen eigenständigen, vernetzten Tankmonitor. Die Messung erfolgt berührungslos von oben per ToF/LiDAR. Das Gerät kombiniert Messung, lokale Anzeige, Weboberfläche, Langzeithistorie, Verbrauchsanalyse und Smart-Home-Anbindung.

## Was das Projekt kann

### Füllstand
- Rohabstand und gefilterter Sensorabstand
- Füllhöhe in Millimetern
- Füllstand in Prozent
- Tankinhalt in Litern
- Quader- und Zylindergeometrie
- Leer-/Voll-Kalibrierung und Kalibrierung über bekannte Literzahl

### Sensorik
- automatische Erkennung von VL53L0X, VL53L1X und VL53L5CX
- Median- und Mittelwertfilter
- Sprung- und Bereichsprüfung
- automatische ToF-Reinitialisierung
- BME280 für Temperatur, Feuchte, Luftdruck und Taupunkt
- Plausibilitätsfilter für Klimadaten

### Historie und Analyse
- bis zu 3650 Tage / rund 10 Jahre
- redundante SPIFFS-Banken A/B
- CRC, Verifikation und atomisches Schreiben
- Charts für ½, 1, 5 und 10 Jahre
- Verbrauch heute, 7, 30 und 365 Tage
- Monats-/Jahresvergleich
- Nachfüllereignisse
- Datenquellen: gemessen, importiert, Testdaten

### CSV
- Export der Historie
- Import vorhandener Tagesdaten
- letzter Datensatz eines Tages gewinnt
- importierte Tageswerte können vorhandene Werte ersetzen
- zukünftige und zu alte Werte werden verworfen

### OLED
Mehrseitige lokale Anzeige für Füllstand, Verbrauch, ToF, Klima, WLAN/MQTT und Zeit/NTP. Die Displaytaste ist standardmäßig GPIO9 und persistent konfigurierbar.

### Weboberfläche
- Dashboard
- Historie und Monatsvergleich
- Einstellungen und Kalibrierung
- System-/Health-Diagnose
- CSV Import/Export
- Testdaten und Servicefunktionen
- Web-OTA

### MQTT & Home Assistant

| Topic | Bedeutung |
|---|---|
| average | aktueller Tankinhalt in Litern |
| fuellhoehe | aktueller gefilterter Sensorabstand in mm |

Zusätzlich liefert die Firmware JSON-Telemetrie. Home Assistant kann per MQTT Discovery Tankinhalt, Prozent, Abstand, Verbrauch, Nachfüllung, WLAN, Uptime, System-Health, Sensortyp und BME280-Werte automatisch erhalten.

## Vorteile

**Berührungslos:** kein mechanischer Schwimmer im Medium.

**Autark:** Kommunikationsprobleme stoppen die Messung nicht.

**Lokale Datenhaltung:** Langzeithistorie bleibt im Gerät.

**Offene Integration:** Web, MQTT und Home Assistant parallel nutzbar.

**Diagnosefähig:** Heap, Resetursache, Largest Block, Recovery- und Reconnect-Zähler machen Stabilitätsprobleme sichtbar.

**Wartbar:** Firmwareupdates über Browser oder ArduinoOTA.

## Hardware

| Bauteil | Zweck | Anschluss |
|---|---|---|
| ESP32-C3 Super Mini | Steuerung, WLAN, Web, History | — |
| VL53L0X/L1X/L5CX | Abstandsmessung | I²C 0x29 |
| SSD1306 128×64 | lokale Anzeige | I²C 0x3C |
| BME280 | Klima | I²C 0x76 |
| Onboard LED | Status | GPIO8 |
| BOOT-Taste | Displayseiten | GPIO9 |
| I²C | gemeinsamer Bus | SDA4 / SCL5 |

## Datenfluss

    ToF -> Filter/Plausibilität -> Tankberechnung -> Livewerte
                                         |-> OLED
                                         |-> Web UI
                                         |-> MQTT / Home Assistant
                                         `-> Tageslogik -> 10-Jahres-Historie
                                                        |-> Charts/Statistik
                                                        `-> CSV Import/Export

## Zuverlässigkeit

Die Firmware bewertet den Zustand über freien/minimalen Heap, Largest Block, Resetursache, Alter der letzten gültigen Messung sowie ToF-, WLAN- und MQTT-Recovery-Zähler.

## Architektur

Die Arduino-INO ist nur Einstiegspunkt. Fachmodule trennen Konfiguration, Sensoren, Messung, MQTT, Netzwerkdienste, Webserver, Runtime und Formatierung. Die Module werden über Fuellstandsmesser3_App.cpp als Unity-Einheit eingebunden.

## Stand V7.3.8

Der aktuelle Stand enthält unter anderem reparierte Langzeitcharts, robustere Import-Persistenz, lückensichere History-Auswertung, plausibilisierte Nachfüllungen, OLED-Bootanimation, ArduinoOTA-Passwort, separat abschaltbares MQTT und ToF-Recovery.

## Grenzen

- Messqualität hängt von Tankgeometrie, Oberfläche, Montage und Kondensation ab.
- nicht eichfähig
- kein Ex-/ATEX-System
- ersetzt keine vorgeschriebene Überfüllsicherung
