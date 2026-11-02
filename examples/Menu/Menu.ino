// [ru] AlashEncoder: меню на энкодере. Крутите, чтобы выбрать пункт, нажмите, чтобы выбрать.
//      Пункты по кругу (после последнего снова первый).
//      Пины: Uno / Nano  CLK=2, DT=3, SW=4;  ESP32  CLK=25, DT=26, SW=27;  ESP8266  CLK=D5, DT=D6, SW=D7.
// [kk] AlashEncoder: энкодердегі мәзір. Тармақты таңдау үшін бұраңыз, таңдау үшін басыңыз.
//      Тармақтар шеңбер бойымен жүреді (соңғыдан кейін қайтадан біріншісі).
//      Пиндер: Uno / Nano  CLK=2, DT=3, SW=4;  ESP32  CLK=25, DT=26, SW=27;  ESP8266  CLK=D5, DT=D6, SW=D7.
// [en] AlashEncoder: a menu on the encoder. Turn to choose an item, press to select.
//      The items wrap around (after the last one comes the first again).
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
const char *items[] = {"Свет", "Вентилятор", "Полив", "Настройки"};
const int COUNT = sizeof(items) / sizeof(items[0]);

void show() {
  Serial.print("-> ");
  Serial.println(items[knob.value()]);
}

void setup() {
  Serial.begin(115200);
  knob.begin();
  knob.setLimits(0, COUNT - 1, true);     // true: по кругу
  show();
}

void loop() {
  if (knob.changed()) show();
  if (knob.clicked()) {
    Serial.print("Выбрано: ");
    Serial.println(items[knob.value()]);
  }
}
