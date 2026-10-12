# Fuellstandsmesser3 – Roadmap

Gesamt: `███████████████████░ 95 %`

- 🟢 Messung / ToF / OLED / BME280 / BMP280 / MQTT
- 🟢 Optionale DS3231 / NTP-Rücksync / RTC-Diagnose
- 🟢 Historie / Import / Nachfüllanalyse / Statistikbasis
- 🟢 Home Assistant / OTA
- 🟢 Stabilität und Langzeitabnahme
- 🟢 MQTT-/Home-Assistant-Kompatibilität
- 🟢 Historie / Tageswechsel / Recovery im Realbetrieb
- 🟢 DS3231-Ganggenauigkeit im Realbetrieb abgenommen
- 🟢 Fester UTC-Offset ohne automatische Sommer-/Winterzeit bewusst akzeptiert
- 🟢 USB-bedingter Reset beim Öffnen des seriellen Monitors akzeptiert
- 🔵 AT24C32: redundante Konfiguration, Kalibrierung und Ereignisjournal

Nicht vorgesehen:
- lokale Alarmzentrale
- umfangreiche lokale Verbrauchsprognosen
- separate V8-Funktionssammlung

## AT24C32-Ideen

Das optionale 4-KiB-EEPROM an der erkannten I²C-Adresse `0x57` bleibt in
V7.4.0 unangetastet. Die Erweiterung ist für eine kompatible Folgeversion
vorgesehen und wird in dieser Reihenfolge umgesetzt:

1. **EEPROM-Grundlage und Diagnose**
   - AT24C32 erkennen, Kapazität prüfen und Zustand in Systemseite/API melden
   - versionierter Header mit Kennung, Formatversion und CRC
   - Betrieb ohne, mit leerem oder mit beschädigtem EEPROM bleibt möglich
2. **Redundante Konfigurationssicherung**
   - zwei voneinander unabhängige Slots mit Sequenznummer und CRC
   - erst neuen Slot vollständig schreiben und prüfen, dann als aktuell werten
   - Wiederherstellung nur anbieten bzw. durchführen, wenn die Flash-
     Konfiguration fehlt oder ungültig ist
   - keine Passwörter oder anderen Geheimnisse ungeschützt ablegen
3. **Redundante Kalibrierdaten**
   - Leer-/Vollabstand, Tankgeometrie, Sensoroffset und Kalibrierzeitpunkt
   - unabhängig von normalen Einstellungen versionieren und prüfen
4. **Begrenztes Ereignisjournal als Ringspeicher**
   - Resetursache, Sensor verloren/wiedergefunden, RTC-, WLAN- und MQTT-
     Zustandswechsel, kritischer Füllstand und Konfigurationsänderungen
   - feste 16-Byte-Datensätze mit Zeitstempel, Ereigniscode, Nutzwert,
     Sequenznummer und CRC
   - gleichartige Fehler drosseln und zusammenfassen, damit keine schnellen
     Schreibschleifen entstehen

Geplante Speicheraufteilung:

| Bereich | Größe | Zweck |
|---|---:|---|
| Header | 64 Byte | Kennung, Formatversion und Gesamtstatus |
| Konfiguration A/B | 2 × 512 Byte | atomare Recovery-Kopien |
| Kalibrierung A/B | 2 × 128 Byte | robuste installationsbezogene Werte |
| Ereignisring | 2.560 Byte | ungefähr 160 Datensätze à 16 Byte |
| Reserve | 192 Byte | spätere kompatible Erweiterungen |

Abnahmekriterien:

- Ein unterbrochener Schreibvorgang darf nie beide Konfigurationskopien
  unbrauchbar machen.
- Das Gerät muss bei fehlendem oder defektem EEPROM vollständig weiterlaufen.
- Es wird nur bei tatsächlichen Änderungen geschrieben, niemals in jedem
  Messzyklus.
- EEPROM-Fehler dürfen Messung, Webserver, MQTT und SPIFFS-Historie nicht
  blockieren.
- Die umfangreiche Messhistorie bleibt im vorhandenen SPIFFS-Doppelspeicher;
  das EEPROM ist ausschließlich für Recovery, Kalibrierung und Diagnose.
