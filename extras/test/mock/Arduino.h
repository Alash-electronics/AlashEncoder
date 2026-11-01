// Минимальная имитация Arduino.h для запуска тестов на компьютере (g++).
#pragma once
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <algorithm>
using __FlashStringHelper = char;
#define F(x) ((const __FlashStringHelper *)(x))
#define INPUT 0
#define OUTPUT 1
#define INPUT_PULLUP 2
#define LOW 0
#define HIGH 1
#define CHANGE 1
#define NOT_AN_INTERRUPT -1
// Как в настоящем Arduino.h: эти слова заняты макросами, имена в библиотеке не должны с ними пересекаться
#define PULLUP 0x3
#define PULLDOWN 0x4
#define EXTERNAL 0x5
#define min(a, b) ((a) < (b) ? (a) : (b))
inline uint32_t g_now = 0;
inline uint8_t g_level[64];
inline void (*g_isr[64])();
inline uint32_t millis() { return g_now; }
inline void pinMode(uint8_t p, uint8_t) { g_level[p] = 1; }   // подтяжка: пин в покое HIGH
inline int digitalRead(uint8_t p) { return g_level[p]; }
inline int digitalPinToInterrupt(uint8_t p) { return (p == 2 || p == 3 || p >= 20) ? p : NOT_AN_INTERRUPT; }
inline void attachInterrupt(int n, void (*fn)(), int) { g_isr[n] = fn; }
inline void detachInterrupt(int n) { g_isr[n] = nullptr; }
inline void noInterrupts() {}
inline void interrupts() {}
// Тест меняет уровень пина; если на пине висит прерывание, оно срабатывает
inline void setPin(uint8_t p, int lvl) { if (g_level[p] != lvl) { g_level[p] = lvl; if (g_isr[p]) g_isr[p](); } }
class Print {
 public:
  void print(const char *s) { printf("%s", s); }
  void print(int v) { printf("%d", v); }
  void print(unsigned v) { printf("%u", v); }
  void println(const char *s = "") { printf("%s\n", s); }
  void println(int v) { printf("%d\n", v); }
};
