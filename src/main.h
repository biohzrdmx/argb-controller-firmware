#ifndef MAIN_h
#define MAIN_h

#include <Arduino.h>

void setup();
void loop();
int clamp(int n, int lower, int upper);

void onPressedEncoder();
void onLongPressedEncoder();
void encoderInterruptHandler();

#endif