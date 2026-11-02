// AlashEncoder: поворотный энкодер (KY-040 и подобные) с кнопкой: щелчки, значение с границами, ускорение, клик и долгое нажатие.
// Без зависимостей. Работает опросом в loop() или по прерываниям (begin(true)).
// MIT License. Alash Electronics, https://github.com/Alash-electronics/AlashEncoder
#ifndef ALASH_ENCODER_H
#define ALASH_ENCODER_H

#include <Arduino.h>

// Куда помещать обработчики прерываний (быстрая память на ESP)
#if defined(ESP32)
#define ALASH_ENC_ISR IRAM_ATTR
#elif defined(ESP8266)
#define ALASH_ENC_ISR ICACHE_RAM_ATTR
#else
#define ALASH_ENC_ISR
#endif

class AlashEncoder {
 public:
  enum Error : uint8_t {
    ERR_OK = 0,
    ERR_NO_INTERRUPT,  // на этом пине нет прерываний (Uno / Nano: только 2 и 3)
    ERR_NO_PULLUP      // у этого пина ESP32 (GPIO34-39) нет внутренней подтяжки: нужны внешние резисторы и режим ENC_EXTERNAL
  };
  // Подтяжка пина: ENC_PULLUP (внутри платы, по умолчанию), ENC_EXTERNAL (резисторы на модуле или свои: у KY-040 они уже есть), ENC_PULLDOWN (к земле: только ESP32)
  enum PinMode : uint8_t { ENC_PULLUP = 0, ENC_EXTERNAL, ENC_PULLDOWN };
  enum Language : uint8_t { LANG_RU = 0, LANG_KK = 1, LANG_EN = 2 };

  // pinA, pinB: два выхода энкодера (CLK и DT). pinButton: кнопка (SW), необязательно (-1 = нет).
  AlashEncoder(uint8_t pinA, uint8_t pinB, int8_t pinButton = -1);
  ~AlashEncoder();

  // useInterrupts = true: считать повороты по прерываниям (не пропустит быстрое вращение, пока вы заняты другим).
  bool begin(bool useInterrupts = false);

  // ---- Вращение ----------------------------------------------------------------
  int32_t value();                         // текущее значение: щелчки умножены на шаг, ограничены границами
  void setValue(int32_t v);
  int16_t delta();                         // сколько щелчков прошло с прошлого вызова (вправо +, влево -)
  bool turnedRight();                      // был щелчок вправо (true один раз)
  bool turnedLeft();                       // был щелчок влево
  bool changed();                          // значение изменилось с прошлого вызова

  void setStep(int16_t step) { _step = step; }                 // на сколько меняется значение за щелчок (по умолчанию 1)
  void setLimits(int32_t minValue, int32_t maxValue, bool wrap = false);   // границы; wrap = по кругу
  void setAcceleration(bool on) { _accel = on; }               // быстрое вращение меняет значение сильнее
  void setStepsPerClick(uint8_t n) { _perClick = (n == 1 || n == 2) ? n : 4; }   // 4 (по умолчанию), 2 или 1 импульс на щелчок
  void reverse(bool on = true) { _reversed = on; }             // поменять направление (вместо перепайки A и B)
  void setPinMode(PinMode mode) { _pm = mode; }                // подтяжка пинов A и B (до begin())

  // ---- Кнопка (если указан pinButton) ----------------------------------------------
  bool isPressed();                        // кнопка нажата сейчас
  bool pressed();                          // только что нажали (один раз)
  bool clicked();                          // короткий клик (если при нажатии крутили, клика нет)
  bool longPressed();                      // держат дольше setLongPressMs (один раз)
  void setLongPressMs(uint16_t ms) { _longMs = ms; }           // по умолчанию 800
  void setButtonMode(PinMode mode, bool activeLow = true) { _bm = mode; _bLow = activeLow; }   // подтяжка кнопки и уровень нажатой (до begin()): активный LOW для кнопки к GND, HIGH для кнопки к питанию

  // ---- Диагностика -----------------------------------------------------------------
  void setLanguage(Language lang) { _lang = lang; }
  Error error() const { return _err; }
  const __FlashStringHelper *errorText();
  void diagnose(Print &out);               // уровни на пинах и подсказки

  void update() { poll(); }                // опрос вручную (в режиме опроса достаточно вызывать любой метод)
  void ALASH_ENC_ISR isr() { pollRotation(); }   // для обработчиков прерываний (кнопка там не опрашивается)

 private:
  uint8_t _a, _b;
  int8_t _btn;
  PinMode _pm = ENC_PULLUP, _bm = ENC_PULLUP;
  bool _bLow = true;
  bool _started = false, _irq = false;
  Error _err = ERR_OK;
  Language _lang = LANG_RU;

  // вращение
  uint8_t _prev = 0;                       // прошлое состояние A,B (2 бита)
  int8_t _acc = 0;                         // накопленные четверти шага
  int8_t _lastDir = 0;
  uint8_t _perClick = 4;
  bool _reversed = false, _accel = false;
  volatile int16_t _pending = 0;           // щелчки, ещё не учтённые в value (пишет опрос или прерывание)
  volatile uint16_t _interval = 1000;      // мс между последними щелчками (для ускорения)
  volatile uint32_t _lastClickAt = 0;
  int32_t _value = 0, _min = INT32_MIN, _max = INT32_MAX;
  bool _wrap = false;
  int16_t _step = 1;
  int16_t _deltaAcc = 0;                   // для delta()
  int16_t _rightFlag = 0, _leftFlag = 0;
  bool _changedFlag = false;
  bool _turnedSincePress = false;

  // кнопка
  bool _bRaw = false, _bDown = false;
  uint32_t _bRawAt = 0, _bDownAt = 0;
  uint16_t _longMs = 800;
  bool _fPress = false, _fClick = false, _fLong = false, _longFired = false;

  void poll();
  void ALASH_ENC_ISR pollRotation();
  void pollButton();
  void sync();
  uint8_t ALASH_ENC_ISR readAB();
};

#endif  // ALASH_ENCODER_H
