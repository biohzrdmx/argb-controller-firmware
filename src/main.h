#ifndef MAIN_h
#define MAIN_h

#include <Arduino.h>
#include <RotaryEncoder.h>

void setup();
void loop();
void setColor(uint8_t red,uint8_t green,uint8_t blue);
uint8_t getValue(RotaryEncoder::Direction dir, uint8_t value, uint8_t min, uint8_t max);

void onPressedEncoder();
void onLongPressedEncoder();
void encoderInterruptHandler();

#endif