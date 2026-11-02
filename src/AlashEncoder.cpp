// MIT License. Alash Electronics.
#include "AlashEncoder.h"

#define T(ru, kk, en) (_lang == LANG_KK ? F(kk) : _lang == LANG_EN ? F(en) : F(ru))

// Прерывания: функция без аргументов не знает, чей это энкодер, поэтому на каждый энкодер своя «воронка» (до 4 энкодеров).
static AlashEncoder *s_inst[4] = {nullptr, nullptr, nullptr, nullptr};
static void ALASH_ENC_ISR isr0() { if (s_inst[0]) s_inst[0]->isr(); }
static void ALASH_ENC_ISR isr1() { if (s_inst[1]) s_inst[1]->isr(); }
static void ALASH_ENC_ISR isr2() { if (s_inst[2]) s_inst[2]->isr(); }
static void ALASH_ENC_ISR isr3() { if (s_inst[3]) s_inst[3]->isr(); }
static void (*const s_isr[4])() = {isr0, isr1, isr2, isr3};

AlashEncoder::AlashEncoder(uint8_t pinA, uint8_t pinB, int8_t pinButton) : _a(pinA), _b(pinB), _btn(pinButton) {}

AlashEncoder::~AlashEncoder() {
  if (!_irq) return;
  detachInterrupt(digitalPinToInterrupt(_a));
  detachInterrupt(digitalPinToInterrupt(_b));
  for (uint8_t i = 0; i < 4; i++)
    if (s_inst[i] == this) s_inst[i] = nullptr;   // место свободно для другого энкодера
}

// Режим пина по выбору: подтяжка платы, внешняя (просто вход) или к земле (там, где она есть)
static void setupPin(uint8_t pin, AlashEncoder::PinMode m) {
  if (m == AlashEncoder::ENC_PULLUP) pinMode(pin, INPUT_PULLUP);
#if defined(INPUT_PULLDOWN)
  else if (m == AlashEncoder::ENC_PULLDOWN) pinMode(pin, INPUT_PULLDOWN);
#endif
  else pinMode(pin, INPUT);
}

bool AlashEncoder::begin(bool useInterrupts) {
  _err = ERR_OK;
  setupPin(_a, _pm);
  setupPin(_b, _pm);
  if (_btn >= 0) setupPin(_btn, _bm);
#if defined(ESP32)
  if ((_pm == ENC_PULLUP && ((_a >= 34 && _a <= 39) || (_b >= 34 && _b <= 39))) || (_btn >= 0 && _bm == ENC_PULLUP && _btn >= 34 && _btn <= 39))
    _err = ERR_NO_PULLUP;                                  // эти пины только на вход, без подтяжки: пока работает, но уровни «в воздухе»
#endif
  _prev = readAB();
  _acc = 0;
  _lastClickAt = millis();
  if (_btn >= 0) {
    _bRaw = _bDown = (digitalRead(_btn) == (_bLow ? LOW : HIGH));
    _bRawAt = _bDownAt = millis();
    _longFired = _bDown;                   // зажата при старте: без событий
  }
  _started = true;

  if (useInterrupts) {
    if (digitalPinToInterrupt(_a) == NOT_AN_INTERRUPT || digitalPinToInterrupt(_b) == NOT_AN_INTERRUPT) {
      _err = ERR_NO_INTERRUPT;
      return false;
    }
    for (uint8_t i = 0; i < 4; i++) {
      if (s_inst[i] == nullptr || s_inst[i] == this) {
        s_inst[i] = this;
        attachInterrupt(digitalPinToInterrupt(_a), s_isr[i], CHANGE);
        attachInterrupt(digitalPinToInterrupt(_b), s_isr[i], CHANGE);
        _irq = true;
        break;
      }
    }
    if (!_irq) _err = ERR_NO_INTERRUPT;    // все четыре места заняты
  }
  return _err == ERR_OK;
}

uint8_t AlashEncoder::readAB() { return (digitalRead(_a) ? 2 : 0) | (digitalRead(_b) ? 1 : 0); }

