#define FUELLSTANDSMESSER3_UNITY_BUILD 1
#include "Fuellstandsmesser3_App.h"

/*
===============================================================================
 FUELLSTANDSMESSER3 V7.3.8
 ESP32-C3 Super Mini | 4 MB Flash | Minimal SPIFFS (Large APPS with OTA)
===============================================================================

ZWECK
 Heizöl-/Tank-Füllstandsmesser für einen geschlossenen Tank.
 Die Firmware misst den Abstand zur Flüssigkeitsoberfläche per ToF/LiDAR und
 berechnet daraus Füllstand in Millimetern, Prozent und Litern. Zusätzlich
 werden Verbrauch, Nachfüllereignisse, BME280-Umgebungsdaten, Taupunkt,
 Historie und Systemdiagnose bereitgestellt.

PROJEKTPARAMETER / ARDUINO IDE
 - Zielgerät: ESP32-C3 Super Mini
 - Flash: 4 MB
 - ESP32 Arduino Core: 3.x; getestet mit 3.3.11
 - Partition Scheme: Minimal SPIFFS (Large APPS with OTA)
 - Diese Partitionierung ist Projektvorgabe.
 - Große App-Partitionen ermöglichen OTA; SPIFFS bleibt für die Historie.
 - Erwartete Partitionsbasis:
   NVS 20 KB, OTA-Daten 8 KB, App0 1,92 MB, App1 1,92 MB,
   SPIFFS 128 KB, CoreDump 64 KB.

HARDWARE / PINS / I2C
 - SDA GPIO4, SCL GPIO5
 - Display-/BOOT-Taste: standardmäßig GPIO9, in Einstellungen konfigurierbar
 - Onboard-Status-LED: GPIO8
 - OLED SSD1306 128x64: I2C 0x3C
 - BME280: I2C 0x76
 - ToF/LiDAR: Standardadresse I2C 0x29

TOF-SENSOREN
 Automatische Erkennung und Unterstützung für:
 - VL53L0X
 - VL53L1X
 - VL53L5CX
 Die Erkennung berücksichtigt die konfigurierte I2C-Adresse. Messfehler und
 fehlende Sensorbereitschaft werden überwacht. Bei wiederholten Fehlern wird
 eine automatische ToF-Reinitialisierung versucht. Ist der Sensor nicht am
 I2C-Bus vorhanden, wird der Recovery-Versuch ohne blockierende Initialisierung
 übersprungen. Hot-Plug des ToF wurde mit VL53L0X erfolgreich getestet.

BME280
 - Temperatur, relative Luftfeuchte und Luftdruck
 - Taupunkt lokal per Magnus-Approximation
 - Plausibilitätsprüfung vor Übernahme der Messwerte:
   Temperatur -40..85 °C, Feuchte 0..100 %, Luftdruck 300..1100 hPa
 - Ungültige BME-Werte werden verworfen und nicht als neue Messwerte an
   Weboberfläche, MQTT oder Home Assistant weitergegeben.
 - Für das OLED ist kein Hot-Plug-Recovery vorgesehen; nach Ab-/Anstecken kann
   ein Neustart erforderlich sein.

MESSUNG / FILTER / KALIBRIERUNG
 - Füllstand in Millimetern, Prozent und Litern
 - Median-/Mittelwertfilterung und Plausibilitätsprüfung
 - konfigurierbare Tankgeometrie und Kalibrierparameter
 - Berechnung aus Leer-/Voll-Abstand
 - Erkennung und Speicherung von Nachfüllereignissen
 - Verbrauchsberechnung aus Tages-/Füllstandsänderungen
 - Messbetrieb bleibt bei WLAN-, MQTT- oder NTP-Problemen funktionsfähig.

VERBRAUCH / HISTORIE
 - Tageswerte: Endfüllstand, Verbrauch, Nachfüllmenge und Datenquelle
 - Datenquellen: gemessen, importiert oder Testdaten
 - Verbrauch heute, 7 Tage, 30 Tage und 365 Tage
 - Monats-/Jahresauswertung und Jahresvergleich
 - grafischer Monatsvergleich nach Jahren mit Auswahl 3/5/10 Jahre,
   Farblegende und optionaler Wertetabelle
 - Historiencharts: 1/2 Jahr, 1 Jahr, 5 Jahre, 10 Jahre
 - Alle Chart-Zeiträume werden aus der persistenten SPIFFS-Historie bedient;
   längere Zeiträume werden nur für die Darstellung ausgedünnt.
 - Bis zu 10 Jahre / 3650 Tage persistent in SPIFFS
 - RAM-Cache für die letzten 365 Tage bleibt für interne Berechnungen erhalten.
 - Zwei redundante SPIFFS-Banken A/B mit Header, Generation, Anzahl,
   Tagesdaten, CRC und Verifikation nach dem Schreiben
 - Atomisches Schreiben über temporäre Datei/inaktive Bank
 - Historie bleibt über normalen Reset, Neustart und Stromausfall erhalten.
 - Bestehende ältere History-Dateiformate werden beim Laden/Schreiben
   kompatibel behandelt bzw. migriert.
 - 10-Jahres-Testdaten können erzeugt werden.
 - Verbrauchs-/Historiedaten einschließlich Nachfüllereignissen können
   gelöscht werden; der aktuelle Sensor-/Füllstandswert bleibt erhalten.

CSV IMPORT / EXPORT
 - Historiedaten können als CSV importiert und exportiert werden.
 - Importierte Daten werden als eigene Datenquelle markiert.
 - Importdaten dürfen vorhandene Tageswerte desselben Datums ersetzen.
 - Bei mehrfach vorhandenem Datum gewinnt der zuletzt eingelesene Datensatz.
 - Daten älter als 10 Jahre und zukünftige Datumswerte werden verworfen.
 - Importierte Werte werden bei Verbrauchs- und Historienauswertungen
   berücksichtigt.

OLED
 Mehrseitige Anzeige:
 1. Datum/Uhrzeit, Füllstand, Liter, Prozent
 2. Verbrauch
 3. ToF-Sensor
 4. BME280 und Taupunkt
 5. WLAN/MQTT
 6. NTP/Zeit
 Die Display-Taste ist separat persistent konfigurierbar; Standard GPIO9.
 Status-LED GPIO8 arbeitet entsprechend der Firmwarelogik.

WEBINTERFACE
 - Browser-Favicon mit Tankmotiv direkt aus der Firmware
 - Dashboard zeigt zusätzlich den Tankfaktor in Liter/mm
 - Dashboard mit Füllstand, Litern, Prozent, Verbrauch, Verlauf,
   Sensorstatus, BME280/Taupunkt sowie WLAN-/MQTT-Status
 - Historie und grafischer Monatsvergleich
 - Einstellungen und Kalibrierung
 - System-/Health-Diagnose
 - CSV Import/Export und Testdaten
 - Servicefunktionen, Neustart und Löschen von Verbrauchsdaten
 - Web-OTA

WLAN
 - automatische Verbindung und Wiederverbindung
 - AP/STA-Logik für Konfiguration/Wiederherstellung
 - Webserver und Messbetrieb bleiben von MQTT unabhängig.

MQTT
 - MQTT ist in Einstellungen vollständig ein-/ausschaltbar.
 - MQTT-Verbindung und automatische Wiederverbindung nur bei aktiviertem MQTT
 - eigener Einstellungsblock für Broker/Port/Benutzer/Passwort
 - eigener Einstellungsblock für MQTT-Topics
 - JSON-Telemetrie mit Mess-, Verbrauchs-, Sensor- und Systemwerten
 - konfigurierbares Basis-Topic
 - Kompatibilitäts-Topics bleiben erhalten:
   average     = aktueller Tankinhalt in Litern
   fuellhoehe  = aktueller Sensorabstand in mm
 - Kommunikations-/Brokerfehler blockieren die Messung nicht.

HOME ASSISTANT
 - MQTT Discovery über konfigurierbaren Discovery-Prefix
 - Discovery u.a. für Tankinhalt, Prozent, Abstand, Verbrauch,
   Nachfüllung, WLAN, Uptime, System-Health, Sensortyp sowie BME280-Werte
 - interne Alarme und Verbrauchsprognosen sind nicht vorgesehen;
   weitergehende Auswertung erfolgt extern.

ZEIT / NTP
 - konfigurierbarer fester UTC-Offset
 - automatische NTP-Synchronisation
 - NTP blockiert den Messbetrieb nicht
 - vor gültiger Zeitsynchronisation werden keine erfundenen Zeitstempel
   für die Historie verwendet.

OTA
 - Web-OTA
 - ArduinoOTA mit persistent konfigurierbarem Passwort
 - ArduinoOTA-Passwort wird separat in Preferences gespeichert und nicht
   im Config-Struct abgelegt.
 - Partitionierung muss OTA-fähige große App-Partitionen bereitstellen.

SYSTEM-HEALTH / LANGZEITDIAGNOSE
 - Reset-/Neustartursache und persistent gespeicherter Neustartzähler
 - aktueller freier Heap, Minimum-Heap, Largest Block und kleinster
   Largest Block seit Boot
 - Heap-Bewertung im aktuellen Code:
   OK ab 20 KB Minimum-Heap, Warnung 19..20 KB, kritisch unter 19 KB;
   Largest-Block-Grenzen werden zusätzlich berücksichtigt.
 - Praxistests des ESP32-C3 zeigten bereits in älteren Firmwareständen
   kurzzeitige Minimum-Heap-Werte um etwa 18..20 KB; deshalb sind einzelne
   Minima im Langzeittest zusammen mit Stabilität/Resetursache zu bewerten.
 - Zähler seit Boot für ToF-Reinitialisierungen, WLAN-Reconnects und
   MQTT-Reconnects
 - Zeit seit letzter gültiger ToF-Messung
 - Gesamtbewertung System-Health

DEBUG-STUFEN
 0 = Aus
 1 = Fehler
 2 = Warnungen
 3 = Normal
 4 = Detail
 5 = Sensor/Kommunikation

SOFTWARE / BIBLIOTHEKEN
 - ESP32 Arduino Core 3.3.11
 - Adafruit SSD1306 2.5.17
 - Adafruit GFX 1.12.6
 - Adafruit BME280 2.3.0
 - Adafruit VL53L0X 1.2.5
 - Adafruit VL53L1X 3.1.2
 - SparkFun VL53L5CX Library
 - PubSubClient 2.8
 - ESP32-Systembibliotheken: Wire, WiFi, WebServer, Preferences, SPIFFS,
   Update, ArduinoOTA, ESPmDNS und Zeit/NTP
 - Abhängigkeiten: Adafruit BusIO, Adafruit Unified Sensor

STAND V7.3.8
 - 5-/10-Jahres-Charts auf maximal 400 Datenpunkte erhöht; Linienabstand entsprechend angepasst.
 - 5-/10-Jahres-Füllstandslinie repariert: die erlaubte Linienlücke richtet
   sich jetzt nach der zeitlichen Chart-Ausdünnung statt fest 1,5 Tage.
 - Dashboard- und Historie-Verbrauchschart verwenden dieselbe Legende
   für Füllstand, Verbrauch und Nachfüllung.
 - Import-Persistenz grundlegend korrigiert: nach der In-Place-Konvertierung
   wird der komplette unbenutzte CompactHistoryDay-Bereich genullt und die
   importierten Datensätze werden vor dem SPIFFS-Commit in ein dichtes
   Kalenderlayout normalisiert. Damit können RAM-Restdaten nicht mehr als
   dayOffset/refillLiters interpretiert werden.
 - Historische Nachfüllereignisse werden zusätzlich gegen Datum und Tankkapazität
   plausibilisiert; Zukunftsdaten bzw. unmögliche Mengen werden verworfen.
 - Historie-Frontend repariert: fehlende JavaScript-Anweisungstrennung in
   der Analysefunktion behoben; dadurch funktionieren die History-Charts wieder.
 - Historische Nachfüllereignisse werden aus der persistenten History
   rekonstruiert; die separate NVS-Ereignisliste ist nicht mehr die
   maßgebliche Quelle. Alte/stale Phantomereignisse werden dadurch entfernt.
 - CSV-Import lückensicher: leere SPIFFS-Kalenderslots werden beim Merge ignoriert.
 - Automatische Nachfüllableitung für einfache Datum/Liter-Imports wieder aktiv,
   aber nur bei bestätigtem Sprung: direkter Folgetag, Mindestanstieg und
   Bestätigung durch nachfolgende Messpunkte. Datenlücken sind ausgeschlossen.
 - Explizite Nachfüllspalten aus dem erweiterten Export bleiben erhalten.
 - Komfort/Visualisierung: Browser-Favicon, Liter/mm-Anzeige,
   OLED-Bootanimation und 5x-Displaytasten-Easter-Egg.
 - Basis ist der lückensichere Historienstand V7.2.23.
 - Lücken in importierten Historien-/Verbrauchsdaten werden sicher behandelt.
 - Leere SPIFFS-Kalenderslots werden nicht mehr als Messdatensätze ausgewertet.
 - Chart-Zeiträume sind echte Kalenderfenster; Lücken werden im Verlauf unterbrochen.
 - Verbrauch über lückenhafte Daten summiert nur tatsächlich vorhandene Tageswerte.
 - ArduinoOTA-Passwort in den Einstellungen ergänzt und persistent gespeichert.
 - Dokumentation und Quelltextkommentare basieren auf dem Stand V7.2.21.
   und Quelltextkommentare auf den aktuellen Projektstand.
 - BME280-Plausibilitätsfilter aus V7.2.20 ist enthalten.
 - MQTT ist separat abschaltbar; MQTT und MQTT-Topics sind getrennte
   Einstellungsblöcke.
 - ToF-Hot-Plug-Recovery mit VL53L0X getestet.
 - Historiencharts 1/2/5/10 Jahre und grafischer Monatsvergleich getestet.
 - Verbrauchsberechnung und Nachfüllanzeige wurden mit vorhandenen
   Historiedaten plausibilisiert.
===============================================================================
*/


