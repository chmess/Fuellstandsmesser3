# Fuellstandsmesser3 – Projektdokumentation

Stand: 14.09.2026

Zentrale Projektdokumentation. Die Firmware V7.3.8 wurde aus der monolithischen
`.ino` in eine `.ino`-Einstiegsdatei sowie fachlich getrennte `.cpp`-Module
überführt. Die Funktionslogik, Hardwareparameter, Persistenzformate und
Konfigurationsstruktur bleiben unverändert.

Die Modulaufteilung und das Unity-Buildmodell sind in
[`../ARCHITEKTUR.md`](../ARCHITEKTUR.md) dokumentiert. Buildparameter und
Bibliotheksversionen stehen verbindlich in `../../agent.json`.

Validierter Zwischenstand:

- ESP32-C3, FQBN `esp32:esp32:esp32c3`
- ESP32 Arduino Core `3.3.11`
- Arduino-Agent-Build: erfolgreich
- Upload verifiziert und serieller 20-Sekunden-Test: erfolgreich
