// Тесты AlashEncoder без железа (уровни пинов и время задаёт тест):
//   g++ -std=c++17 -Wall -I mock -I ../../src test_encoder.cpp ../../src/AlashEncoder.cpp && ./a.out
#include <stdio.h>
#include "Arduino.h"
#include "AlashEncoder.h"

static int fails = 0;
#define CHECK(cond) do { if (!(cond)) { printf("FAIL line %d: %s\n", __LINE__, #cond); fails++; } } while (0)

// Состояния (A<<1)|B. Щелчок вправо: 11 -> 10 -> 00 -> 01 -> 11. Влево: 11 -> 01 -> 00 -> 10 -> 11.
static void go(uint8_t pa, uint8_t pb, int st) { setPin(pa, (st >> 1) & 1); setPin(pb, st & 1); }
static void clickRight(AlashEncoder &e, uint8_t pa, uint8_t pb, uint32_t gap = 100, bool poll = true) {
  const int s[] = {2, 0, 1, 3};
  for (int x : s) { g_now += gap / 4; go(pa, pb, x); if (poll) e.update(); }
}
static void clickLeft(AlashEncoder &e, uint8_t pa, uint8_t pb, uint32_t gap = 100, bool poll = true) {
  const int s[] = {1, 0, 2, 3};
  for (int x : s) { g_now += gap / 4; go(pa, pb, x); if (poll) e.update(); }
}
static void reset() { memset(g_level, 1, sizeof g_level); memset(g_isr, 0, sizeof g_isr); g_now = 1000; }

