# Bekannte Probleme / offene Punkte

## Bewusste Entscheidungen

- Die Zeitzone nutzt einen festen UTC-Offset; auf automatische
  Sommer-/Winterzeit wird verzichtet.
- Ein USB-bedingter Reset beim Öffnen des seriellen Ports ist akzeptiert.

## Offene optionale Punkte

- Das AT24C32-EEPROM auf dem RTC-Modul wird noch nicht verwendet. Denkbar sind
  redundante Konfigurationskopien, ein kleines Ereignisjournal oder langlebige
  Kalibrierdaten.
- Messqualität hängt weiterhin von Montage, Reflexionen und Kondensation ab.
