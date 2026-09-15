# Fuellstandsmesser3 – Hardware & Verdrahtung

## Controller

ESP32-C3 Super Mini, 4 MB Flash, ESP32 Arduino Core 3.3.11.

## Pinbelegung

| Signal | ESP32-C3 |
|---|---:|
| I²C SDA | GPIO4 |
| I²C SCL | GPIO5 |
| Status-LED | GPIO8 |
| Display-/BOOT-Taste | GPIO9 |

## I²C-Geräte

| Gerät | Adresse |
|---|---:|
| VL53L0X / VL53L1X / VL53L5CX | 0x29 |
| SSD1306 OLED 128×64 | 0x3C |
| BME280 | 0x76 |

## Prinzipverdrahtung

    ESP32-C3 Super Mini
    GPIO4 SDA ----+---- ToF SDA
                  +---- OLED SDA
                  `---- BME280 SDA

    GPIO5 SCL ----+---- ToF SCL
                  +---- OLED SCL
                  `---- BME280 SCL

    GPIO8 ------------- Status-LED
    GPIO9 ------------- BOOT-/Displaytaste
    3V3/GND ----------- Versorgung nach Breakout-Spezifikation

Der ESP32-C3 arbeitet mit 3,3-V-Logik. Versorgung und Pegel der verwendeten Breakout-Boards immer prüfen.

## ToF-Montage

- möglichst senkrecht auf die Flüssigkeitsoberfläche
- freies Sichtfeld
- Reflexionen von Tankwänden und Einbauten vermeiden
- Sensorfenster vor Staub und Kondensation schützen
- Leer-/Vollabstand nach der mechanischen Montage kalibrieren

## Tankgeometrie

Unterstützt werden Zylinder- und Quadergeometrien. Die Literberechnung basiert auf den Tankabmessungen und der kalibrierten Messspanne.

## Buildprofil

| Parameter | Wert |
|---|---|
| FQBN | esp32:esp32:esp32c3 |
| Core | 3.3.11 |
| Flash | 4 MB |
| Partition | Minimal SPIFFS / Large APPS with OTA |
| Serial | 115200 |
