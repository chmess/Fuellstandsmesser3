# Fuellstandsmesser3

ESP32-C3-Füllstandsmesser für ToF/LiDAR, OLED, BME280, MQTT, Weboberfläche,
OTA und persistente 10-Jahres-Historie.

Der aktuelle Sketch liegt unter
`Fuellstandsmesser3_V7.3.8/`. Die `.ino` ist nur der Arduino-Einstieg; die
Firmware ist in mehrere `.cpp`-Module gegliedert. Details stehen in
[`ARCHITEKTUR.md`](Fuellstandsmesser3_V7.3.8/ARCHITEKTUR.md) und der
Dokumentation unter `Fuellstandsmesser3_V7.3.8/docs/`.

Build und Upload erfolgen ausschließlich über den Arduino-Agent:

```sh
arduino-agent build Fuellstandsmesser3
arduino-agent flash Fuellstandsmesser3
```