#include <Arduino.h>
#include "esp_system.h"
#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <ESPmDNS.h>
#include <ArduinoOTA.h>
#include <Preferences.h>

Preferences preferences;
#include <FS.h>
#include <SPIFFS.h>
#include <Update.h>
#include <time.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_BME280.h>
#include <Adafruit_VL53L0X.h>
#include <Adafruit_VL53L1X.h>
#include <SparkFun_VL53L5CX_Library.h>

#include <PubSubClient.h>

struct CompactHistoryDay;

// ===== 10-JAHRES-HISTORIE / PERSISTENZ =====
// Persistente Tageshistorie in SPIFFS. Feste Aufbewahrung: 10 Jahre.
// Quelle je Tagesdatensatz: MEASURED / IMPORTED / TEST.
// Die Historie bleibt kompakt in SPIFFS; im RAM werden nur die letzten 365 Tage gecacht.
// Zwei redundante SPIFFS-Banken A/B + temporäre Datei fuer atomisches Schreiben.
#ifndef HISTORY_DAYS
#define HISTORY_DAYS 3650
#define HISTORY_RAM_CACHE_DAYS 365
// 10 Jahre dauerhaft in SPIFFS; maximal 365 Tage als interner RAM-Cache.
#endif
#define HISTORY_SPIFFS_MAGIC   0x48495339UL
#define HISTORY_SPIFFS_VERSION 0x00060203UL  // Aktuelles Format: Fuellstand, Verbrauch und Nachfuellung in ganzen Litern
#define HISTORY_SPIFFS_FILE_A  "/history_a.bin"
#define HISTORY_SPIFFS_FILE_B  "/history_b.bin"
#define HISTORY_SPIFFS_TMP_A   "/history_a.tmp"
#define HISTORY_SPIFFS_TMP_B   "/history_b.tmp"
#define HISTORY_NVS_META_NS    "hist5778m"

