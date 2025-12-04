#ifndef LANGUAGES_H
#define LANGUAGES_H
#include <stdint.h>

// Store fixed-size strings in the struct so the whole struct can live in PROGMEM
// and be copied out with memcpy_P. Array bounds must be compile-time
// constant expressions, so use constexpr integer constants rather than
// an array of const values.
constexpr uint8_t LANG_COUNT = 10;
constexpr uint8_t LANG_NAME_LEN = 9;
constexpr uint8_t LANG_PROMPT_LEN = 11;
constexpr uint8_t LANG_TANKTITLE_LEN = 14;

struct Language {
  char name[LANG_NAME_LEN];
  char prompt[LANG_PROMPT_LEN];
  char tankTitle[LANG_TANKTITLE_LEN];
};

// the array itself (defined in language.cpp)
extern const Language LANGUAGES[LANG_COUNT];

// index of currently selected language (global)
extern uint8_t languageIndex;

// function to read one language
Language readLanguage(uint8_t idx);


#endif
