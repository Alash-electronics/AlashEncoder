# AlashEncoder: руководство / нұсқаулық / guide

[Русский](#русский) | [Қазақша](#қазақша) | [English](#english)

---

## Русский

Простая библиотека для **поворотных энкодеров** (модуль KY-040 и подобные) с кнопкой. Считает щелчки, держит значение в границах, ускоряет при быстром вращении, различает клик и долгое нажатие. Без зависимостей. Сделана для уроков и наборов Alash Electronics.

### Установка
Arduino IDE: Скетч, Подключить библиотеку, Управлять библиотеками, найти **AlashEncoder** и нажать Установить. Или скачайте ZIP с GitHub: Скетч, Подключить библиотеку, Добавить .ZIP библиотеку.

### Платы
Arduino Uno / Nano / Mega (AVR), ESP32, ESP8266 и другие, где работает Arduino-ядро.

### Подключение (KY-040)
| Модуль | Uno / Nano | ESP32 |
|---|---|---|
| + | 5 В | 3.3 В |
| GND | GND | GND |
| CLK | D2 | GPIO25 |
| DT | D3 | GPIO26 |
| SW | D4 | GPIO27 |

На Uno / Nano для режима прерываний нужны пины 2 и 3 для CLK и DT.

### Быстрый старт
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
- **Щелчки без дребезга:** алгоритм по коду Грея отбрасывает помехи и учитывает пропущенные состояния при быстром вращении; потерянный импульс не сдвигает счёт навсегда.
- **Значение:** `value()`, `setValue()`, `setStep(n)`, `setLimits(min, max)` и `setLimits(min, max, true)` для меню по кругу.
- **События:** `changed()`, `turnedRight()`, `turnedLeft()`, `delta()`.
- **Ускорение:** `setAcceleration(true)`: чем быстрее крутите, тем больше шаг (до 10 раз).
- **Кнопка (SW):** `pressed()`, `clicked()`, `longPressed()`, `isPressed()`. Если при нажатии крутили ручку, клика нет (удобно для "нажми и поверни").
- **Два режима:** опрос в `loop()` (по умолчанию) или прерывания `begin(true)`: не пропустит повороты, пока вы заняты другим. До 4 энкодеров.
- **Подстройка под модуль:** `setStepsPerClick(1|2|4)`, `reverse()`.
- **Понятные подсказки** на русском, казахском и английском: `diagnose(Serial)` и `errorText()`.
- Работает на **Arduino AVR, ESP32, ESP8266** и других платах.

### Справочник по методам
| Метод | Что делает |
|---|---|
| `begin(useInterrupts)` | Запуск; true = считать повороты по прерываниям |
| `value() / setValue(v)` | Текущее значение |
| `delta()` | Сколько щелчков с прошлого вызова |
| `turnedRight() / turnedLeft()` | Щелчок вправо / влево (один раз) |
| `changed()` | Значение изменилось |
| `setStep(n) / setLimits(min, max, wrap)` | Шаг и границы (wrap = по кругу) |
| `setAcceleration(on)` | Быстрое вращение меняет значение сильнее |
| `setStepsPerClick(n)` | 1, 2 или 4 импульса на щелчок |
| `reverse()` | Поменять направление |
| `clicked() / pressed() / longPressed()` | Кнопка энкодера (SW) |
| `diagnose(Serial)` | Самопроверка |

### Если что-то не работает
**Значение прыгает или считает в обе стороны**

Проверьте GND и подтяжки (у модулей KY-040 уже стоят). Попробуйте `setStepsPerClick(2)` или `(1)`: у вашего энкодера может быть другое число импульсов на щелчок.

**Направление наоборот**

Вызовите `encoder.reverse()` вместо перепайки A и B.

**Ошибка: на этом пине нет прерываний**

На Uno и Nano прерывания только на пинах 2 и 3. Используйте их или режим без прерываний `begin()`.

### Чем отличается от других библиотек
Мы посмотрели Encoder (Paul Stoffregen), RotaryEncoder (Matthias Hertel), Ai Esp32 Rotary Encoder и GyverEncoder:
- Большинство возвращает «сырые» импульсы: на модуле KY-040 значение растёт на 4 за щелчок, и ученик не понимает почему. У нас один щелчок это один шаг.
- Нет границ, цикла по кругу, ускорения: каждый пишет их сам. У нас это одна строка.
- Кнопку энкодера приходится подключать отдельно; у нас клик и долгое нажатие уже есть.
- Нет подсказок, что делать, если считает неправильно: у нас `diagnose()`.

### Самопроверка
В библиотеке есть `diagnose(Serial)`: она печатает, что видит плата, и подсказывает, что проверить. Язык подсказок: `setLanguage(LANG_RU / LANG_KK / LANG_EN)`.

### Примеры
`Basic`, `Menu`, `Volume`, `Interrupts`, `Diagnose`.

---

## Қазақша

**Айналмалы энкодерлерге** (KY-040 модулі және ұқсастары) арналған қарапайым кітапхана. Шерткілерді санайды, мәнді шектерде ұстайды, тез бұрағанда үдетеді, клик пен ұзақ басуды ажыратады. Тәуелділіксіз. Alash Electronics сабақтары мен жинақтарына арналған.

### Орнату
Arduino IDE: Sketch, Include Library, Manage Libraries, **AlashEncoder** табып, Install басыңыз. Немесе GitHub-тан ZIP жүктеп, Sketch, Include Library, Add .ZIP Library.

### Тақталар
Arduino Uno / Nano / Mega (AVR), ESP32, ESP8266 және Arduino ядросы бар басқалар.

### Қосу (KY-040)
| Модуль | Uno / Nano | ESP32 |
|---|---|---|
| + | 5 В | 3.3 В |
| GND | GND | GND |
| CLK | D2 | GPIO25 |
| DT | D3 | GPIO26 |
| SW | D4 | GPIO27 |

### Жылдам бастау
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
- **Шерткілер, діріл жоқ:** Грей коды бойынша алгоритм кедергілерді сүзеді, тез бұрағанда өткізіп алынған күйлерді ескереді.
- **Мән:** `value()`, `setValue()`, `setStep(n)`, `setLimits(min, max)`, айналдыру үшін `setLimits(min, max, true)`.
- **Оқиғалар:** `changed()`, `turnedRight()`, `turnedLeft()`, `delta()`.
- **Үдеу:** `setAcceleration(true)`.
- **Түйме (SW):** `pressed()`, `clicked()`, `longPressed()`, `isPressed()`.
- **Екі режим:** `loop()` ішінде сұрау (әдепкі) немесе үзілістер `begin(true)`. 4 энкодерге дейін.
- **Түсінікті кеңестер** орысша, қазақша, ағылшынша: `diagnose(Serial)`, `errorText()`.
- **Arduino AVR, ESP32, ESP8266** тақталарында жұмыс істейді.

### Әдістер анықтамасы
| Әдіс | Не істейді |
|---|---|
| `begin(useInterrupts)` | Іске қосу; true = бұруды үзіліспен санау |
| `value() / setValue(v)` | Ағымдағы мән |
| `delta()` | Өткен шақырудан бері неше шертпе |
| `turnedRight() / turnedLeft()` | Оңға / солға шертпе (бір рет) |
| `changed()` | Мән өзгерді |
| `setStep(n) / setLimits(min, max, wrap)` | Қадам және шектер (wrap = айналма) |
| `setAcceleration(on)` | Жылдам бұру мәнді көбірек өзгертеді |
| `setStepsPerClick(n)` | Шертпеге 1, 2 не 4 импульс |
| `reverse()` | Бағытты ауыстыру |
| `clicked() / pressed() / longPressed()` | Энкодер түймесі (SW) |
| `diagnose(Serial)` | Өзін тексеру |

### Бірдеңе жұмыс істемесе
**Мән секіреді не екі жаққа санайды**

GND мен тартуларды тексеріңіз. `setStepsPerClick(2)` не `(1)` қолданып көріңіз.

**Бағыты керісінше**

A және B-ны ауыстырудың орнына `encoder.reverse()` шақырыңыз.

**Қате: бұл пинде үзіліс жоқ**

Uno мен Nano-да үзіліс тек 2 және 3 пиндерде. Оларды қолданыңыз не үзіліссіз `begin()`.

### Өзін-өзі тексеру
Кітапханада `diagnose(Serial)` бар: тақта не көргенін басып шығарады және нені тексеру керегін айтады. Кеңес тілі: `setLanguage(LANG_RU / LANG_KK / LANG_EN)`.

### Мысалдар
`Basic`, `Menu`, `Volume`, `Interrupts`, `Diagnose`.

---

## English

A simple library for **rotary encoders** (KY-040 module and similar) with a button. Counts clicks, keeps the value within limits, accelerates on fast turns, tells click from long press. No dependencies. Made for classes and Alash Electronics kits.

### Installation
Arduino IDE: Sketch, Include Library, Manage Libraries, search **AlashEncoder** and click Install. Or download the ZIP from GitHub: Sketch, Include Library, Add .ZIP Library.

### Boards
Arduino Uno / Nano / Mega (AVR), ESP32, ESP8266 and others with an Arduino core.

### Wiring (KY-040)
| Module | Uno / Nano | ESP32 |
|---|---|---|
| + | 5 V | 3.3 V |
| GND | GND | GND |
| CLK | D2 | GPIO25 |
| DT | D3 | GPIO26 |
| SW | D4 | GPIO27 |

### Quick start
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
- **Clean clicks:** a Gray-code algorithm rejects noise and handles skipped states on fast turns; a lost pulse never shifts the count for good.
- **Value:** `value()`, `setValue()`, `setStep(n)`, `setLimits(min, max)`, and `setLimits(min, max, true)` for wrap-around menus.
- **Events:** `changed()`, `turnedRight()`, `turnedLeft()`, `delta()`.
- **Acceleration:** `setAcceleration(true)`.
- **Button (SW):** `pressed()`, `clicked()`, `longPressed()`, `isPressed()`. No click if you turned the knob while pressing.
- **Two modes:** polling in `loop()` (default) or interrupts with `begin(true)`. Up to 4 encoders.
- **Clear tips** in Russian, Kazakh and English: `diagnose(Serial)`, `errorText()`.
- Works on **Arduino AVR, ESP32, ESP8266** and others.

### Method reference
| Method | What it does |
|---|---|
| `begin(useInterrupts)` | Start; true = count turns by interrupts |
| `value() / setValue(v)` | Current value |
| `delta()` | Clicks since the last call |
| `turnedRight() / turnedLeft()` | A click right / left (once) |
| `changed()` | The value changed |
| `setStep(n) / setLimits(min, max, wrap)` | Step and limits (wrap = around) |
| `setAcceleration(on)` | Fast turning changes the value more |
| `setStepsPerClick(n)` | 1, 2 or 4 pulses per click |
| `reverse()` | Swap direction |
| `clicked() / pressed() / longPressed()` | The encoder button (SW) |
| `diagnose(Serial)` | Self-check |

### If something does not work
**The value jumps or counts both ways**

Check GND and pull-ups (KY-040 modules have them). Try `setStepsPerClick(2)` or `(1)`: your encoder may have a different number of pulses per click.

**The direction is backwards**

Call `encoder.reverse()` instead of swapping A and B.

**Error: this pin has no interrupts**

On Uno and Nano interrupts are only on pins 2 and 3. Use them or the polling mode `begin()`.

### Self-check
The library has `diagnose(Serial)`: it prints what the board sees and tells what to check. Tips language: `setLanguage(LANG_RU / LANG_KK / LANG_EN)`.

### Examples
`Basic`, `Menu`, `Volume`, `Interrupts`, `Diagnose`.

---

MIT License, Alash Electronics. https://github.com/Alash-electronics/AlashEncoder