int main() {
  // 1) вправо и влево
  reset();
  AlashEncoder e(4, 5);
  CHECK(e.begin());
  for (int i = 0; i < 3; i++) clickRight(e, 4, 5);
  CHECK(e.value() == 3);
  for (int i = 0; i < 5; i++) clickLeft(e, 4, 5);
  CHECK(e.value() == -2);

  // 2) события и delta
  reset(); AlashEncoder f(4, 5); f.begin();
  CHECK(!f.turnedRight() && !f.turnedLeft() && f.delta() == 0 && !f.changed());
  clickRight(f, 4, 5); clickRight(f, 4, 5);
  CHECK(f.turnedRight() && !f.turnedRight() && !f.turnedLeft());
  CHECK(f.delta() == 2 && f.delta() == 0);
  CHECK(f.changed() && !f.changed());
  clickLeft(f, 4, 5);
  CHECK(f.turnedLeft() && f.delta() == -1);

  // 3) помеха (дребезг) внутри щелчка не даёт лишних щелчков
  reset(); AlashEncoder n(4, 5); n.begin();
  const int noisy[] = {2, 3, 2, 0, 1, 3};                // 11 -> 10 -> 11 -> 10 -> 00 -> 01 -> 11
  for (int x : noisy) { g_now += 5; go(4, 5, x); n.update(); }
  CHECK(n.value() == 1);
  // неполный поворот туда и обратно не считается
  const int half[] = {2, 0, 2, 3};
  for (int x : half) { g_now += 5; go(4, 5, x); n.update(); }
  CHECK(n.value() == 1);

  // 4) пропущено состояние при быстром вращении (опрос не успел)
  reset(); AlashEncoder q(4, 5); q.begin();
  const int skip[] = {2, 1, 3};                           // 11 -> 10 -> (00 пропущено) 01 -> 11
  for (int x : skip) { g_now += 3; go(4, 5, x); q.update(); }
  CHECK(q.value() == 1);

  // 5) потерянный импульс не сдвигает счёт навсегда
  reset(); AlashEncoder l(4, 5); l.begin();
  const int lost[] = {2, 3};                              // 11 -> 10 -> 11: один неполный шаг
  for (int x : lost) { g_now += 5; go(4, 5, x); l.update(); }
  for (int i = 0; i < 4; i++) clickRight(l, 4, 5);
  CHECK(l.value() == 4);

  // 6) reverse, 2 и 1 импульс на щелчок
  reset(); AlashEncoder r(4, 5); r.begin(); r.reverse();
  clickRight(r, 4, 5); CHECK(r.value() == -1);
  reset(); AlashEncoder h(4, 5); h.begin(); h.setStepsPerClick(2);
  clickRight(h, 4, 5);                                    // 4 перехода = 2 щелчка при 2 на щелчок
  CHECK(h.value() == 2);
  reset(); AlashEncoder o(4, 5); o.begin(); o.setStepsPerClick(1);
  clickRight(o, 4, 5);
  CHECK(o.value() == 4);

  // 7) шаг, границы, по кругу
  reset(); AlashEncoder s(4, 5); s.begin(); s.setStep(5); s.setLimits(0, 20);
  for (int i = 0; i < 10; i++) clickRight(s, 4, 5);
  CHECK(s.value() == 20);
  for (int i = 0; i < 10; i++) clickLeft(s, 4, 5);
  CHECK(s.value() == 0);
  reset(); AlashEncoder w(4, 5); w.begin(); w.setLimits(0, 9, true);
  for (int i = 0; i < 12; i++) clickRight(w, 4, 5);
  CHECK(w.value() == 2);
  for (int i = 0; i < 3; i++) clickLeft(w, 4, 5);
  CHECK(w.value() == 9);
  w.setValue(50); CHECK(w.value() == 9);

  // 8) ускорение: медленно +1, быстро +10 за щелчок
  reset(); AlashEncoder a(4, 5); a.begin(); a.setAcceleration(true);
  clickRight(a, 4, 5, 400); CHECK(a.value() == 1);
  clickRight(a, 4, 5, 20);  CHECK(a.value() == 11);

  // 9) кнопка: клик, долгое нажатие, при вращении клика нет
  reset(); AlashEncoder b(4, 5, 6); b.begin();
  g_now += 100; setPin(6, 0); for (int i = 0; i < 10; i++) { g_now += 10; b.update(); }
  CHECK(b.pressed() && b.isPressed());
  setPin(6, 1); for (int i = 0; i < 10; i++) { g_now += 10; b.update(); }
  CHECK(b.clicked() && !b.isPressed() && !b.longPressed());
  setPin(6, 0); for (int i = 0; i < 100; i++) { g_now += 10; b.update(); }
  CHECK(b.longPressed() && !b.longPressed());
  setPin(6, 1); for (int i = 0; i < 10; i++) { g_now += 10; b.update(); }
  CHECK(!b.clicked());
  setPin(6, 0); for (int i = 0; i < 10; i++) { g_now += 10; b.update(); }
  clickRight(b, 4, 5);                                    // покрутили при нажатой кнопке
  setPin(6, 1); for (int i = 0; i < 10; i++) { g_now += 10; b.update(); }
  CHECK(!b.clicked());                                    // клика нет
  CHECK(b.value() == 1);

  // 10) прерывания: щелчки считаются, даже если main их не опрашивает
  {
  reset(); AlashEncoder i1(2, 3); CHECK(i1.begin(true));
  for (int k = 0; k < 4; k++) clickRight(i1, 2, 3, 100, false);
  CHECK(i1.value() == 4);
  }
  {
  reset(); AlashEncoder i2(5, 6);
  CHECK(!i2.begin(true) && i2.error() == AlashEncoder::ERR_NO_INTERRUPT);   // пин 5 без прерывания (как на Uno)
  }
  {
  reset();
  AlashEncoder m1(20, 21), m2(22, 23), m3(24, 25), m4(26, 27), m5(28, 29);
  CHECK(m1.begin(true) && m2.begin(true) && m3.begin(true) && m4.begin(true));
  CHECK(!m5.begin(true) && m5.error() == AlashEncoder::ERR_NO_INTERRUPT);   // больше 4 энкодеров
  clickRight(m2, 22, 23, 100, false); clickRight(m4, 26, 27, 100, false);
  CHECK(m1.value() == 0 && m2.value() == 1 && m3.value() == 0 && m4.value() == 1);   // каждый считает свои
  }

  // 11) тексты
  e.setLanguage(AlashEncoder::LANG_KK);
  e.diagnose(*(new Print));

  if (fails) { printf("FAILED: %d\n", fails); return 1; }
  printf("ALL TESTS PASSED\n");
  return 0;
}
