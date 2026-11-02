// [ru] AlashEncoder: самый простой пример. Крутите ручку: значение растёт вправо и падает влево.
//
//      Подключение модуля KY-040: + к 3.3 В (Uno: 5 В), GND к GND, CLK и DT на пины A и B, SW на пин кнопки.
//      Пины: Uno / Nano  CLK=2, DT=3, SW=4;  ESP32  CLK=25, DT=26, SW=27;  ESP8266  CLK=D5, DT=D6, SW=D7.
// [kk] AlashEncoder: ең қарапайым мысал. Тұтқаны бұраңыз: оңға бұрағанда мән өседі, солға бұрағанда азаяды.
//
//      KY-040 модулін қосу: + 3.3 В-қа (Uno: 5 В), GND GND-ға, CLK және DT A және B пиндерге, SW батырма пініне.
//      Пиндер: Uno / Nano  CLK=2, DT=3, SW=4;  ESP32  CLK=25, DT=26, SW=27;  ESP8266  CLK=D5, DT=D6, SW=D7.
// [en] AlashEncoder: the simplest example. Turn the knob: the value grows to the right and falls to the left.
//
//      KY-040 module wiring: + to 3.3 V (Uno: 5 V), GND to GND, CLK and DT to pins A and B, SW to the button pin.
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
  knob.begin();
}

void loop() {
  if (knob.changed()) {                   // true один раз после каждого щелчка
    Serial.print("Значение: ");
    Serial.println(knob.value());
  }
  if (knob.clicked()) Serial.println("Нажатие на ручку");
}
