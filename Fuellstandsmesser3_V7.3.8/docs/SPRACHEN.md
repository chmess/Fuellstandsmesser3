# Mehrsprachigkeit

## Compile-Time-Auswahl

Die Firmware nutzt eine feste Sprachvariante pro Build. Dadurch entsteht kein
zusätzlicher Speicherbedarf für unbenötigte Sprachen und die Anzeige bleibt auch
ohne Konfigurationsdatei deterministisch.

| Flagwert | Sprache | Datei |
|---:|---|---|
| `0` | Deutsch | `Language_de.h` |
| `1` | English | `Language_en.h` |

Der Schalter lautet:

```text
-DFUELLSTANDSMESSER3_LANGUAGE=0
```

Er wird im Projekt in `agent.json` unter
`build_options.properties.compiler.cpp.extra_flags` und zusätzlich im
Arduino-Profil `sketch.yaml` dokumentiert.

## Sprachdateien erweitern

Die Einträge in `Language_en.h` sind Quelltext-/HTML-Ersetzungen. Neue sichtbare
Webbegriffe werden als Paar ergänzt:

```cpp
{"Deutscher Begriff", "English translation"},
```

`Language_de.h` enthält absichtlich keine Ersetzungen: Deutsch ist die
Quellsprache. HTML wird vor dem Senden durch `localizeWebHtml()` verarbeitet;
JSON, MQTT-Topics und Persistenzschlüssel werden nicht übersetzt.

## Varianten bauen

```sh
# Deutsch
arduino-agent build Fuellstandsmesser3

# Englisch: agent.json und sketch.yaml auf LANGUAGE=1 setzen
arduino-agent build Fuellstandsmesser3
```
