# Fuellstandsmesser3 – Projektdokumentation

Stand: 12.10.2026

Zentrale Projektdokumentation. Die Firmware V7.4.0 ist aus der monolithischen
`.ino` in eine `.ino`-Einstiegsdatei sowie fachlich getrennte `.cpp`-Module
überführt. Die Funktionslogik, Hardwareparameter, Persistenzformate und
Konfigurationsstruktur bleiben kompatibel. V7.4.0 ergänzt eine optionale
DS3231, BME280-/BMP280-Erkennung, nichtblockierende VL53L0X-Verarbeitung und
erweiterte Web-/Heap-Diagnose.

Die Modulaufteilung und das Unity-Buildmodell sind in
[`../ARCHITEKTUR.md`](../ARCHITEKTUR.md) dokumentiert. Buildparameter und
Bibliotheksversionen stehen verbindlich in `../../agent.json`.

Validierter Zwischenstand:

- ESP32-C3, FQBN `esp32:esp32:esp32c3`
- ESP32 Arduino Core `3.3.11`
- Arduino-Agent-Build: erfolgreich
- Upload verifiziert und serieller 20-Sekunden-Test: erfolgreich
- DS3231-Startzeit und NTP-Rücksync: erfolgreich
- BMP280 (Chip-ID 0x58) und VL53L0X real geprüft
- Web-Stresstest mit 120 Einstellungsaufrufen: erfolgreich