struct HistoryEntry {
  uint32_t dayKey;
  float endLiters;
  float consumedLiters;
  float refillLiters;
  uint8_t source;
};
enum HistorySource : uint8_t {
  HISTORY_MEASURED = 0,
  HISTORY_IMPORTED = 1,
  HISTORY_TEST = 2
};

struct __attribute__((packed)) CompactHistoryDay {
  uint16_t dayOffset;
  // Alle Literwerte als ganze Liter: kein 16-Bit-Clipping bei 655,35 L.
  uint16_t levelLiters;
  uint16_t consumptionLiters;
  uint16_t refillLiters;
  uint8_t source;
};

// Format V6.2.5: Fuellstand/Nachfuellung ganze Liter, Verbrauch noch x100.
struct __attribute__((packed)) V625CompactHistoryDay {
  uint16_t dayOffset;
  uint16_t levelLiters;
  uint16_t consumption100;
  uint16_t refillLiters;
  uint8_t source;
};

// Format V6.2.3/V6.2.4: Fuellstand schon in ganzen Litern, Nachfuellung noch x100.
struct __attribute__((packed)) PriorCompactHistoryDay {
  uint16_t dayOffset;
  uint16_t levelLiters;
  uint16_t consumption100;
  uint16_t refill100;
  uint8_t source;
};

