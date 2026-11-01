// AlashEncoder: регулятор громкости или яркости 0-100 %. Быстрое вращение меняет значение сильнее (ускорение),
// нажатие на ручку включает и выключает звук (значение запоминается).
// Пины: Uno / Nano  CLK=2, DT=3, SW=4;  ESP32  CLK=25, DT=26, SW=27;  ESP8266  CLK=D5, DT=D6, SW=D7.
#if defined(ESP32)
const uint8_t PIN_A = 25, PIN_B = 26, PIN_SW = 27;
#elif defined(ESP8266)
const uint8_t PIN_A = 14, PIN_B = 12, PIN_SW = 13;
#else
const uint8_t PIN_A = 2, PIN_B = 3, PIN_SW = 4;
#endif
#include <AlashEncoder.h>

AlashEncoder knob(PIN_A, PIN_B, PIN_SW);
bool muted = false;
int32_t beforeMute = 0;

void setup() {
  Serial.begin(115200);
  knob.begin();
  knob.setLimits(0, 100);                 // от 0 до 100, дальше не уходит
  knob.setStep(2);                        // один щелчок = 2 %
  knob.setAcceleration(true);             // быстро крутите: до 10 раз быстрее
  knob.setValue(30);
}

void loop() {
  if (knob.clicked()) {
    muted = !muted;
    if (muted) { beforeMute = knob.value(); knob.setValue(0); }
    else knob.setValue(beforeMute);
    Serial.println(muted ? "Звук выключен" : "Звук включён");
  }
  if (knob.changed()) {
    Serial.print("Громкость: ");
    Serial.print(knob.value());
    Serial.println(" %");
  }
}
