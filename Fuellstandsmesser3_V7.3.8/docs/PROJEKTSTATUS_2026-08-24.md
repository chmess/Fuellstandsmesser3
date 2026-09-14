# Fuellstandsmesser3 – Projektstatus 24.08.2026

Aktueller dokumentierter Source: **V7.3.8**.

Schwerpunkt bleibt Stabilität und Abnahme. Home-Assistant-Discovery, OTA,
MQTT und Historie sind bereits vorhanden. Alarmierung und umfangreiche
Verbrauchsprognosen sollen nicht als eigener lokaler Funktionsblock
weitergeführt werden; Auswertung/Alarmierung erfolgen extern.

Wichtige Regressionsthemen:
- Langzeittest / Heap / Largest Block
- WLAN-/MQTT-Recovery
- Historienpersistenz und Tageswechsel
- Home-Assistant-/MQTT-Kompatibilität
- Sensor-Recovery / Hot-Plug
- MQTT-Kompatibilitätstopics `average` und `fuellhoehe` beibehalten

Hardware-Kurzreferenz:
ESP32-C3 Super Mini, OLED 0x3C, ToF 0x29, BME280 0x76,
SDA GPIO4, SCL GPIO5, Display-Taste GPIO9 Default.