struct __attribute__((packed)) LegacyCompactHistoryDay {
  uint16_t dayOffset;
  uint16_t level10;
  uint16_t consumption100;
  uint16_t refill100;
};
struct __attribute__((packed)) HistoryFileHeader {
  uint32_t magic, version, generation;
  uint16_t count, index;
  uint32_t lastDayKey;
  float dayStartLiters, lastLiters, pendingRefill;
  uint32_t baseDay, dataBytes, dataCrc;
};

static HistoryEntry historyStore[HISTORY_RAM_CACHE_DAYS];

// Gemeinsamer Arbeitsbereich fuer Langzeit-Historie.
// HistoryEntry und CompactHistoryDay werden nie gleichzeitig benoetigt.
union HistoryWorkspace {
  HistoryEntry entries[HISTORY_DAYS];
  CompactHistoryDay compact[HISTORY_DAYS];
};
static HistoryWorkspace historyWorkspace;

static uint16_t historyIndex=0, historyCount=0;
static uint32_t historyLastDayKey=0;
static float historyDayStartLiters=NAN, historyLastLiters=NAN, historyPendingRefill=0.0f;
#ifndef HISTORY_LIVE_SAVE_INTERVAL
#define HISTORY_LIVE_SAVE_INTERVAL 21600000UL  // 6 Stunden
#endif
static uint32_t historyLastLiveSaveMs = 0;
static bool historyStoreDirty=false, historySpiffsReady=false;


// ============================================================================
// ESP32 ARDUINO CORE 3.x
// ============================================================================

#if defined(ESP_ARDUINO_VERSION_MAJOR)
  #if ESP_ARDUINO_VERSION_MAJOR < 3
    #error "Fuellstandsmesser V7.3.8 requires ESP32 Arduino Core 3.x"
  #endif
#else
  #warning "ESP_ARDUINO_VERSION_MAJOR not available; verify ESP32 Arduino Core 3.x"
#endif

// ============================================================================
// VERSION / NAME
// ============================================================================

#define FW_VERSION             "V7.3.8"
#define DEVICE_NAME            "Fuellstandsmesser3"
#define DEVICE_HOSTNAME        "Fuellstandsmesser3"
#define DEFAULT_MQTT_TOPIC     "Fuellstandsmesser3"
#define OLD_DEVICE_NAME        "Fuellstansmesser3"

// ============================================================================
// SERIAL
// ============================================================================

#define SERIAL_BAUD            115200

// ============================================================================
// SENSOR TYPES
// ============================================================================

#define SENSOR_VL53L0X         0
#define SENSOR_VL53L1X         1
#define SENSOR_VL53L5CX        2
#define SENSOR_AUTO            255

// ============================================================================
// GEOMETRY
// ============================================================================

#define GEOMETRY_CYLINDER      0
#define GEOMETRY_BOX            1

// ============================================================================
// HISTORY / FILTER
// ============================================================================

#define LEGACY_CONSUMPTION_SAVE_INTERVAL 21600000UL  // Legacy: Historie wird in SPIFFS gespeichert  // 6 Stunden, seltenes NVS-Speichern
#define REFILL_EVENT_COUNT        8
#define REFILL_MAX_CANDIDATE_MS   14400000UL  // 4 Stunden
#define MAX_SAMPLES             64
#define MAX_MEDIAN_SAMPLES       7

// ============================================================================
// HARDWARE - ESP32-C3 Super Mini
// ============================================================================

// I2C / OLED / Sensoren
#define OLED_SDA_PIN             4
#define OLED_SCL_PIN             5
#define OLED_RESET_PIN          -1
#define OLED_ADDRESS           0x3C

// Standard-I2C-Adressen der aktuellen Hardware
#define VL_ADDRESS              0x29
#define BME_ADDRESS             0x76

// Status-LED und Display-/BOOT-Taste der aktuellen ESP32-C3-Hardware.
// Sensor-/I2C-Pinbelegung: SDA GPIO4, SCL GPIO5.
#define USER_LED_PIN             8
#define BOOT_PIN                 9  // ESP32-C3 Super Mini onboard BOOT-Taste

// OLED-Seitentaste separat persistent; bestehende Config-Struktur bleibt binär kompatibel.
uint8_t displayButtonPin = BOOT_PIN;

