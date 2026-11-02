// [ru] AlashEncoder: режим прерываний. Повороты считаются сразу, даже если программа занята долгим делом (здесь delay(500)).
//      На Uno / Nano прерывания есть только у пинов 2 и 3 (CLK и DT). На ESP32 и ESP8266 подойдут любые пины.
//      Пины: Uno / Nano  CLK=2, DT=3, SW=4;  ESP32  CLK=25, DT=26, SW=27;  ESP8266  CLK=D5, DT=D6, SW=D7.
// [kk] AlashEncoder: үзіліс режимі. Бұрылыстар бағдарлама ұзақ іспен айналысса да (мұнда delay(500)) бірден санала береді.
//      Uno / Nano-да үзілістер тек 2 және 3 пиндерде (CLK және DT). ESP32 мен ESP8266-да кез келген пин жарайды.
//      Пиндер: Uno / Nano  CLK=2, DT=3, SW=4;  ESP32  CLK=25, DT=26, SW=27;  ESP8266  CLK=D5, DT=D6, SW=D7.
// [en] AlashEncoder: interrupt mode. Turns are counted at once, even if the program is busy with a long task (here delay(500)).
//      On Uno / Nano only pins 2 and 3 (CLK and DT) have interrupts. On ESP32 and ESP8266 any pins work.
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
  if (!knob.begin(true)) {                // true: по прерываниям
    Serial.println(knob.errorText());
    while (true) delay(1000);
  }
}

void loop() {
  delay(500);                             // пока программа "спит", повороты всё равно считаются
  Serial.println(knob.value());
}
