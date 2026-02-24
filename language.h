/**
 * ============================================================================
 * LANGUAGE.H - Multi-Language Support
 * ============================================================================
 */

#ifndef LANGUAGES_H
#define LANGUAGES_H
#include <stdint.h>
#include <avr/pgmspace.h>

constexpr uint8_t LANG_COUNT = 10;
constexpr uint8_t LANG_NAME_LEN = 10;
constexpr uint8_t LANG_PROMPT_LEN = 9;
constexpr uint8_t LANG_TANKTITLE_LEN = 16;
constexpr uint8_t LANG_AMOUNTTITLE_LEN = 17;
constexpr uint8_t LANG_MAINSCREEN_LEN = 17;
constexpr uint8_t LANG_NOTASK_LEN = 17;
constexpr uint8_t LANG_PUMPWORKING_LEN = 17;
constexpr uint8_t LANG_LOWTHRESHOLD_LEN = 18;
constexpr uint8_t LANG_HIGHTHRESHOLD_LEN = 18;
constexpr uint8_t LANG_INTERVALTITLE_LEN = 18;
constexpr uint8_t LANG_WATER_ERROR_LEN = 17;
constexpr uint8_t LANG_PUMP_STATUS_LEN = 14;

struct LangGeneral { char name[LANG_NAME_LEN]; char prompt[LANG_PROMPT_LEN]; };
struct LangTank {
  char volumeTitle[LANG_TANKTITLE_LEN];
  char amountTitle[LANG_AMOUNTTITLE_LEN];
  char intervalTitle[LANG_INTERVALTITLE_LEN];
  char lowThresholdTitle[LANG_LOWTHRESHOLD_LEN];
  char highThresholdTitle[LANG_HIGHTHRESHOLD_LEN];
};
struct LangStatus {
  char mainScreen[LANG_MAINSCREEN_LEN];
  char noTask[LANG_NOTASK_LEN];
  char pumpWorking[LANG_PUMPWORKING_LEN];
  char waterLevel[LANG_WATER_ERROR_LEN];
  char inletPumpOn[LANG_WATER_ERROR_LEN];
  char outletPumpOn[LANG_WATER_ERROR_LEN];
  char pumpsOk[LANG_PUMP_STATUS_LEN];
};
struct LangError {
  char critical[LANG_MAINSCREEN_LEN];
  char checkSensor[LANG_MAINSCREEN_LEN];
  char waterSensorError[LANG_WATER_ERROR_LEN];
  char sensorTimeout[LANG_WATER_ERROR_LEN];
  char commError[LANG_WATER_ERROR_LEN];
  char invalidData[LANG_WATER_ERROR_LEN];
  char pumpTimeout[LANG_WATER_ERROR_LEN];
  char unknownError[LANG_WATER_ERROR_LEN];
};
struct Language { LangGeneral general; LangTank tank; LangStatus status; LangError error; };

/**
 * Glyphs mapping (Octal Escapes):
 * \200: ą, \201: ć, \202: ę, \203: ł, \204: ó, \205: ś, \206: ź,
 * \207: Г, \210: Д, \211: Ж, \212: э \213: И, \214: Й, \215: Л, \216: П, \217: Я, \220: ы, \221: ъ, \222: ё, \223: Ч, \224: ь
 * \225: ü, \226: ä
 * \227: ç, \230: é, \231: â, \232: ñ, \233: í, \234: á
 * \235: à
 * \236: ê, \237: ã
 * \240: ı, \241: ş, \242: ğ, \243: ö, \244: İ
 * \245: Č, \246: š, \247: ž, \250: ý, \251: ú, \252: ě
 */