// Heap-Diagnose; Referenz aus Praxistests:
// kleinster bisher beobachteter Minimum-Heap ca. 20 KB.
static const uint32_t HEAP_WARN_BYTES = 20UL * 1024UL;
static const uint32_t HEAP_CRIT_BYTES = 19UL * 1024UL;

static const char* heapHealthText(uint32_t minHeap, uint32_t largestBlock) {
  if (minHeap < HEAP_CRIT_BYTES || largestBlock < (HEAP_CRIT_BYTES / 2UL)) return "KRITISCH";
  if (minHeap < HEAP_WARN_BYTES || largestBlock < HEAP_CRIT_BYTES) return "WARNUNG";
  return "OK";
}


// Reset-/Neustartdiagnose fuer Langzeittests.
esp_reset_reason_t bootResetReason = ESP_RST_UNKNOWN;

// Langzeitdiagnose und Recovery-Zähler
uint32_t persistentRestartCount = 0;
uint32_t lowestLargestBlockSinceBoot = 0xFFFFFFFFUL;
uint32_t lastMemoryDiagSampleMs = 0;
static const uint32_t MEMORY_DIAG_SAMPLE_INTERVAL = 10000UL; // 10 s
uint32_t wifiReconnectCountSinceBoot = 0;
uint32_t mqttReconnectCountSinceBoot = 0;
bool wifiEverConnectedSinceBoot = false;
bool mqttEverConnectedSinceBoot = false;
uint32_t sensorReinitCount = 0;
uint32_t lastValidMeasurementMs = 0;
uint32_t healthMeasurementIntervalMs = 20000UL;

static void updateMemoryDiagnostics() {
  const uint32_t now = millis();
  if (lastMemoryDiagSampleMs != 0 &&
      (uint32_t)(now - lastMemoryDiagSampleMs) < MEMORY_DIAG_SAMPLE_INTERVAL) return;

  lastMemoryDiagSampleMs = now;
  const uint32_t largest = ESP.getMaxAllocHeap();
  if (largest > 0 && largest < lowestLargestBlockSinceBoot) {
    lowestLargestBlockSinceBoot = largest;
  }
}

static const char* resetReasonText(esp_reset_reason_t reason) {
  switch (reason) {
    case ESP_RST_POWERON:   return "Power-On";
    case ESP_RST_EXT:       return "Externer Reset";
    case ESP_RST_SW:        return "Software-Reset";
    case ESP_RST_PANIC:     return "Panic/Exception";
    case ESP_RST_INT_WDT:   return "Interrupt-Watchdog";
    case ESP_RST_TASK_WDT:  return "Task-Watchdog";
    case ESP_RST_WDT:       return "Watchdog";
    case ESP_RST_DEEPSLEEP: return "Deep-Sleep Wakeup";
    case ESP_RST_BROWNOUT:  return "Brownout";
    case ESP_RST_SDIO:      return "SDIO";
    default:                return "Unbekannt";
  }
}

static bool resetReasonIsCritical(esp_reset_reason_t reason) {
  return reason == ESP_RST_PANIC ||
         reason == ESP_RST_INT_WDT ||
         reason == ESP_RST_TASK_WDT ||
         reason == ESP_RST_WDT ||
         reason == ESP_RST_BROWNOUT;
}


static const uint8_t HEALTH_OK = 0;
static const uint8_t HEALTH_WARN = 1;
static const uint8_t HEALTH_CRITICAL = 2;

static uint8_t overallHealthState() {
  const uint32_t minHeap = ESP.getMinFreeHeap();
  const uint32_t currentLargest = ESP.getMaxAllocHeap();
  const uint32_t largest = (lowestLargestBlockSinceBoot == 0xFFFFFFFFUL)
                         ? currentLargest
                         : lowestLargestBlockSinceBoot;

  const uint32_t now = millis();
  const uint32_t staleLimit = max<uint32_t>(30000UL, healthMeasurementIntervalMs * 5UL);
  const bool measurementStale =
      now > 60000UL &&
      (lastValidMeasurementMs == 0 ||
       (uint32_t)(now - lastValidMeasurementMs) > staleLimit);

  if (minHeap < HEAP_CRIT_BYTES ||
      largest < (HEAP_CRIT_BYTES / 2UL) ||
      resetReasonIsCritical(bootResetReason) ||
      measurementStale) {
    return HEALTH_CRITICAL;
  }

  if (minHeap < HEAP_WARN_BYTES ||
      largest < HEAP_CRIT_BYTES ||
      sensorReinitCount >= 3 ||
      wifiReconnectCountSinceBoot >= 5 ||
      mqttReconnectCountSinceBoot >= 10) {
    return HEALTH_WARN;
  }

  return HEALTH_OK;
}

static const char* overallHealthText() {
  switch (overallHealthState()) {
    case HEALTH_CRITICAL: return "KRITISCH";
    case HEALTH_WARN:     return "AUFFAELLIG";
    default:              return "OK";
  }
}

