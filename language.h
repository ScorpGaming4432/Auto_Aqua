/**
 * ============================================================================
 * LANGUAGE.H - Multi-Language Support
 * ============================================================================
 *
 * Provides support for 10 different languages in the aquarium system interface.
 * All strings are stored in PROGMEM to minimize RAM usage.
 *
 * Supported Languages (0-9):
 *   0: Polski (Polish)
 *   1: English
 *   2: Руccкий (Russian)
 *   3: Deutsch (German)
 *   4: Français (French)
 *   5: Español (Spanish)
 *   6: Italiano (Italian)
 *   7: Português (Portuguese)
 *   8: Türkçe (Turkish)
 *   9: Čeština (Czech)
 */

#ifndef LANGUAGES_H
#define LANGUAGES_H
#include <stdint.h>
#include <avr/pgmspace.h>

// Number of supported languages
constexpr uint8_t LANG_COUNT = 10;

// Max characters per string (visible characters). All strings must fit in
// MAX_LANG_CHARS characters; buffers allocate one extra byte for the null.
constexpr uint8_t MAX_LANG_CHARS = 16;          // MAX 16 characters per field
constexpr uint8_t LANG_NAME_LEN = 8;            ///< Language name
constexpr uint8_t LANG_PROMPT_LEN = 7;          ///< Input prompt
constexpr uint8_t LANG_TANKTITLE_LEN = 14;      ///< Tank volume title
constexpr uint8_t LANG_AMOUNTTITLE_LEN = 15;    ///< Pump amount title
constexpr uint8_t LANG_MAINSCREEN_LEN = 15;     ///< Main screen label
constexpr uint8_t LANG_NOTASK_LEN = 15;         ///< No task message
constexpr uint8_t LANG_PUMPWORKING_LEN = 15;    ///< Pump active message
constexpr uint8_t LANG_LOWTHRESHOLD_LEN = 16;   ///< Water threshold message
constexpr uint8_t LANG_HIGHTHRESHOLD_LEN = 16;  ///< High water threshold message
constexpr uint8_t LANG_INTERVALTITLE_LEN = 16;  ///< Pump interval title

// Lengths for newly added strings
constexpr uint8_t LANG_WATER_ERROR_LEN = 16;   ///< Water-related error messages
constexpr uint8_t LANG_PUMP_STATUS_LEN = 14;   ///< Pump status messages

/**
 * Language structure storing all UI strings for a language
 */
struct Language {
  char langName[LANG_NAME_LEN + 1];                     ///< Display name of language
  char langPrompt[LANG_PROMPT_LEN + 1];                 ///< Prompt for language selection
  char tankVolumeTitle[LANG_TANKTITLE_LEN + 1];         ///< Title for tank volume input
  char amountTitle[LANG_AMOUNTTITLE_LEN + 1];           ///< Title for pump amount input
  char mainScreen[LANG_MAINSCREEN_LEN + 1];             ///< Main screen heading
  char noTask[LANG_NOTASK_LEN + 1];                     ///< Message when no task active
  char pumpWorking[LANG_PUMPWORKING_LEN + 1];           ///< Message when pump is running
  char lowThresholdTitle[LANG_LOWTHRESHOLD_LEN + 1];    ///< Message for low water threshold
  char highThresholdTitle[LANG_HIGHTHRESHOLD_LEN + 1];  ///< Message for high water threshold
  char intervalTitle[LANG_INTERVALTITLE_LEN + 1];       ///< Title for pump interval input
  char criticalError[LANG_MAINSCREEN_LEN + 1];          ///< Message for critical error
  char checkSensor[LANG_MAINSCREEN_LEN + 1];            ///< Message to check sensor

  // New strings for water management
  char waterSensorError[LANG_WATER_ERROR_LEN + 1];  ///< General water sensor error
  char sensorTimeout[LANG_WATER_ERROR_LEN + 1];     ///< Sensor timeout message
  char commError[LANG_WATER_ERROR_LEN + 1];         ///< Communication error
  char invalidData[LANG_WATER_ERROR_LEN + 1];       ///< Invalid data message
  char pumpTimeout[LANG_WATER_ERROR_LEN + 1];       ///< Pump timeout message
  char unknownError[LANG_WATER_ERROR_LEN + 1];      ///< Unknown error message
  char waterLevel[LANG_WATER_ERROR_LEN + 1];        ///< Water level label
  char inletPumpOn[LANG_PUMP_STATUS_LEN + 1];       ///< Inlet pump active
  char outletPumpOn[LANG_PUMP_STATUS_LEN + 1];      ///< Outlet pump active
  char pumpsOk[LANG_PUMP_STATUS_LEN + 1];           ///< Pumps OK status
};

