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

// Number of supported languages
constexpr uint8_t LANG_COUNT = 10;

// Buffer sizes for language strings (in bytes)
constexpr uint8_t LANG_NAME_LEN = 16;              ///< Language name
constexpr uint8_t LANG_PROMPT_LEN = 12;            ///< Input prompt
constexpr uint8_t LANG_TANKTITLE_LEN = 16;         ///< Tank volume title
constexpr uint8_t LANG_AMOUNTTITLE_LEN = 16;       ///< Pump amount title
constexpr uint8_t LANG_DURATIONTITLE_LEN = 16;     ///< Pump duration title
constexpr uint8_t LANG_MAINSCREEN_LEN = 16;        ///< Main screen label
constexpr uint8_t LANG_NOTASK_LEN = 16;            ///< No task message
constexpr uint8_t LANG_PUMPWORKING_LEN = 16;       ///< Pump active message

// Number of characters to display on 16-character LCD screen
constexpr uint8_t LANG_NAME_VISIBLE = 8;
constexpr uint8_t LANG_PROMPT_VISIBLE = 7;
constexpr uint8_t LANG_TANKTITLE_VISIBLE = 16;
constexpr uint8_t LANG_AMOUNTTITLE_VISIBLE = 16;
constexpr uint8_t LANG_DURATIONTITLE_VISIBLE = 16;
constexpr uint8_t LANG_PUMPWORKING_VISIBLE = 16;
constexpr uint8_t LANG_MAINSCREEN_VISIBLE = 16;
constexpr uint8_t LANG_NOTASK_VISIBLE = 16;

// Number of pumps in the system
constexpr uint8_t PUMP_COUNT = 5;

/**
 * Language structure storing all UI strings for a language
 */
struct Language {
  char langName[LANG_NAME_LEN];              ///< Display name of language
  char langPrompt[LANG_PROMPT_LEN];          ///< Prompt for language selection
  char tankVolumeTitle[LANG_TANKTITLE_LEN];  ///< Title for tank volume input
  char amountTitle[LANG_AMOUNTTITLE_LEN];
  char durationTitle[LANG_DURATIONTITLE_LEN];  ///< Title for pump duration input
  char mainScreen[LANG_MAINSCREEN_LEN];      ///< Main screen heading
  char noTask[LANG_NOTASK_LEN];              ///< Message when no task active
  char pumpWorking[LANG_PUMPWORKING_LEN];    ///< Message when pump is running
};

// Language definitions stored in program memory (PROGMEM) to save RAM
const Language LANGUAGES[LANG_COUNT] PROGMEM = {
  { "Polski   ", "J\000zyk  ", "Poj. zbiornika  ", "Ilosc płynu w #", "Czas pracy pompy #", "Ekran g\001\002wny    ", "Brak zadania    " },
  { "English  ", "Lang   ", "Tank volume     ", "Liquid in #    ", "Pump duration #", "Main screen     ", "No task         " },
  { "Pycckий  ", "Языk   ", "O6ъём бака      ", "Жидкость в #   ", "Время работы помпы #", "Главный экран   ", "Нет задачи      " },
  { "Deutsch  ", "Sprache", "Tankvolumen     ", "Flüssigk. in # ", "Pumpendauer #", "Hauptbildschirm ", "Keine Aufgabe   " },
  { "Français ", "Langue ", "Vol. réservoir  ", "Liquide dans # ", "Durée pompe #", "Écran principal ", "Aucune tâche    " },
  { "Español  ", "Idioma ", "Vol. tanque     ", "Líquido en #   ", "Duración bomba #", "Pantalla princ. ", "Sin tarea       " },
  { "Italiano ", "Lingua ", "Vol. serbatoio  ", "Liquido in #   ", "Schermo prin.   ", "Nessun compito  " },
  { "Português", "Idioma ", "Vol. do tanque  ", "Líquido em #   ", "Duração bomba #", "Tela principal  ", "Sem tarefa      " },
  { "Türkçe   ", "Dil    ", "Hacim           ", "Sıvı içinde #  ", "Pompa süresi #", "Ana ekran       ", "Görev yok       " },
  { "Čeština  ", "Jazyk  ", "Objem nádrže    ", "Kapalina v #   ", "Délka činnosti #", "Hlavní obrazovka", "Žádný úkol      " }
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
