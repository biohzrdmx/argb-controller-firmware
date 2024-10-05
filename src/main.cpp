#include "main.h"

#include <YetAnotherPcInt.h>
#include <EasyButton.h>
#include <RotaryEncoder.h>
#include <TM1651.h>
#include <FastLED.h>

#define BTN_DEBOUNCE    40

#define PIN_ENCODER_BTN A2
#define PIN_ENCODER_A   A1
#define PIN_ENCODER_B   A0
#define PIN_LED_CLK     A3
#define PIN_LED_DIO     A4
#define PIN_OUTPUT      9

#define LED_TYPE        WS2811
#define COLOR_ORDER     GRB
#define NUM_LEDS        17

#define MODE_SOLID      0

#define COLOR_RED       0
#define COLOR_GREEN     1
#define COLOR_BLUE      2

CRGB leds[NUM_LEDS];

RotaryEncoder encoder(PIN_ENCODER_A, PIN_ENCODER_B);
EasyButton btnEncoder(PIN_ENCODER_BTN, BTN_DEBOUNCE, true);
TM1651 display(PIN_LED_CLK, PIN_LED_DIO);

bool config_mode = false;

int16_t oldPos = 0;

uint8_t mode = MODE_SOLID;
uint8_t color = COLOR_RED;

int8_t brightness = 100;
int8_t red = 255;
int8_t green = 0;
int8_t blue = 0;

void setup() {
  Serial.begin(9600);
  // Setup display
  display.displaySet(BRIGHT_TYPICAL);
  // Setup encoder
  btnEncoder.onPressed(onPressedEncoder);
  btnEncoder.onPressedFor(1000, onLongPressedEncoder);
  PcInt::attachInterrupt(PIN_ENCODER_A, encoderInterruptHandler, CHANGE);
  PcInt::attachInterrupt(PIN_ENCODER_B, encoderInterruptHandler, CHANGE);

  display.displayInteger(0);
  delay(1000);
  FastLED.addLeds<LED_TYPE, PIN_OUTPUT, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  
  Serial.println("boot OK");
  // brightness = EEPROM.read(0);
  // if (brightness < 0 || brightness > 100) {
  //   brightness = 30;
  // }
}

void loop() {
  int newPos = encoder.getPosition();
  btnEncoder.read();
  // If the encoder changed, act depending on the current state
  if (newPos != oldPos) {
    RotaryEncoder::Direction dir = encoder.getDirection();
    if (config_mode) {
      switch (mode) {
        case MODE_SOLID:
          switch (color) {
            case COLOR_RED:
              red += (int)dir;
              red = clamp(red, 0, 255);
            break;
            case COLOR_GREEN:
              green += (int)dir;
              green = clamp(green, 0, 255);
            break;
            case COLOR_BLUE:
              blue += (int)dir;
              blue = clamp(blue, 0, 255);
            break;
          }
        break;
      }
    } else {
      // Set brightness
      brightness += (int)dir;
      brightness = clamp(brightness, 0, 100);
    }
    oldPos = newPos;
  }
  // Update display
  if (config_mode) {
    switch (mode) {
      case MODE_SOLID:
        switch (color) {
          case COLOR_RED:
            display.displayInteger(red);
          break;
          case COLOR_GREEN:
            display.displayInteger(green);
          break;
          case COLOR_BLUE:
            display.displayInteger(blue);
          break;
        }
      break;
    }
  } else {
    display.displayInteger(brightness);
  }
  // Update leds
  switch (mode) {
    case MODE_SOLID:
      leds->setRGB(red, green, blue);
    break;
  }
  FastLED.setBrightness(brightness);
  FastLED.show();  
}

int clamp(int n, int lower, int upper){
  return max(lower, min(n, upper)); 
}

void encoderInterruptHandler() {
  encoder.tick();
}

void onPressedEncoder() {
  if (config_mode) {
    switch (mode) {
      case MODE_SOLID:
        switch (color) {
          case COLOR_RED:
            color = COLOR_GREEN;
          break;
          case COLOR_GREEN:
            color = COLOR_BLUE;
          break;
          case COLOR_BLUE:
            config_mode = false;
          break;
        }
      break;
    }
  } else {
    // TBD: Change mode
  }
}

void onLongPressedEncoder() {
  if (!config_mode) {
    config_mode = true;
    switch (mode) {
      case MODE_SOLID:
        color = COLOR_RED;
      break;
    }
  } else {
    config_mode = false;
  }
}