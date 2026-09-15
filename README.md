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

## Sprachen

Die Weboberfläche wird über getrennte Sprachdateien übersetzt:

- `Fuellstandsmesser3_V7.3.8/Language_de.h` – Deutsch (Standard)
- `Fuellstandsmesser3_V7.3.8/Language_en.h` – English

Der Compiler-Schalter ist `FUELLSTANDSMESSER3_LANGUAGE`:

```text
0 = Deutsch
1 = English
```

Für Englisch in `agent.json` und `sketch.yaml` setzen:

```text
compiler.cpp.extra_flags = -DFUELLSTANDSMESSER3_LANGUAGE=1
```

Die Auswahl wird in die Firmware kompiliert; Messwerte, JSON-API, MQTT-Topics
und gespeicherte Datenformate bleiben unabhängig von der Anzeigesprache.
