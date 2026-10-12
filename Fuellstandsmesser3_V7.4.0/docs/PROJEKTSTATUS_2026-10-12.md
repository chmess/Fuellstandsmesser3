# Fuellstandsmesser3 – Projektstatus 12.10.2026

Aktueller dokumentierter Source: **V7.4.0**.

RTC, BMP280-Unterstützung, Home-Assistant-Discovery, OTA, MQTT und Historie
sind integriert. Alarmierung und umfangreiche Verbrauchsprognosen bleiben
bewusst externen Systemen vorbehalten.

Abgenommen:
- Stabilität / Heap / Largest Block
- WLAN-/MQTT-Recovery und Home-Assistant-Kompatibilität
- Historienpersistenz, Tageswechsel und Recovery im Realbetrieb
- Sensor-Recovery / Hot-Plug
- MQTT-Kompatibilitätstopics `average` und `fuellhoehe`
- DS3231-Ganggenauigkeit sowie Start und Betrieb ohne angeschlossene RTC
- fester UTC-Offset ohne automatische Sommer-/Winterzeit

Verbleibender optionaler Punkt:
- mögliche spätere Nutzung des AT24C32 für Recovery-Konfiguration,
  Ereignisjournal oder Kalibrierdaten

Hardware-Kurzreferenz:
ESP32-C3 Super Mini, OLED 0x3C, ToF 0x29, BME280/BMP280 0x76,
DS3231 optional 0x68, AT24C32 optional 0x57 (unbenutzt),
SDA GPIO4, SCL GPIO5, Display-Taste GPIO9 Default.
