// Тесты AlashEncoder без железа для ESP32 (пины GPIO34-39 без подтяжки, IRAM_ATTR): те же проверки с определением ESP32.
//   g++ -std=c++17 -Wall -DESP32 -I mock -I ../../src test_encoder_esp32.cpp ../../src/AlashEncoder.cpp && ./a.out
#include "test_encoder.cpp"
