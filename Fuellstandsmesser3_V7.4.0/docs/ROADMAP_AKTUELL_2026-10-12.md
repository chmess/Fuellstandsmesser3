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
- 🔵 optionale kompatible V7.4.x-Verbesserungen

Nicht vorgesehen:
- lokale Alarmzentrale
- umfangreiche lokale Verbrauchsprognosen
- separate V8-Funktionssammlung

## AT24C32-Ideen

Das 4-KiB-EEPROM bleibt in V7.4.0 unangetastet. Sinnvolle spätere Nutzungen:

1. zwei CRC-geschützte Konfigurationskopien als Recovery bei beschädigtem Flash
2. kleines Ereignisjournal für Resetursachen, Sensorwechsel und Alarmgrenzen
3. installationsbezogene Kalibrierdaten, die einen Firmwarewechsel überleben

Messwerte im Minuten- oder Tagesrhythmus dort fortlaufend zu speichern ist
wegen begrenzter Schreibzyklen weniger sinnvoll. Die umfangreiche Historie
bleibt besser im vorhandenen SPIFFS-Doppelspeicher.
