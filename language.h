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
 *   2: Русский (Russian)
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
#include <string.h>
#include <avr/pgmspace.h>

// Number of supported languages
constexpr uint8_t LANG_COUNT = 10;

// Max characters per string (visible characters). All strings must fit in
// MAX_LANG_CHARS characters; buffers allocate one extra byte for the null.
constexpr uint8_t MAX_LANG_CHARS = 16;          // MAX 16 characters per field
constexpr uint8_t LANG_NAME_LEN = 9;            ///< Language name
constexpr uint8_t LANG_PROMPT_LEN = 7;          ///< Input prompt
constexpr uint8_t LANG_TANKTITLE_LEN = 14;      ///< Tank volume title
constexpr uint8_t LANG_AMOUNTTITLE_LEN = 15;    ///< Pump amount title
constexpr uint8_t LANG_MAINSCREEN_LEN = 15;     ///< Main screen label
constexpr uint8_t LANG_NOTASK_LEN = 15;         ///< No task message
constexpr uint8_t LANG_PUMPWORKING_LEN = 15;    ///< Pump active message
constexpr uint8_t LANG_LOWTHRESHOLD_LEN = 16;   ///< Water threshold message
constexpr uint8_t LANG_HIGHTHRESHOLD_LEN = 16;  ///< High water threshold message

/**
 * Language structure storing all UI strings for a language
 */
struct Language {
  char langName[LANG_NAME_LEN];                     ///< Display name of language
  char langPrompt[LANG_PROMPT_LEN];                 ///< Prompt for language selection
  char tankVolumeTitle[LANG_TANKTITLE_LEN];         ///< Title for tank volume input
  char amountTitle[LANG_AMOUNTTITLE_LEN];           ///< Title for pump amount input
  char mainScreen[LANG_MAINSCREEN_LEN];             ///< Main screen heading
  char noTask[LANG_NOTASK_LEN];                     ///< Message when no task active
  char pumpWorking[LANG_PUMPWORKING_LEN];           ///< Message when pump is running
  char lowThresholdTitle[LANG_LOWTHRESHOLD_LEN];    ///< Message for low water threshold
  char highThresholdTitle[LANG_HIGHTHRESHOLD_LEN];  ///< Message for high water threshold
  char intervalTitle[16];
};

// Language definitions stored in program memory (PROGMEM) to save RAM
const Language LANGUAGES[LANG_COUNT] PROGMEM = {
  // langName,    prompt,    tank title,       amount title,      main screen,       no task,          pump working,     low thresh,           high thresh
  { "Polski   ", "Jezyk  ", "Poj. zbiornika", "Ilosc plynu w #", "Ekran glowny   ", "Brak zadania  ", "Pompa dziala   ", "Dolna granica", "Gorna granica ", "Interwal pompy #" },
  { "English  ", "Lang   ", "Tank volume   ", "Liquid in #    ", "Main screen    ", "No task       ", "Pump running   ", "Low Threshold", "High Threshold", "Pump interval # " },
  { "Russkii  ", "Yazyk  ", "Obyem baka    ", "Zhidkost v #   ", "Glavnyi ekran  ", "Net zadachi   ", "Pompa rabotaet ", "             ", "              ", "Pump interval # " },
  { "Deutsch  ", "Sprache", "Tankvolumen   ", "Fluessig in #  ", "Hauptbildschir ", "Keine Aufgabe ", "Pumpe laeuft   ", "             ", "              ", "Pump interval # " },
  { "Francais ", "Langue ", "Vol reservoir ", "Liquide en #   ", "Ecran principal", "Aucune tache  ", "Pompe en marche", "             ", "              ", "Pump interval # " },
  { "Espanol  ", "Idioma ", "Vol tanque    ", "Liquido en #   ", "Pantalla princ ", "Sin tarea     ", "Bomba activa   ", "             ", "              ", "Pump interval # " },
  { "Italiano ", "Lingua ", "Vol serbatoio ", "Liquido in #   ", "Schermo princ  ", "Nessun compito", "Pompa attiva   ", "             ", "              ", "Pump interval # " },
  { "Portugues", "Idioma ", "Vol tanque    ", "Liquido em #   ", "Tela principal ", "Sem tarefa    ", "Bomba ativa    ", "             ", "              ", "Pump interval # " },
  { "Turkce   ", "Dil    ", "Hacim         ", "Sivi icinde #  ", "Ana ekran      ", "Gorev yok     ", "Pompa calisiyor", "             ", "              ", "Pump interval # " },
  { "Cestina  ", "Jazyk  ", "Objem nadrze  ", "Kapalina v #   ", "Hlavni obraz   ", "Zadny ukol    ", "Pumpa bezi     ", "             ", "              ", "Pump interval # " }
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
