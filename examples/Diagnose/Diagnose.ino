// [ru] AlashEncoder: энкодер не работает или считает не так? Скетч покажет уровни на пинах и подскажет настройки.
//      Пины: Uno / Nano  CLK=2, DT=3, SW=4;  ESP32  CLK=25, DT=26, SW=27;  ESP8266  CLK=D5, DT=D6, SW=D7.
// [kk] AlashEncoder: энкодер жұмыс істемей ме немесе қате санай ма? Скетч пиндердегі деңгейлерді көрсетеді және баптауларды ұсынады.
//      Пиндер: Uno / Nano  CLK=2, DT=3, SW=4;  ESP32  CLK=25, DT=26, SW=27;  ESP8266  CLK=D5, DT=D6, SW=D7.
// [en] AlashEncoder: the encoder does not work or counts wrongly? The sketch shows the pin levels and suggests settings.
//      Pins: Uno / Nano  CLK=2, DT=3, SW=4;  ESP32  CLK=25, DT=26, SW=27;  ESP8266  CLK=D5, DT=D6, SW=D7.
#if defined(ESP32)
const uint8_t PIN_A = 25, PIN_B = 26, PIN_SW = 27;
#elif defined(ESP8266)
const uint8_t PIN_A = 14, PIN_B = 12, PIN_SW = 13;
#else
const uint8_t PIN_A = 2, PIN_B = 3, PIN_SW = 4;
#endif
#include <AlashEncoder.h>

AlashEncoder knob(PIN_A, PIN_B, PIN_SW);

void setup() {
  Serial.begin(115200);
  delay(500);
  knob.setLanguage(AlashEncoder::LANG_RU);   // LANG_KK: қазақша, LANG_EN: English
  knob.begin();
}

void loop() {
  knob.diagnose(Serial);
  Serial.println();
  delay(1000);
}