// Код Грея: при вращении состояние (A,B) идёт по кругу 00 -> 01 -> 11 -> 10. Любой другой переход это помеха или пропуск.
void AlashEncoder::pollRotation() {
  static const int8_t table[16] = {0, 1, -1, 0, -1, 0, 0, 1, 1, 0, 0, -1, 0, -1, 1, 0};
  uint8_t cur = readAB();
  if (cur == _prev) return;
  int8_t d = table[(_prev << 2) | cur];
  if (d == 0) d = _lastDir * 2;            // пропущено промежуточное состояние (быстрое вращение): идём в ту же сторону
  else _lastDir = d;
  _prev = cur;
  if (_reversed) d = -d;
  _acc += d;

  while (_acc >= (int8_t)_perClick) {
    _acc -= _perClick;
    uint32_t now = millis();
    _interval = (uint16_t)min((uint32_t)60000, (uint32_t)(now - _lastClickAt));
    _lastClickAt = now;
    _pending++;
  }
  while (_acc <= -(int8_t)_perClick) {
    _acc += _perClick;
    uint32_t now = millis();
    _interval = (uint16_t)min((uint32_t)60000, (uint32_t)(now - _lastClickAt));
    _lastClickAt = now;
    _pending--;
  }
  // В положении покоя копим с нуля: потерянный импульс не должен смещать счёт навсегда
  if ((_perClick == 4 && cur == 3) || (_perClick == 2 && (cur == 0 || cur == 3))) _acc = 0;
}

void AlashEncoder::pollButton() {
  if (_btn < 0) return;
  uint32_t now = millis();
  bool raw = (digitalRead(_btn) == (_bLow ? LOW : HIGH));
  if (raw != _bRaw) {
    _bRaw = raw;
    _bRawAt = now;
  }
  if (_bRaw != _bDown && (uint32_t)(now - _bRawAt) >= 30) {
    _bDown = _bRaw;
    if (_bDown) {
      _bDownAt = now;
      _longFired = false;
      _turnedSincePress = false;
      _fPress = true;
    } else if (!_longFired && !_turnedSincePress) {
      _fClick = true;
    }
  }
  if (_bDown && !_longFired && (uint32_t)(now - _bDownAt) >= _longMs && !_turnedSincePress) {
    _longFired = true;
    _fLong = true;
  }
}

// Переносит накопленные щелчки в значение: шаг, ускорение, границы
void AlashEncoder::sync() {
  noInterrupts();
  int16_t p = _pending;
  _pending = 0;
  uint16_t iv = _interval;
  interrupts();
  if (p == 0) return;

  if (_bDown) _turnedSincePress = true;
  _deltaAcc += p;
  if (p > 0) _rightFlag = 1; else _leftFlag = 1;

  int32_t mult = 1;
  if (_accel) mult = iv < 30 ? 10 : iv < 60 ? 5 : iv < 120 ? 2 : 1;   // чем быстрее крутят, тем больше шаг
  int32_t v = _value + (int32_t)p * _step * mult;
  if (_wrap && _min < _max) {
    int64_t range = (int64_t)_max - _min + 1;
    int64_t m = ((int64_t)v - _min) % range;
    if (m < 0) m += range;
    v = (int32_t)(m + _min);
  } else {
    if (v < _min) v = _min;
    if (v > _max) v = _max;
  }
  if (v != _value) {
    _value = v;
    _changedFlag = true;
  }
}

void AlashEncoder::poll() {
  if (!_started) begin();
  if (!_irq) pollRotation();
  pollButton();
  sync();
}

int32_t AlashEncoder::value() { poll(); return _value; }
void AlashEncoder::setValue(int32_t v) {
  poll();
  _value = v < _min ? _min : v > _max ? _max : v;
}
void AlashEncoder::setLimits(int32_t minValue, int32_t maxValue, bool wrap) {
  _min = minValue;
  _max = maxValue;
  _wrap = wrap;
  if (_value < _min) _value = _min;
  if (_value > _max) _value = _max;
}
int16_t AlashEncoder::delta() {
  poll();
  int16_t d = _deltaAcc;
  _deltaAcc = 0;
  return d;
}
bool AlashEncoder::turnedRight() { poll(); bool f = _rightFlag; _rightFlag = 0; return f; }
bool AlashEncoder::turnedLeft() { poll(); bool f = _leftFlag; _leftFlag = 0; return f; }
bool AlashEncoder::changed() { poll(); bool f = _changedFlag; _changedFlag = false; return f; }