static String overallHealthDetails() {
  String s;
  if (resetReasonIsCritical(bootResetReason)) s += "kritische Reset-Ursache; ";
  if (ESP.getMinFreeHeap() < HEAP_WARN_BYTES) s += "Minimum-Heap niedrig; ";
  const uint32_t largest = (lowestLargestBlockSinceBoot == 0xFFFFFFFFUL)
                         ? ESP.getMaxAllocHeap()
                         : lowestLargestBlockSinceBoot;
  if (largest < HEAP_CRIT_BYTES) s += "Largest Block klein; ";
  const uint32_t now = millis();
  const uint32_t staleLimit = max<uint32_t>(30000UL, healthMeasurementIntervalMs * 5UL);
  if (now > 60000UL &&
      (lastValidMeasurementMs == 0 ||
       (uint32_t)(now - lastValidMeasurementMs) > staleLimit)) {
    s += "Messung veraltet/ausgefallen; ";
  }
  if (sensorReinitCount >= 3) s += "mehrere Sensor-Recoveries; ";
  if (wifiReconnectCountSinceBoot >= 5) s += "mehrere WLAN-Reconnects; ";
  if (mqttReconnectCountSinceBoot >= 10) s += "viele MQTT-Reconnects; ";
  if (s.length() == 0) s = "Keine Auffaelligkeiten erkannt";
  return s;
}


static void updatePersistentRestartCount() {
  preferences.begin("fuellstand", false);
  persistentRestartCount = preferences.getUInt("restartCount", 0);
  if (persistentRestartCount < 0xFFFFFFFFUL) {
    persistentRestartCount++;
  }
  preferences.putUInt("restartCount", persistentRestartCount);
  preferences.end();
}
// LOCALE / AP
// ============================================================================

#define AP_PASSWORD              ""

#define DNS_PORT                 53

// ============================================================================
// TIMING
// ============================================================================

#define WIFI_CONNECT_TIMEOUT     15000UL
#define WIFI_RECONNECT_INTERVAL  15000UL
#define WIFI_AP_STA_RETRY_INTERVAL 60000UL
#define MQTT_RECONNECT_INTERVAL  10000UL
#define MQTT_RECONNECT_MAX_INTERVAL 60000UL
#define MQTT_SOCKET_TIMEOUT_SEC      2U
#define MQTT_BUFFER_SIZE         4096U

#define SENSOR_FAIL_TIMEOUT      10000UL
#define SENSOR_REINIT_INTERVAL   30000UL
#define SENSOR_I2C_TIMEOUT_MS        20U

#define DEBUG_INTERVAL           600000UL  // 10 Minuten
#define NTP_RETRY_INTERVAL       30000UL

#define DEBUG_LEVEL_ERROR         1
#define DEBUG_LEVEL_WARN          2
#define DEBUG_LEVEL_NORMAL        3
#define DEBUG_LEVEL_DETAIL        4
#define DEBUG_LEVEL_SENSOR        5
#define DEFAULT_DEBUG_LEVEL       DEBUG_LEVEL_NORMAL

#define LED_BLINK_TIME            90UL

// ============================================================================
// CONFIG
// ============================================================================

struct Config {
  char wifiSSID[33];
  char wifiPassword[65];

  char mqttHost[65];
  uint16_t mqttPort;
  char mqttUser[33];
  char mqttPassword[65];
  char mqttTopic[65];
  char mqttAverageTopic[65];
  char mqttFuellhoeheTopic[65];

  char ntpServer[65];
  int32_t timezoneOffset;
  bool ntpEnabled;

  uint8_t sdaPin;
  uint8_t sclPin;

  uint8_t oledAddress;
  uint8_t vlAddress;
  uint8_t bmeAddress;

  uint8_t vlSensorType;

  uint32_t measurementInterval;
  uint32_t mqttInterval;

  uint8_t sampleCount;

  bool medianEnabled;
  uint8_t medianSamples;

  uint16_t minDistance;
  uint16_t maxDistance;
  uint16_t maxJump;

  int16_t sensorOffset;

  uint8_t geometry;

  float diameter;
  float tankLength;
  float tankWidth;
  float tankHeight;

  float emptyDistance;
  float fullDistance;

  float lowLevelPercent;
  float highLevelPercent;

  uint8_t debugLevel;

  bool oledEnabled;
  uint32_t oledPageInterval;
};

// ============================================================================
// NACHFUELL-EVENTS
// ============================================================================


struct RefillEvent {
  uint32_t timestamp;
  float liters;
};

RefillEvent refillEvents[REFILL_EVENT_COUNT];
uint8_t refillEventCount = 0;
float lastConfirmedRefillLiters = 0.0f;
uint32_t lastRefillTimestamp = 0;

// Wird von der Nachfuell-Erkennung gesetzt und anschliessend genau einmal
// von der zentralen Historienlogik verarbeitet.
float confirmedRefillPendingLiters = 0.0f;

// ============================================================================
// GLOBAL OBJECTS
// ============================================================================


WebServer *server = nullptr;
DNSServer dnsServer;

WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
bool mqttServerConfigured = false;

Adafruit_SSD1306 display(
  128,
  64,
  &Wire,
  OLED_RESET_PIN
);

Adafruit_BME280 bme;

Adafruit_VL53L0X vl53l0x;
Adafruit_VL53L1X vl53l1x;
SparkFun_VL53L5CX vl53l5cx;
VL53L5CX_ResultsData vl53l5cxData;
bool vl53l5cxInitialized = false;
uint8_t vl53l5cxValidZones = 0;

Config cfg;

// ============================================================================
// STATUS
// ============================================================================

bool oledOK = false;
bool vlOK = false;
bool bmeOK = false;

bool measurementValid = false;
bool updateRunning = false;