const Language LANGUAGES[LANG_COUNT] PROGMEM = {
  { // Polish (0)
    { "Polski   ", "J\202zyk   " },
    { "Poj. zbiornika", "Ilo\205\201 dawki w #", "Interwa\203 pompy #", "Dolna granica", "G\204rna granica " },
    { "Ekran g\203\204wny    ", "Brak zadania  ", "Pompa dzia\203a   ", "Poziom wody:  ", "Wlew ON         ", "Wylew ON       ", "Pompy: OK  " },
    { "B\203\200d krytyczny!", "Sprawd\206 sensor  ", "B\203\200d czujnika   ", "Timeout czuj.  ", "B\203\200d komunik.   ", "Nieprawidl. dane", "Timeout pompy   ", "Nieznany b\203\200d   " }
  },
  { // English (1)
    { "English  ", "Language" },
    { "Tank volume   ", "Dosage of #    ", "Pump interval # ", "Low Threshold", "High Threshold" },
    { "Main screen     ", "No task       ", "Pump running   ", "Water Level:  ", "Inlet Pump ON   ", "Outlet Pump ON ", "Pumps: OK  " },
    { "CRITICAL ERROR ", "CHECK SENSOR    ", "Sensor Error    ", "Sensor Timeout ", "Comm Error      ", "Invalid Data    ", "Pump Timeout    ", "Unknown Error   " }
  },
  { // Russian (2)
    { "Pycck\213\214  ", "\2173\220k    " },
    { "O6\221\222m 6aka    ", "\210o3\213poBka #    ", "\213HTepBa\215 #      ", "H\213\211H\213\214 \216opo\207 ", "BepxH\213\214 \216opo\207 " },
    { "\207\215aBH\220\214 \212kpaH   ", "HeT 3a\210a\223\213    ", "Hacoc pa6oTaeT ", "YpoBeH\224 Bo\210\220: ", "Bxo\210. Hacoc A   ", "B\220xo\210. Hacoc A ", "Hacoc\220 OK  " },
    { "KP\213T. OW\213BKA   ", "\216POBEP\224TE \210AT\223\213K", "OW\2136. \210aT\223\213ka   ", "Ta\214mayT ceHcopa", "OW\2136. cB\2173\213     ", "HeBep. \210aHH\220e   ", "Ta\214mayT Hacoca  ", "He\2133B. oW\2136ka   " }
  },
  { // German (3)
    { "Deutsch  ", "Sprache " },
    { "Volumen Tank  ", "Dosierung der #", "Intervall #     ", "Min. Schwelle", "Max. Schwelle " },
    { "Hauptmen\225       ", "Keine Aufgabe ", "Pumpe l\226uft    ", "Wasserstand:  ", "Zulauf Pumpe AN ", "Ablauf Pumpe AN", "Pumpen OK  " },
    { "KRIT. FEHLER   ", "SENSOR PR\225FEN   ", "Sensor Fehler   ", "Sensor Timeout ", "Komm. Fehler    ", "Ung\225ltige Daten ", "Pumpen Timeout  ", "Unbek. Fehler   " }
  },
  { // French (4)
    { "Fran\226ais ", "Langue  " },
    { "Vol. r\230servoir", "Dosage de la # ", "Intervalle #    ", "Seuil bas    ", "Seuil haut    " },
    { "\230cran principal ", "Aucune t\231che  ", "Pompe active   ", "Niveau d'eau: ", "Entr\230e pompe A  ", "Sortie pompe A ", "Pompes OK  " },
    { "ERREUR CRITIQ  ", "V\230RIF CAPTEUR   ", "Erreur capteur  ", "Timeout capteur", "Erreur communic.", "Donn\230es inval.  ", "Timeout pompe   ", "Erreur inconnue " }
  },
  { // Spanish (5)
    { "Espa\232ol  ", "Idioma  " },
    { "Vol. dep\204sito ", "Cantidad en #  ", "Intervalo #     ", "Umbral m\233nimo", "Umbral m\234ximo " },
    { "Pantalla princ. ", "Sin tarea     ", "Bomba activa   ", "Nivel de agua:", "Bomba entrada A ", "Bomba salida A ", "Bombas OK  " },
    { "ERROR CR\233TICO  ", "COMPROBE SENSOR ", "Error sensor    ", "Timeout sensor ", "Error de com.   ", "Datos inv\234lidos ", "Timeout bomba   ", "Error desconoc. " }
  },
  { // Italian (6)
    { "Italiano ", "Lingua  " },
    { "Volume serb.  ", "Quantit\235 in #  ", "Intervallo #    ", "Soglia minima", "Soglia massima" },
    { "Schermata princ.", "Nessun compito", "Pompa attiva   ", "Livello acqua:", "Pomp. ingresso A", "Pomp. uscita A ", "Pompe OK   " },
    { "ERRORE CRITICO ", "CONTROLLARE SEN ", "Errore sensore  ", "Timeout sensore", "Errore comunic. ", "Dati non validi ", "Timeout pompa   ", "Errore sconosciu" }
  },
  { // Portuguese (7)
    { "Portugu\236s", "Idioma  " },
    { "Volume tanque ", "Quantidade #   ", "Intervalo #     ", "Limite m\233nimo", "Limite m\234ximo " },
    { "Ecr\237 principal  ", "Sem tarefa    ", "Bomba ativa    ", "N\233vel de \234gua:", "Bomba entrada A ", "Bomba sa\233da A  ", "Bombas OK  " },
    { "ERRO CR\233TICO   ", "VERIFI. SENSOR  ", "Erro sensor     ", "Timeout sensor ", "Erro de com.    ", "Dados inv\234lidos ", "Timeout bomba   ", "Erro desconoc.  " }
  },
  { // Turkish (8)
    { "T\225rk\226e   ", "Dil     " },
    { "Tank Hacmi    ", "Miktar #       ", "Aral\240k #        ", "Alt E\241i\242i    ", "\225st E\241i\242i     " },
    { "Ana Ekran       ", "G\243rev yok     ", "Pompa \226al\240\241\240yor", "Su Seviyesi:  ", "Giri\241 Pomp. A   ", "\226\240k\240\241 Pomp. A  ", "Pompalar OK" },
    { "KR\244T\244K HATA    ", "SENS\243R KONTROL  ", "Sens\243r Hatas\240   ", "Sens\243r Timeout ", "Hata.\244leti\241im   ", "Ge\226ersiz Veri   ", "Pompa Timeout   ", "Bilinmeyen Hata " }
  },
  { // Czech (9)
    { "\245e\246tina  ", "Jazyk   " },
    { "Objem n\234dr\247e  ", "Mno\247stv\233 v #   ", "Interval #      ", "Spodn\233 pr\234h  ", "Horn\233 pr\234h    " },
    { "Hlavn\233 obrazov. ", "\247\234dn\250 \251kol    ", "\245erpadlo b\252\247\233  ", "Hladina wody: ", "Vstupn\233 \245erp. A ", "V\250stupn\233 \245er. A", "\245erpadla OK" },
    { "KRIT. CHYBA    ", "ZKONTROL \245IDLO  ", "Chyba \245idla     ", "Timeout \245idla  ", "Chyba komunik.  ", "Neplatn\234 data   ", "Timeout \245erpadla", "Nezn\234m\234 chyba   " }
  }
};

Language readLanguage(uint8_t idx, Language *dest);
void readLanguageField(uint8_t idx, uint8_t offset, char* dest, uint8_t len);

#endif