bool AlashEncoder::isPressed() { poll(); return _bDown; }
bool AlashEncoder::pressed() { poll(); bool f = _fPress; _fPress = false; return f; }
bool AlashEncoder::clicked() { poll(); bool f = _fClick; _fClick = false; return f; }
bool AlashEncoder::longPressed() { poll(); bool f = _fLong; _fLong = false; return f; }

const __FlashStringHelper *AlashEncoder::errorText() {
  switch (_err) {
    case ERR_OK: return F("OK");
    case ERR_NO_INTERRUPT:
      return T("Для прерываний нужны пины с прерыванием (Uno / Nano: только 2 и 3) и не больше 4 энкодеров. Используйте begin() без аргумента или другие пины",
               "Үзіліс үшін үзілісі бар пиндер керек (Uno / Nano: тек 2 және 3) және 4 энкодерден көп емес. begin()-ді аргументсіз қолданыңыз немесе басқа пиндер таңдаңыз",
               "Interrupts need pins with interrupt support (Uno / Nano: only 2 and 3) and at most 4 encoders. Use begin() without arguments or other pins");
    case ERR_NO_PULLUP:
      return T("У этих пинов ESP32 (GPIO34-39) нет внутренней подтяжки: поставьте резисторы 10 кОм к 3.3 В на A, B и кнопку и вызовите setPinMode(ENC_EXTERNAL) и setButtonMode(ENC_EXTERNAL), либо выберите другие пины. У модуля KY-040 резисторы уже стоят: достаточно ENC_EXTERNAL.",
               "ESP32-нің бұл пиндерінде (GPIO34-39) ішкі тартқыш жоқ: A, B және түймеге 3.3 В-қа 10 кОм резисторлар қойып, setPinMode(ENC_EXTERNAL) және setButtonMode(ENC_EXTERNAL) шақырыңыз немесе басқа пиндер таңдаңыз. KY-040 модулінде резисторлар бар: ENC_EXTERNAL жеткілікті.",
               "These ESP32 pins (GPIO34-39) have no internal pull-up: add 10 kOhm resistors to 3.3 V on A, B and the button and call setPinMode(ENC_EXTERNAL) and setButtonMode(ENC_EXTERNAL), or pick other pins. The KY-040 module already has the resistors: ENC_EXTERNAL is enough.");
  }
  return F("");
}

void AlashEncoder::diagnose(Print &out) {
  if (!_started) begin();
  out.println(T("AlashEncoder: диагностика", "AlashEncoder: диагностика", "AlashEncoder: diagnostics"));
  out.print(T("Пины A / B: ", "A / B пиндері: ", "Pins A / B: "));
  out.print(_a);
  out.print(F(" / "));
  out.println(_b);
  out.print(T("Уровни A, B сейчас: ", "A, B деңгейлері қазір: ", "Levels A, B now: "));
  out.print(digitalRead(_a));
  out.print(F(", "));
  out.println(digitalRead(_b));
  if (_btn >= 0) {
    out.print(T("Кнопка (SW): ", "Түйме (SW): ", "Button (SW): "));
    out.println(digitalRead(_btn) == (_bLow ? LOW : HIGH) ? T("нажата", "басылған", "pressed") : T("отпущена", "жіберілген", "released"));
  }
  out.print(T("Значение: ", "Мәні: ", "Value: "));
  out.println((int)value());
  if (_err != ERR_OK) out.println(errorText());
  out.println(T("Покрутите ручку и запустите снова: уровни A и B должны меняться. Если значение растёт на 2 или 4 за один щелчок, вызовите setStepsPerClick(2) или (1). Если направление обратное, вызовите reverse(). Питание модуля KY-040: 5 В или 3.3 В, GND.",
                "Тұтқаны бұрап, қайта іске қосыңыз: A және B деңгейлері өзгеруі керек. Бір шерткенде мән 2 немесе 4-ке өссе, setStepsPerClick(2) немесе (1) шақырыңыз. Бағыт кері болса, reverse() шақырыңыз. KY-040 қуаты: 5 В немесе 3.3 В, GND.",
                "Turn the knob and run again: levels A and B must change. If the value grows by 2 or 4 per click, call setStepsPerClick(2) or (1). If the direction is reversed, call reverse(). KY-040 power: 5 V or 3.3 V, GND."));
}
