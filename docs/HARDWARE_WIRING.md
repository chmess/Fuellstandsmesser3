# Fuellstandsmesser3 V7.4.0 – Hardware und Verdrahtung

## Controller

ESP32-C3 Super Mini mit 4 MB Flash und ESP32 Arduino Core 3.3.11.

## Pinbelegung

| Signal | ESP32-C3 |
|---|---:|
| I²C SDA | GPIO4 |
| I²C SCL | GPIO5 |
| Status-LED | GPIO8 |
| Display-/BOOT-Taste | GPIO9 |

## I²C-Geräte

| Gerät | Adresse | Pflicht |
|---|---:|---|
| VL53L0X / VL53L1X / VL53L5CX | `0x29` | für Füllstandsmessung |
| SSD1306 OLED 128×64 | `0x3C` | optional konfigurierbar |
| BME280 oder BMP280 | `0x76` | optional konfigurierbar |
| DS3231 RTC | `0x68` | optional |
| AT24C32 EEPROM auf vielen RTC-Modulen | häufig `0x57` | unbenutzt |

Die Firmware identifiziert BME280 und BMP280 über die Chip-ID. Die DS3231 wird
über ihr Registerlayout plausibilisiert; ein beliebiges Gerät auf `0x68` gilt
nicht automatisch als RTC. Fehlt die DS3231, arbeitet das Gerät mit NTP weiter.

## Prinzipverdrahtung

```text
ESP32-C3 Super Mini
GPIO4 SDA ----+---- ToF SDA
              +---- OLED SDA
              +---- BME/BMP280 SDA
              `---- DS3231 SDA

GPIO5 SCL ----+---- ToF SCL
              +---- OLED SCL
              +---- BME/BMP280 SCL
              `---- DS3231 SCL

GPIO8 ------------- Status-LED
GPIO9 ------------- BOOT-/Displaytaste
3V3/GND ----------- Versorgung nach Breakout-Spezifikation
```

Der ESP32-C3 arbeitet mit 3,3-V-Logik. Versorgung, Pegel und Pull-ups der
konkret verwendeten Breakout-Boards müssen vor Anschluss geprüft werden.

## RTC und EEPROM

Die DS3231 hält die Zeit bei Netzausfall. Nach einem gültigen NTP-Abgleich wird
sie automatisch aktualisiert. Eine ungültige oder fehlende RTC blockiert den
Messbetrieb nicht. Das häufig mit verbaute AT24C32 besitzt 4 KiB EEPROM, wird
von V7.4.0 aber bewusst nicht beschrieben.

## ToF-Montage

- senkrecht auf die Flüssigkeitsoberfläche ausrichten
- freies Sichtfeld sicherstellen
- Reflexionen von Tankwänden und Einbauten vermeiden
- Sensorfenster vor Staub und Kondensation schützen
- Leer-/Vollabstand nach der mechanischen Montage kalibrieren

## Buildprofil

| Parameter | Wert |
|---|---|
| FQBN | `esp32:esp32:esp32c3` |
| Core | `3.3.11` |
| Flash | 4 MB |
| Partition | Minimal SPIFFS / Large APPS with OTA |
| Seriell | 115200 Baud |
