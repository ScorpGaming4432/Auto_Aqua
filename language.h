/**
 * ============================================================================
 * LANGUAGE.H - Multi-Language Support (UTF-8 Enabled)
 * ============================================================================
 */

#ifndef LANGUAGES_H
#define LANGUAGES_H
#include <stdint.h>
#include <avr/pgmspace.h>

constexpr uint8_t LANG_COUNT = 10;

// Internal buffer sizes (enough for multi-byte UTF-8)
constexpr uint8_t LANG_NAME_LEN = 20;
constexpr uint8_t LANG_PROMPT_LEN = 20;
constexpr uint8_t LANG_TANKTITLE_LEN = 32;
constexpr uint8_t LANG_AMOUNTTITLE_LEN = 32;
constexpr uint8_t LANG_MAINSCREEN_LEN = 32;
constexpr uint8_t LANG_NOTASK_LEN = 32;
constexpr uint8_t LANG_PUMPWORKING_LEN = 32;
constexpr uint8_t LANG_LOWTHRESHOLD_LEN = 32;
constexpr uint8_t LANG_HIGHTHRESHOLD_LEN = 32;
constexpr uint8_t LANG_INTERVALTITLE_LEN = 32;
constexpr uint8_t LANG_WATER_ERROR_LEN = 32;
constexpr uint8_t LANG_PUMP_STATUS_LEN = 32;

struct LangGeneral {
  char name[LANG_NAME_LEN];
  char prompt[LANG_PROMPT_LEN];
};
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
  char inletPumpOn[LANG_PUMP_STATUS_LEN];
  char outletPumpOn[LANG_PUMP_STATUS_LEN];
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
struct Language {
  LangGeneral general;
  LangTank tank;
  LangStatus status;
  LangError error;
};

