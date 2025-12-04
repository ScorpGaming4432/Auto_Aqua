#include "language.h"

const Language LANGUAGES[LANG_COUNT] = {
  { "Polski   ", "Num=J\000zyk  ", "Obj. Zbiornika" },
  { "English  ", "Num=Lang.  ", "Tank volume   " },
  { "P\210CCK\204\205  ", "Num=\255\203\216K   ", "O\200\215\353M \200AKA    " }, 
  { "Deutsch  ", "Num=Sprache", "Tankvolumen   " },
  { "Fran\347ais ", "Num=Langue ", "Vol. r\351servoir" },
  { "Espa\361ol  ", "Num=Idioma ", "Vol. tanque   " },
  { "Italiano ", "Num=Lingua ", "Vol. serbatoio" },
  { "Portugu\352s", "Num=Idioma ", "Vol. do tanque" },
  { "T\374rk\347e   ", "Num=Dil    ", "Hacim         " },
  { "\000e\001tina  ", "Num=Jazyk  ", "Objem n\341dr\002e  " }
};

// function to fetch these
Language readLanguage(uint8_t idx) {
  Language CurrentLanguage = LANGUAGES[idx];
  return CurrentLanguage;
}