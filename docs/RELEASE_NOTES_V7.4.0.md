# Release Notes – V7.4.0

## Neu

- optionale DS3231-Unterstützung mit Startzeit vor WLAN/NTP
- NTP-Rücksync zur DS3231
- RTC-Indikator im Dashboard und Diagnosefelder in Web/API
- automatische BME280-/BMP280-Erkennung
- BMP280-Betrieb ohne erfundene Feuchte- oder Taupunktwerte

## Zuverlässigkeit

- VL53L0X kontinuierlich und nichtblockierend betrieben
- begrenzte I²C-Zugriffe und vorsichtige Sensor-Reinitialisierung
- Webclient-Timeout und unabhängiger Loop-Supervisor
- zusätzliche Heap-, Uptime-, WLAN- und Webserver-Diagnose
- MQTT-Erstverbindung gegen eine gleichzeitige SNTP-/DNS-Initialisierung
  abgesichert
- fehlende Umweltwerte werden im Dashboard als `--` statt irreführend als
  Nullwert dargestellt

## Kompatibilität

- vorhandene Konfiguration und SPIFFS-Historie bleiben erhalten
- MQTT-Kompatibilitätstopics bleiben unverändert
- RTC und EEPROM sind optional; das Gerät läuft ohne beide weiter
- AT24C32 wird nicht beschrieben

## Verifiziert

- ESP32-C3 / Core 3.3.11
- VL53L0X, SSD1306 und DS3231 am realen Gerät
- Start und vollständiger Netzbetrieb ohne angeschlossene DS3231
- BME280-/BMP280-Treiber und automatische Chip-ID-Auswahl im Build
- MQTT, Home Assistant Discovery, Weboberfläche und Historie
- 120 Aufrufe der großen Einstellungsseite ohne Fehler oder Neustart
- Stabilität, MQTT und Historie im Realbetrieb abgenommen

## Release-Datei

- `Fuellstandsmesser3-V7.4.0.bin` – OTA-/App-Binärdatei
- SHA-256: `b5427f64dd039db5dc77829c64c65ebc669a5245f2b3d91c5022c32fad9ca3f4`