const Language LANGUAGES[LANG_COUNT] PROGMEM = {
  { // Polish (0)
    { "Polski   ", "Język   " },
    { "Poj. zbiornika", "Ilość dawki w #", "Interwał pompy #", "Dolna granica", "Górna granica " },
    { "Ekran główny    ", "Brak zadania  ", "Pompa działa   ", "Poziom wody:  ", "Wlew ON         ", "Wylew ON       ", "Pompy: OK  " },
    { "Błąd krytyczny!", "Sprawdź sensor  ", "Błąd czujnika   ", "Timeout czuj.  ", "Błąd komunik.   ", "Nieprawidl. dane", "Timeout pompy   ", "Nieznany błąd   " } },
  { // English (1)
    { "English  ", "Language" },
    { "Tank volume   ", "Dosage of #    ", "Pump interval # ", "Low Threshold", "High Threshold" },
    { "Main screen     ", "No task       ", "Pump running   ", "Water Level:  ", "Inlet Pump ON   ", "Outlet Pump ON ", "Pumps: OK  " },
    { "CRITICAL ERROR ", "CHECK SENSOR    ", "Sensor Error    ", "Sensor Timeout ", "Comm Error      ", "Invalid Data    ", "Pump Timeout    ", "Unknown Error   " } },
  { // Russian (2)
    { "Русский  ", "Язык    " },
    { "Объём бака    ", "Дозировка #    ", "Интервал #      ", "Нижний порог ", "Верхний порог " },
    { "Главный экран   ", "Нет задачи    ", "Насос работает ", "Уровень воды: ", "Вход. насос A   ", "Выход. насос A ", "Насосы OK  " },
    { "КРИТ. ОШИБКА   ", "ПРОВЕРЬТЕ ДАТЧИК", "Ошиб. датчика   ", "Таймаут сенсора", "Ошиб. связи     ", "Невер. данные   ", "Таймаут насоса  ", "Неизв. ошибка   " } },
  { // German (3)
    { "Deutsch  ", "Sprache " },
    { "Volumen Tank  ", "Dosierung der #", "Intervall #     ", "Min. Schwelle", "Max. Schwelle " },
    { "Hauptmenü       ", "Keine Aufgabe ", "Pumpe läuft    ", "Wasserstand:  ", "Zulauf Pumpe AN ", "Ablauf Pumpe AN", "Pumpen OK  " },
    { "KRIT. FEHLER   ", "SENSOR PRÜFEN   ", "Sensor Fehler   ", "Sensor Timeout ", "Komm. Fehler    ", "Ungültige Daten ", "Pumpen Timeout  ", "Unbek. Fehler   " } },
  { // French (4)
    { "Français ", "Langue  " },
    { "Vol. réservoir", "Dosage de la # ", "Intervalle #    ", "Seuil bas    ", "Seuil haut    " },
    { "Écran principal ", "Aucune tâche  ", "Pompe active   ", "Niveau d'eau: ", "Entrée pompe A  ", "Sortie pompe A ", "Pompes OK  " },
    { "ERREUR CRITIQ  ", "VÉRIF CAPTEUR   ", "Erreur capteur  ", "Timeout capteur", "Erreur communic.", "Données inval.  ", "Timeout pompe   ", "Erreur inconnue " } },
  { // Spanish (5)
    { "Español  ", "Idioma  " },
    { "Vol. depósito ", "Cantidad en #  ", "Intervalo #     ", "Umbral mínimo", "Umbral máximo " },
    { "Pantalla princ. ", "Sin tarea     ", "Bomba activa   ", "Nivel de agua:", "Bomba entrada A ", "Bomba salida A ", "Bombas OK  " },
    { "ERROR CRÍTICO  ", "COMPROBE SENSOR ", "Error sensor    ", "Timeout sensor ", "Error de com.   ", "Datos inválidos ", "Timeout bomba   ", "Error desconoc. " } },
  { // Italian (6)
    { "Italiano ", "Lingua  " },
    { "Volume serb.  ", "Quantità in #  ", "Intervallo #    ", "Soglia minima", "Soglia massima" },
    { "Schermata princ.", "Nessun compito", "Pompa activa   ", "Livello acqua:", "Pomp. ingresso A", "Pomp. uscita A ", "Pompe OK   " },
    { "ERRORE CRITICO ", "CONTROLLARE SEN ", "Errore sensore  ", "Timeout sensore", "Errore comunic. ", "Dati non validi ", "Timeout pompa   ", "Errore sconosciu" } },
  { // Portuguese (7)
    { "Português", "Idioma  " },
    { "Volume tanque ", "Quantidade #   ", "Intervalo #     ", "Limite mínimo", "Limite máximo " },
    { "Ecrã principal  ", "Sem tarefa    ", "Bomba ativa    ", "Nível de água:", "Bomba entrada A ", "Bomba saída A  ", "Bombas OK  " },
    { "ERRO CRÍTICO   ", "VERIFI. SENSOR  ", "Erro sensor     ", "Timeout sensor ", "Erro de com.    ", "Dados inválidos ", "Timeout bomba   ", "Erro desconoc.  " } },
  { // Turkish (8)
    { "Türkçe   ", "Dil     " },
    { "Tank Hacmi    ", "Miktar #       ", "Aralık #        ", "Alt Eşiği    ", "Üst Eşiği     " },
    { "Ana Ekran       ", "Görev yok     ", "Pompa çalışıyor", "Su Seviyesi:  ", "Giriş Pomp. A   ", "Çıkış Pomp. A  ", "Pompalar OK" },
    { "KRİTİK HATA    ", "SENSÖR KONTROL  ", "Sensör Hatası   ", "Sensör Timeout ", "Hata.İletişim   ", "Geçersiz Veri   ", "Pompa Timeout   ", "Bilinmeyen Hata " } },
  { // Czech (9)
    { "Čeština  ", "Jazyk   " },
    { "Objem nádrže  ", "Množství v #   ", "Interval #      ", "Spodní práh  ", "Horní práh    " },
    { "Hlavní obrazov. ", "Žádный úkol    ", "Čerpadlo běží  ", "Hladina wody: ", "Вступní čerp. A ", "Výstupní čer. A", "Čerpadla OK" },
    { "KRIT. CHYBA    ", "ZKONTROL ČIDLO  ", "Chyba čidla     ", "Timeout čidla  ", "Chyba komunik.  ", "Neplatná data   ", "Timeout čerpadla", "Neznámá chyba   " } }
};

Language readLanguage(uint8_t idx, Language* dest);
void readLanguageField(uint8_t idx, uint8_t offset, char* dest, uint8_t len);

#endif
