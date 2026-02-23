/**
 * ============================================================================
 * LANGUAGE.H - Multi-Language Support
 * ============================================================================
 *
 * Provides support for 10 different languages in the aquarium system interface.
 * All strings are stored in PROGMEM to minimize RAM usage.
 */

#ifndef LANGUAGES_H
#define LANGUAGES_H
#include <stdint.h>
#include <avr/pgmspace.h>

// Number of supported languages
constexpr uint8_t LANG_COUNT = 10;

// Max characters per string (visible characters). All strings must fit in
// MAX_LANG_CHARS characters; buffers allocate one extra byte for the null.
constexpr uint8_t MAX_LANG_CHARS = 16 + 2;      // MAX 16 characters per field
constexpr uint8_t LANG_NAME_LEN = 10;           ///< Language name
constexpr uint8_t LANG_PROMPT_LEN = 9;          ///< Input prompt
constexpr uint8_t LANG_TANKTITLE_LEN = 16;      ///< Tank volume title
constexpr uint8_t LANG_AMOUNTTITLE_LEN = 17;    ///< Pump amount title
constexpr uint8_t LANG_MAINSCREEN_LEN = 17;     ///< Main screen label
constexpr uint8_t LANG_NOTASK_LEN = 17;         ///< No task message
constexpr uint8_t LANG_PUMPWORKING_LEN = 17;    ///< Pump active message
constexpr uint8_t LANG_LOWTHRESHOLD_LEN = 18;   ///< Water threshold message
constexpr uint8_t LANG_HIGHTHRESHOLD_LEN = 18;  ///< High water threshold message
constexpr uint8_t LANG_INTERVALTITLE_LEN = 18;  ///< Pump interval title

// Lengths for newly added strings
constexpr uint8_t LANG_WATER_ERROR_LEN = 16;  ///< Water-related error messages
constexpr uint8_t LANG_PUMP_STATUS_LEN = 14;  ///< Pump status messages

/**
 * Language sub-structures for organizational discipline (max 10 fields each)
 */
struct LangGeneral {
  char name[LANG_NAME_LEN];                     ///< Display name of language
  char prompt[LANG_PROMPT_LEN];                 ///< Prompt for language selection
};

struct LangTank {
  char volumeTitle[LANG_TANKTITLE_LEN];         ///< Title for tank volume input
  char amountTitle[LANG_AMOUNTTITLE_LEN];       ///< Title for pump amount input
  char intervalTitle[LANG_INTERVALTITLE_LEN];   ///< Title for pump interval input
  char lowThresholdTitle[LANG_LOWTHRESHOLD_LEN]; ///< Message for low water threshold
  char highThresholdTitle[LANG_HIGHTHRESHOLD_LEN]; ///< Message for high water threshold
};

struct LangStatus {
  char mainScreen[LANG_MAINSCREEN_LEN];         ///< Main screen heading
  char noTask[LANG_NOTASK_LEN];                 ///< Message when no task active
  char pumpWorking[LANG_PUMPWORKING_LEN];       ///< Message when pump is running
  char waterLevel[LANG_WATER_ERROR_LEN];        ///< Water level label
  char inletPumpOn[LANG_PUMP_STATUS_LEN];       ///< Inlet pump active
  char outletPumpOn[LANG_PUMP_STATUS_LEN];      ///< Outlet pump active
  char pumpsOk[LANG_PUMP_STATUS_LEN];           ///< Pumps OK status
};

struct LangError {
  char critical[LANG_MAINSCREEN_LEN];           ///< Message for critical error
  char checkSensor[LANG_MAINSCREEN_LEN];         ///< Message to check sensor
  char waterSensorError[LANG_WATER_ERROR_LEN];  ///< General water sensor error
  char sensorTimeout[LANG_WATER_ERROR_LEN];      ///< Sensor timeout message
  char commError[LANG_WATER_ERROR_LEN];         ///< Communication error
  char invalidData[LANG_WATER_ERROR_LEN];        ///< Invalid data message
  char pumpTimeout[LANG_WATER_ERROR_LEN];        ///< Pump timeout message
  char unknownError[LANG_WATER_ERROR_LEN];       ///< Unknown error message
};

/**
 * Language structure storing all UI strings for a language
 */
struct Language {
  LangGeneral general;
  LangTank tank;
  LangStatus status;
  LangError error;
};