bool apMode = false;
bool wifiConnecting = false;
bool mdnsOK = false;
bool webServerConfigured = false;
bool webServerStarted = false;
uint32_t wifiConnectedSince = 0;
bool otaOK = false;
bool otaLastWebUpdateOK = false;
uint32_t otaLastWebUpdateBytes = 0;
bool ntpStarted = false;

uint32_t lastMeasurement = 0;
uint32_t lastMqtt = 0;
uint32_t lastOLED = 0;

uint32_t lastWifiAttempt = 0;
uint32_t lastApStaAttempt = 0;
uint32_t lastMqttAttempt = 0;
uint32_t mqttReconnectDelay = MQTT_RECONNECT_INTERVAL;
bool mqttEnabled = true;
String otaPassword;

uint32_t lastDebug = 0;
uint32_t lastNtpSync = 0;

uint32_t measurementCounter = 0;
uint32_t lastValidMeasurement = 0;

// ============================================================================
// MEASUREMENTS
// ============================================================================

uint16_t rawDistance = 0;

float rawDistanceFiltered = NAN;
float filteredDistance = NAN;
float medianDistance = NAN;

float tankPercent = NAN;
float tankLiters = NAN;
float tankHeightMm = NAN;

float temperature = NAN;
float humidity = NAN;
float pressure = NAN;
float dewPoint = NAN;

uint8_t sensorStatus = 0;
uint8_t sensorValidZones = 0;

// Sensorfehler / automatische Reinitialisierung
uint32_t sensorErrorCount = 0;
uint32_t lastSensorReinit = 0;
uint32_t sensorFailureSince = 0;
bool sensorReinitInProgress = false;

// ToF-Bereitschaft: dataReady()==false ist bei kontinuierlich messenden ToF-Sensoren
// zunächst nur "noch kein neuer Messwert" und kein echter Sensorfehler.
bool sensorDataPending = false;

// ============================================================================
// KALIBRIERUNG
// ============================================================================

// Berechnet aus einem bekannten Volumen die Flüssigkeitshöhe.
// Eingabe: Liter, Ausgabe: Flüssigkeitshöhe ab Tankboden in mm.
bool calculateHeightFromLiters(float liters, float &heightMm) {

  if (!isfinite(liters) || liters < 0.0f || cfg.tankHeight <= 0.0f) {
    return false;
  }

  double volumeM3 = (double)liters / 1000.0;
  double heightM = 0.0;

  if (cfg.geometry == GEOMETRY_CYLINDER) {
    double radiusM = ((double)cfg.diameter / 2.0) / 1000.0;
    double areaM2 = PI * radiusM * radiusM;
    if (areaM2 <= 0.0) return false;
    heightM = volumeM3 / areaM2;
  } else {
    double lengthM = (double)cfg.tankLength / 1000.0;
    double widthM = (double)cfg.tankWidth / 1000.0;
    double areaM2 = lengthM * widthM;
    if (areaM2 <= 0.0) return false;
    heightM = volumeM3 / areaM2;
  }

  heightMm = (float)(heightM * 1000.0);

  if (heightMm < 0.0f || heightMm > cfg.tankHeight + 1.0f) {
    return false;
  }

  heightMm = constrain(heightMm, 0.0f, cfg.tankHeight);
  return true;
}

// Aus bekanntem Volumen wird der theoretische Sensorabstand anhand
// der aktuellen Leer-/Voll-Kalibrierung berechnet.
bool calculateDistanceFromLiters(float liters, float &distanceMm, float &heightMm) {

  if (!calculateHeightFromLiters(liters, heightMm)) return false;

  float span = cfg.emptyDistance - cfg.fullDistance;
  if (span <= 0.0f || cfg.tankHeight <= 0.0f) return false;

  float fraction = heightMm / cfg.tankHeight;
  distanceMm = cfg.emptyDistance - fraction * span;
  return true;
}

// Einpunkt-Kalibrierung: Das aktuell gemessene Sensormaß wird einem
// bekannten Volumen zugeordnet. Die Messspanne (leer-voll) bleibt erhalten;
// dadurch wird primär der Nullpunkt korrigiert.
bool calibrateFromKnownLiters(float liters, float measuredDistance,
                              float &newEmpty, float &newFull, float &heightMm) {

  if (!isfinite(measuredDistance) || measuredDistance <= 0.0f) return false;

  if (!calculateHeightFromLiters(liters, heightMm)) return false;

  float span = cfg.emptyDistance - cfg.fullDistance;
  if (span <= 0.0f || cfg.tankHeight <= 0.0f) return false;

  float fraction = heightMm / cfg.tankHeight;
  newFull = measuredDistance - fraction * span;
  newEmpty = newFull + span;

  if (newFull <= 0.0f || newEmpty <= newFull) return false;

  return true;
}

// ============================================================================
// FILTER BUFFERS
// ============================================================================

float averageBuffer[MAX_SAMPLES];
uint8_t averageIndex = 0;
uint8_t averageCount = 0;

float medianBuffer[MAX_MEDIAN_SAMPLES];
uint8_t medianIndex = 0;
uint8_t medianCount = 0;

// ============================================================================
// OLED
// ============================================================================

uint8_t oledPage = 0;