// Language definitions stored in program memory (PROGMEM) to save RAM
const Language LANGUAGES[LANG_COUNT] PROGMEM = {
  // langName,    prompt,     tank title,       amount title,      main screen,        no task,          pump working,      low thresh,        high thresh,       interval title,     critical error,    check sensor,       waterSensorError,   sensorTimeout,     commError,          invalidData,        pumpTimeout,        unknownError,       waterLevel,       inletPumpOn,        outletPumpOn,      pumpsOK
  { "Polski   ", "Język   ", "Poj. zbiornika", "Ilość dawki w #", "Ekran główny    ", "Brak zadania  ", "Pompa działa   ", "Dolna granica", "Górna granica ", "Interwał pompy #", "Błąd krytyczny!", "Sprawdź sensor  ", "Błąd czujnika   ", "Timeout czuj.  ", "Błąd komunik.   ", "Nieprawidl. dane", "Timeout pompy   ", "Nieznany błąd   ", "Poziom wody:  ", "Wlew ON         ", "Wylew ON       ", "Pompy: OK  " },
  { "English  ", "Language", "Tank volume   ", "Dosage of #    ", "Main screen     ", "No task       ", "Pump running   ", "Low Threshold", "High Threshold", "Pump interval # ", "CRITICAL ERROR ", "CHECK SENSOR    ", "Sensor Error    ", "Sensor Timeout ", "Comm Error      ", "Invalid Data    ", "Pump Timeout    ", "Unknown Error   ", "Water Level:  ", "Inlet Pump ON   ", "Outlet Pump ON ", "Pumps: OK  " },
  { "Pycckий  ", "Языk    ", "O6ъёm 6aka    ", "Дозировка #    ", "Глaвный эkpaн   ", "Heт зaдaчи    ", "Hacoc pa6oтaeт ", "Hижний пopoг ", "Bepxний пopoг ", "Интepвaл #      ", "KPИT. OШИБKA   ", "ПPOBEPЬTE ДATЧИK", "Oши6. дaтчиka   ", "Taйmayт ceнcopa", "Oши6. cвязи     ", "Heвep. дaнныe   ", "Taйmayт нacoca  ", "Heизв. oши6ka   ", "Ypoвeнь вoды: ", "Bxoд. нacoc A   ", "Bыxoд. нacoc A ", "Hacocы OK  " },
  { "Deutsch  ", "Sprache ", "Volumen Tank  ", "Dosierung der #", "Hauptmenü       ", "Keine Aufgabe ", "Pumpe läuft    ", "Min. Schwelle", "Max. Schwelle ", "Intervall #     ", "KRIT. FEHLER   ", "SENSOR PRÜFEN   ", "Sensor Fehler   ", "Sensor Timeout ", "Komm. Fehler    ", "Ungültige Daten ", "Pumpen Timeout  ", "Unbek. Fehler   ", "Wasserstand:  ", "Zulauf Pumpe AN ", "Ablauf Pumpe AN", "Pumpen OK  " },
  { "Français ", "Langue  ", "Vol. réservoir", "Dosage de la # ", "Écran principal ", "Aucune tâche  ", "Pompe active   ", "Seuil bas    ", "Seuil haut    ", "Intervalle #    ", "ERREUR CRITIQ  ", "VÉRIF CAPTEUR   ", "Erreur capteur  ", "Timeout capteur", "Erreur communic.", "Données inval.  ", "Timeout pompe   ", "Erreur inconnue ", "Niveau d'eau: ", "Entrée pompe A  ", "Sortie pompe A ", "Pompes OK  " },
  { "Español  ", "Idioma  ", "Vol. depósito ", "Cantidad en #  ", "Pantalla princ. ", "Sin tarea     ", "Bomba activa   ", "Umbral mínimo", "Umbral máximo ", "Intervalo #     ", "ERROR CRÍTICO  ", "COMPROBE SENSOR ", "Error sensor    ", "Timeout sensor ", "Error de com.   ", "Datos inválidos ", "Timeout bomba   ", "Error desconoc. ", "Nivel de agua:", "Bomba entrada A ", "Bomba salida A ", "Bombas OK  " },
  { "Italiano ", "Lingua  ", "Volume serb.  ", "Quantità in #  ", "Schermata princ.", "Nessun compito", "Pompa attiva   ", "Soglia minima", "Soglia massima", "Intervallo #    ", "ERRORE CRITICO ", "CONTROLLARE SEN ", "Errore sensore  ", "Timeout sensore", "Errore comunic. ", "Dati non validi ", "Timeout pompa   ", "Errore sconosciu", "Livello acqua:", "Pomp. ingresso A", "Pomp. uscita A ", "Pompe OK   " },
  { "Português", "Idioma  ", "Volume tanque ", "Quantidade #   ", "Ecrã principal  ", "Sem tarefa    ", "Bomba ativa    ", "Limite mínimo", "Limite máximo ", "Intervalo #     ", "ERRO CRÍTICO   ", "VERIFI. SENSOR  ", "Erro sensor     ", "Timeout sensor ", "Erro de com.    ", "Dados inválidos ", "Timeout bomba   ", "Erro desconoc.  ", "Nível de água:", "Bomba entrada A ", "Bomba saída A  ", "Bombas OK  " },
  { "Türkçe   ", "Dil     ", "Tank Hacmi    ", "Miktar #       ", "Ana Ekran       ", "Görev yok     ", "Pompa çalışıyor", "Alt Eşiği    ", "Üst Eşiği     ", "Aralık #        ", "KRİTİK HATA    ", "SENSÖR KONTROL  ", "Sensör Hatası   ", "Sensör Timeout ", "Hata.İletişim   ", "Geçersiz Veri   ", "Pompa Timeout   ", "Bilinmeyen Hata ", "Su Seviyesi:  ", "Giriş Pomp. A   ", "Çıkış Pomp. A  ", "Pompalar OK" },
  { "Čeština  ", "Jazyk   ", "Objem nádrže  ", "Množství v #   ", "Hlavní obrazov. ", "Žádný úkol    ", "Čerpadlo běží  ", "Spodní práh  ", "Horní práh    ", "Interval #      ", "KRIT. CHYBA    ", "ZKONTROL ČIDLO  ", "Chyba čidla     ", "Timeout čidla  ", "Chyba komunik.  ", "Neplatná data   ", "Timeout čerpadla", "Neznámá chyba   ", "Hladina vody: ", "Vstupní čerp. A ", "Výstupní čer. A", "Čerpadla OK" }
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