# AlashEncoder

[Русский](#русский) | [Қазақша](#қазақша) | [English](#english)

---

## Русский

Простая библиотека для **поворотных энкодеров** (модуль KY-040 и подобные) с кнопкой. Считает щелчки, держит значение в границах, ускоряет при быстром вращении, различает клик и долгое нажатие. Без зависимостей. Сделана для уроков и наборов Alash Electronics.

```cpp
#include <AlashEncoder.h>
AlashEncoder knob(2, 3, 4);        // CLK, DT, SW

void setup() { Serial.begin(115200); knob.begin(); }
void loop() {
  if (knob.changed()) Serial.println(knob.value());
  if (knob.clicked()) Serial.println("Нажали");
}
```

### Что умеет
- **Любая схема подключения:** подтяжка платы, внешние резисторы (как у KY-040) или к земле (`setPinMode()`, `setButtonMode()`); кнопка к GND или к питанию; пины ESP32 GPIO34-39 с внешними резисторами.
- **Щелчки без дребезга:** алгоритм по коду Грея отбрасывает помехи и учитывает пропущенные состояния при быстром вращении; потерянный импульс не сдвигает счёт навсегда.
- **Значение:** `value()`, `setValue()`, `setStep(n)`, `setLimits(min, max)` и `setLimits(min, max, true)` для меню по кругу.
- **События:** `changed()`, `turnedRight()`, `turnedLeft()`, `delta()`.
- **Ускорение:** `setAcceleration(true)`: чем быстрее крутите, тем больше шаг (до 10 раз).
- **Кнопка (SW):** `pressed()`, `clicked()`, `longPressed()`, `isPressed()`. Если при нажатии крутили ручку, клика нет (удобно для "нажми и поверни").
- **Два режима:** опрос в `loop()` (по умолчанию) или прерывания `begin(true)`: не пропустит повороты, пока вы заняты другим. До 4 энкодеров.
- **Подстройка под модуль:** `setStepsPerClick(1|2|4)`, `reverse()`.
- **Понятные подсказки** на русском, казахском и английском: `diagnose(Serial)` и `errorText()`.
- Работает на **Arduino AVR, ESP32, ESP8266** и других платах.

### Подключение (KY-040)
| Модуль | Uno / Nano | ESP32 |
|---|---|---|
| + | 5 В | 3.3 В |
| GND | GND | GND |
| CLK | D2 | GPIO25 |
| DT | D3 | GPIO26 |
| SW | D4 | GPIO27 |

На Uno / Nano для режима прерываний нужны пины 2 и 3 для CLK и DT.

### Чем отличается от других библиотек
Мы посмотрели Encoder (Paul Stoffregen), RotaryEncoder (Matthias Hertel), Ai Esp32 Rotary Encoder и GyverEncoder:
- Большинство возвращает «сырые» импульсы: на модуле KY-040 значение растёт на 4 за щелчок, и ученик не понимает почему. У нас один щелчок это один шаг.
- Нет границ, цикла по кругу, ускорения: каждый пишет их сам. У нас это одна строка.
- Кнопку энкодера приходится подключать отдельно; у нас клик и долгое нажатие уже есть.
- Нет подсказок, что делать, если считает неправильно: у нас `diagnose()`.

### Примеры
`Basic`, `Menu`, `Volume`, `Interrupts`, `Diagnose`.

### Тесты
Вращение в обе стороны, помехи, пропущенные состояния, границы, ускорение, кнопка и прерывания проверены без железа: `extras/test`.

Подробное руководство: [docs/WIKI.md](docs/WIKI.md).

---

## Қазақша

**Айналмалы энкодерлерге** (KY-040 модулі және ұқсастары) арналған қарапайым кітапхана. Шерткілерді санайды, мәнді шектерде ұстайды, тез бұрағанда үдетеді, клик пен ұзақ басуды ажыратады. Тәуелділіксіз. Alash Electronics сабақтары мен жинақтарына арналған.

```cpp
#include <AlashEncoder.h>
AlashEncoder knob(2, 3, 4);        // CLK, DT, SW

void setup() { Serial.begin(115200); knob.begin(); }
void loop() {
  if (knob.changed()) Serial.println(knob.value());
  if (knob.clicked()) Serial.println("Басылды");
}
```

### Мүмкіндіктері
- **Кез келген қосу сұлбасы:** тақта тартқышы, сыртқы резисторлар (KY-040 сияқты) немесе жерге (`setPinMode()`, `setButtonMode()`); түйме GND-ға немесе қуатқа; сыртқы резисторлы ESP32 GPIO34-39 пиндері.
- **Шерткілер, діріл жоқ:** Грей коды бойынша алгоритм кедергілерді сүзеді, тез бұрағанда өткізіп алынған күйлерді ескереді.
- **Мән:** `value()`, `setValue()`, `setStep(n)`, `setLimits(min, max)`, айналдыру үшін `setLimits(min, max, true)`.
- **Оқиғалар:** `changed()`, `turnedRight()`, `turnedLeft()`, `delta()`.
- **Үдеу:** `setAcceleration(true)`.
- **Түйме (SW):** `pressed()`, `clicked()`, `longPressed()`, `isPressed()`.
- **Екі режим:** `loop()` ішінде сұрау (әдепкі) немесе үзілістер `begin(true)`. 4 энкодерге дейін.
- **Түсінікті кеңестер** орысша, қазақша, ағылшынша: `diagnose(Serial)`, `errorText()`.
- **Arduino AVR, ESP32, ESP8266** тақталарында жұмыс істейді.

### Қосу (KY-040)
| Модуль | Uno / Nano | ESP32 |
|---|---|---|
| + | 5 В | 3.3 В |
| GND | GND | GND |
| CLK | D2 | GPIO25 |
| DT | D3 | GPIO26 |
| SW | D4 | GPIO27 |

### Мысалдар
`Basic`, `Menu`, `Volume`, `Interrupts`, `Diagnose`.

Толық нұсқаулық: [docs/WIKI.md](docs/WIKI.md).

---

## English

A simple library for **rotary encoders** (KY-040 module and similar) with a button. Counts clicks, keeps the value within limits, accelerates on fast turns, tells click from long press. No dependencies. Made for classes and Alash Electronics kits.

```cpp
#include <AlashEncoder.h>
AlashEncoder knob(2, 3, 4);        // CLK, DT, SW

void setup() { Serial.begin(115200); knob.begin(); }
void loop() {
  if (knob.changed()) Serial.println(knob.value());
  if (knob.clicked()) Serial.println("Pressed");
}
```

### Features
- **Any wiring scheme:** the board pull-up, external resistors (like the KY-040) or pull-down (`setPinMode()`, `setButtonMode()`); a button to GND or to the supply; ESP32 GPIO34-39 pins with external resistors.
- **Clean clicks:** a Gray-code algorithm rejects noise and handles skipped states on fast turns; a lost pulse never shifts the count for good.
- **Value:** `value()`, `setValue()`, `setStep(n)`, `setLimits(min, max)`, and `setLimits(min, max, true)` for wrap-around menus.
- **Events:** `changed()`, `turnedRight()`, `turnedLeft()`, `delta()`.
- **Acceleration:** `setAcceleration(true)`.
- **Button (SW):** `pressed()`, `clicked()`, `longPressed()`, `isPressed()`. No click if you turned the knob while pressing.
- **Two modes:** polling in `loop()` (default) or interrupts with `begin(true)`. Up to 4 encoders.
- **Clear tips** in Russian, Kazakh and English: `diagnose(Serial)`, `errorText()`.
- Works on **Arduino AVR, ESP32, ESP8266** and others.

### Wiring (KY-040)
| Module | Uno / Nano | ESP32 |
|---|---|---|
| + | 5 V | 3.3 V |
| GND | GND | GND |
| CLK | D2 | GPIO25 |
| DT | D3 | GPIO26 |
| SW | D4 | GPIO27 |

### Examples
`Basic`, `Menu`, `Volume`, `Interrupts`, `Diagnose`.

Full guide: [docs/WIKI.md](docs/WIKI.md).

### License
MIT, Alash Electronics.