// GPIO9 / BOOT-Taste steuert die OLED-Seiten.
// Seite 0 ist die permanente Hauptseite. Ein Tastendruck startet
// eine einmalige Anzeige der Zusatzseiten 1..4. Jede Zusatzseite
// bleibt cfg.oledPageInterval sichtbar; danach folgt die nächste.
// Anschließend kehrt das Display automatisch auf Seite 0 zurück.
bool oledButtonLastState = HIGH;
uint32_t oledPageShownAt = 0;
uint32_t oledButtonChangedAt = 0;
bool oledPageSequenceActive = false;
bool oledButtonHandled = false;

#define OLED_PAGE_COUNT 8

// V7.3.8: Fünffachklick auf die Display-Taste aktiviert kurz ein Easter Egg.
uint8_t oledFastClickCount = 0;
uint32_t oledFastClickWindowStart = 0;
bool oledEasterEggActive = false;
uint32_t oledEasterEggUntil = 0;


// ============================================================================
// LED STATE MACHINE
// ============================================================================

bool ledActive = false;
bool ledState = false;

uint16_t ledPulsesRemaining = 0;
uint16_t ledPulsesRequested = 0;

uint32_t ledTimer = 0;

// ============================================================================
// SYSTEM STATE
// ============================================================================

enum SystemState {
  STATE_BOOT,
  STATE_SETUP_AP,
  STATE_WIFI_CONNECTING,
  STATE_WIFI_CONNECTED,
  STATE_MQTT_CONNECTING,
  STATE_MQTT_CONNECTED,
  STATE_RUNNING,
  STATE_SENSOR_ERROR,
  STATE_WIFI_ERROR,
  STATE_MQTT_ERROR
};

SystemState systemState = STATE_BOOT;

// ============================================================================
// FORWARD DECLARATIONS
// ============================================================================

void saveConfig();
bool loadConfig();
void factoryReset();

void setupWebServer();
bool ensureWebServerObject();
void handleRoot();
void handleConfig();
void handleConfigSave();
void handleFactoryReset();
void handleStatus();
void handleStatusJson();
void handleApiPing();
void handleSystemStatusPage();
void handleHistory();
void handleHistoryPage();
void handleHistoryCsv();
void handleHistoryImport();
void handleHistoryImportUpload();
void handleConsumption();
void handleCalibration();
void handleCalibrationApply();
void handleReboot();
void handleNotFound();
void handleUpdatePage();
void handleGenerateTestHistory();
void handleClearConsumptionData();
void handleUpdateUpload();
void handleUpdateFinished();

void startAccessPoint();
void stopAccessPoint();
void startWiFi();
void wifiTask();

void mqttTask();
bool connectMQTT();
void mqttPublish();
void publishHomeAssistantDiscovery();

void setupOTA();
void otaTask();

void setupMDNS();
void setupNTP();
void ntpTask();

void measurementTask();
void initConsumptionTracking();
static void updateDailyHistory(float liters);
static void loadHistoryStore();
float consumptionToday();
float consumptionWeek();
float consumptionMonth();
float consumption365Days();
float consumptionAverage7Days();
float consumptionAverage30Days();
void addRefillEvent(float liters);
void saveRefillEvents();
void loadRefillEvents();
static uint32_t historyDayKeyToTimestamp(uint32_t dayKey);

// Kompatibilität: Verbrauchsseite wird von der bestehenden OLED-Seitenlogik dargestellt.


void oledTask();
void ledTask();

void initializeSensors();
void i2cScan();
void reinitializeSensors(bool force = false);
bool tofDevicePresent();
bool calculateHeightFromLiters(float liters, float &heightMm);
float calculateTankCapacityLiters();
bool calculateDistanceFromLiters(float liters, float &distanceMm, float &heightMm);
bool calibrateFromKnownLiters(float liters, float measuredDistance, float &newEmpty, float &newFull, float &heightMm);

String buildMqttJson();

static bool bmeValuesPlausible(float tempC, float humidityPct, float pressureHpa) {
  if (!isfinite(tempC) || !isfinite(humidityPct) || !isfinite(pressureHpa)) return false;
  if (tempC < -40.0f || tempC > 85.0f) return false;
  if (humidityPct < 0.0f || humidityPct > 100.0f) return false;
  if (pressureHpa < 300.0f || pressureHpa > 1100.0f) return false;
  return true;
}



 // Modulübergreifende Vorwärtsdeklarationen.
static bool debugEnabled(uint8_t requiredLevel);
String htmlEscape(const String &input);
String jsonEscape(const String &input);
String jsonFloat(float value, uint8_t decimals);
float calculateDewPointC(float temperatureC, float humidityPercent);
bool updateRefillDetection(float currentLiters);
static uint32_t historyDayKeyNow();
static void startHistoryDay(float liters);
static void closeHistoryDay(float endLiters);
static bool saveHistoryStore();
static bool saveGeneratedCompactHistory(const CompactHistoryDay* data,
                                        uint16_t count,
                                        uint32_t baseDay,
                                        uint32_t lastDayKey,
                                        float dayStartLiters,
                                        float lastLiters,
                                        float pendingRefill);

// Fachmodule werden als Unity-Einheit eingebunden.
#include "Config.h"
#include "DisplayAndSensors.h"
#include "MeasurementAndSensors.h"
#include "Mqtt.h"
#include "NetworkServices.h"
#include "WebServerModule.h"
#include "Runtime.h"
#include "Config.cpp"
#include "DisplayAndSensors.cpp"
#include "MeasurementAndSensors.cpp"
#include "Mqtt.cpp"
#include "NetworkServices.cpp"
#include "WebServer.cpp"
#include "Runtime.cpp"
