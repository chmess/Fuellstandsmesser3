# Fuellstandsmesser3 – Architektur

Stand: 14.09.2026, Firmware V7.3.8

## Ziel

Die Arduino-`.ino` enthält nur noch den Sketch-Einstieg. Die Firmwarelogik ist
in fachlich getrennte `.cpp`-Dateien aufgeteilt. Das Verhalten, die Pinbelegung,
Konfigurationsstruktur und Persistenzformate bleiben bei diesem Refactoring
unverändert.

## Dateien

| Datei | Verantwortung |
|---|---|
| `Fuellstandsmesser3_V7.3.8.ino` | Arduino-Einstieg, bindet die App-Schnittstelle ein |
| `Fuellstandsmesser3_App.h` | öffentliche Einstiegsschnittstelle (`setup`, `loop`) |
| `Fuellstandsmesser3_App.cpp` | gemeinsame Includes, Zustand, Datentypen und Unity-Einbindung |
| `Config.cpp` | Defaults, Validierung und persistente Konfiguration |
| `DisplayAndSensors.cpp` | LED, I2C-Scan, OLED und BME280 |
| `MeasurementAndSensors.cpp` | ToF-Sensoren, Filter, Tankberechnung und Messzyklus |
| `Mqtt.cpp` | MQTT, JSON und Home-Assistant-Discovery |
| `NetworkServices.cpp` | WLAN-nahe Dienste, mDNS, NTP und OTA |
| `WebServer.cpp` | Weboberfläche, APIs, Historie, CSV und Web-OTA |
| `Runtime.cpp` | Diagnose, Sensor-Recovery, `setup()` und `loop()` |
| `Fuellstandsmesser3_Formatting.cpp/.h` | HTML-/JSON-Formatierung und Taupunkt |

## Buildmodell

Die sieben Fachdateien werden von `Fuellstandsmesser3_App.cpp` als Unity-Einheit
eingebunden. Jede Fachdatei ist zusätzlich mit
`FUELLSTANDSMESSER3_UNITY_BUILD` geschützt. Dadurch bleiben die Dateien im
Sketch sichtbar und einzeln wartbar, während Arduino sie nicht zusätzlich als
separate Übersetzungseinheiten linkt.

Der Agent-Build bleibt die verbindliche Prüfung:

```sh
arduino-agent build Fuellstandsmesser3
arduino-agent flash Fuellstandsmesser3
```

Ziel bleibt `esp32:esp32:esp32c3` mit Core 3.3.11 und den in `agent.json`
festgeschriebenen Bibliotheken. Vor Änderungen wird ein Agent-Backup erstellt.

## Refactoring-Regeln

- Keine Hardware-Pins oder Boardparameter aus den Modulgrenzen ableiten.
- Globale Laufzeitdaten und binär kompatible Strukturen bleiben zunächst in der
  App-Kompilation zentral definiert.
- Funktionsverhalten wird erst nach erfolgreichem Build weiter umstrukturiert.
- Hardwaretests gelten nur als bestanden, wenn der Agent einen seriellen Bericht
  mit tatsächlicher Ausgabe erzeugt.