// Language definitions stored in program memory (PROGMEM) to save RAM
const Language LANGUAGES[LANG_COUNT] PROGMEM = {
  {
    { "Polski   ", "Język   " },
    { "Poj. zbiornika", "Ilość dawki w #", "Interwał pompy #", "Dolna granica", "Górna granica " },
    { "Ekran główny    ", "Brak zadania  ", "Pompa działa   ", "Poziom wody:  ", "Wlew ON         ", "Wylew ON       ", "Pompy: OK  " },
    { "Błąd krytyczny!", "Sprawdź sensor  ", "Błąd czujnika   ", "Timeout czuj.  ", "Błąd komunik.   ", "Nieprawidl. dane", "Timeout pompy   ", "Nieznany błąd   " }
  },
  {
    { "English  ", "Language" },
    { "Tank volume   ", "Dosage of #    ", "Pump interval # ", "Low Threshold", "High Threshold" },
    { "Main screen     ", "No task       ", "Pump running   ", "Water Level:  ", "Inlet Pump ON   ", "Outlet Pump ON ", "Pumps: OK  " },
    { "CRITICAL ERROR ", "CHECK SENSOR    ", "Sensor Error    ", "Sensor Timeout ", "Comm Error      ", "Invalid Data    ", "Pump Timeout    ", "Unknown Error   " }
  },
  {
    { "Pycckий  ", "Языk    " },
    { "O6ъёm 6aka    ", "Дозировка #    ", "Интepвaл #      ", "Hижний пopoг ", "Bepxний пopoг " },
    { "Глaвный эkpaн   ", "Heт зaдaчи    ", "Hacoc pa6oтaeт ", "Ypoвeнь вoды: ", "Bxoд. нacoc A   ", "Bыxoд. нacoc A ", "Hacocы OK  " },
    { "KPИT. OШИBKA   ", "ПPOBEPЬTE ДATЧИK", "Oши6. дaтчиka   ", "Taйmayт ceнcopa", "Oши6. cвязи     ", "Heвep. дaнныe   ", "Taйmayт нacoca  ", "Heизв. oши6ka   " }
  },
  {
    { "Deutsch  ", "Sprache " },
    { "Volumen Tank  ", "Dosierung der #", "Intervall #     ", "Min. Schwelle", "Max. Schwelle " },
    { "Hauptmenü       ", "Keine Aufgabe ", "Pumpe läuft    ", "Wasserstand:  ", "Zulauf Pumpe AN ", "Ablauf Pumpe AN", "Pumpen OK  " },
    { "KRIT. FEHLER   ", "SENSOR PRÜFEN   ", "Sensor Fehler   ", "Sensor Timeout ", "Komm. Fehler    ", "Ungültige Daten ", "Pumpen Timeout  ", "Unbek. Fehler   " }
  },
  {
    { "Français ", "Langue  " },
    { "Vol. réservoir", "Dosage de la # ", "Intervalle #    ", "Seuil bas    ", "Seuil haut    " },
    { "Écran principal ", "Aucune tâche  ", "Pompe active   ", "Niveau d'eau: ", "Entrée pompe A  ", "Sortie pompe A ", "Pompes OK  " },
    { "ERREUR CRITIQ  ", "VÉRIF CAPTEUR   ", "Erreur capteur  ", "Timeout capteur", "Erreur communic.", "Données inval.  ", "Timeout pompe   ", "Erreur inconnue " }
  },
  {
    { "Español  ", "Idioma  " },
    { "Vol. depósito ", "Cantidad en #  ", "Intervalo #     ", "Umbral mínimo", "Umbral máximo " },
    { "Pantalla princ. ", "Sin tarea     ", "Bomba activa   ", "Nivel de agua:", "Bomba entrada A ", "Bomba salida A ", "Bombas OK  " },
    { "ERROR CRÍTICO  ", "COMPROBE SENSOR ", "Error sensor    ", "Timeout sensor ", "Error de com.   ", "Datos inválidos ", "Timeout bomba   ", "Error desconoc. " }
  },
  {
    { "Italiano ", "Lingua  " },
    { "Volume serb.  ", "Quantità in #  ", "Intervallo #    ", "Soglia minima", "Soglia massima" },
    { "Schermata princ.", "Nessun compito", "Pompa attiva   ", "Livello acqua:", "Pomp. ingresso A", "Pomp. uscita A ", "Pompe OK   " },
    { "ERRORE CRITICO ", "CONTROLLARE SEN ", "Errore sensore  ", "Timeout sensore", "Errore comunic. ", "Dati non validi ", "Timeout pompa   ", "Errore sconosciu" }
  },
  {
    { "Português", "Idioma  " },
    { "Volume tanque ", "Quantidade #   ", "Intervalo #     ", "Limite mínimo", "Limite máximo " },
    { "Ecrã principal  ", "Sem tarefa    ", "Bomba ativa    ", "Nível de água:", "Bomba entrada A ", "Bomba saída A  ", "Bombas OK  " },
    { "ERRO CRÍTICO   ", "VERIFI. SENSOR  ", "Erro sensor     ", "Timeout sensor ", "Erro de com.    ", "Dados inválidos ", "Timeout bomba   ", "Erro desconoc.  " }
  },
  {
    { "Türkçe   ", "Dil     " },
    { "Tank Hacmi    ", "Miktar #       ", "Aralık #        ", "Alt Eşiği    ", "Üst Eşiği     " },
    { "Ana Ekran       ", "Görev yok     ", "Pompa çalışıyor", "Su Seviyesi:  ", "Giriş Pomp. A   ", "Çıkış Pomp. A  ", "Pompalar OK" },
    { "KRİTİK HATA    ", "SENSÖR KONTROL  ", "Sensör Hatası   ", "Sensör Timeout ", "Hata.İletişim   ", "Geçersiz Veri   ", "Pompa Timeout   ", "Bilinmeyen Hata " }
  },
  {
    { "Čeština  ", "Jazyk   " },
    { "Objem nádrže  ", "Množství v #   ", "Interval #      ", "Spodní práh  ", "Horní práh    " },
    { "Hlavní obrazov. ", "Žádný úkol    ", "Čerpadlo běží  ", "Hladina wody: ", "Vstupní čerp. A ", "Výstupní čer. A", "Čerpadla OK" },
    { "KRIT. CHYBA    ", "ZKONTROL ČIDLO  ", "Chyba čidla     ", "Timeout čidla  ", "Chyba komunik.  ", "Neplatná data   ", "Timeout čerpadla", "Neznámá chyba   " }
  }
};

/**
 * Read a complete language structure from PROGMEM
 * @param idx Language index (0-9)
 * @return Language structure for the selected language
 */
Language readLanguage(uint8_t idx);

/**
 * Read a single field from a language structure in PROGMEM
 * More memory-efficient than reading entire structure
 * @param idx Language index (0-9)
 * @param offset Byte offset within Language structure
 * @param dest Destination buffer for the string
 * @param len Number of bytes to copy
 */
void readLanguageField(uint8_t idx, uint8_t offset, char* dest, uint8_t len);

#endif
