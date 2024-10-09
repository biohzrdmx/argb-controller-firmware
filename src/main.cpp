#include "main.h"

#include <YetAnotherPcInt.h>
#include <EasyButton.h>
#include <TM1651.h>
#include <FastLED.h>
#include <Timer.h>

#define BTN_DEBOUNCE    40

#define PIN_ENCODER_BTN A2
#define PIN_ENCODER_A   A1
#define PIN_ENCODER_B   A0
#define PIN_LED_CLK     A3
#define PIN_LED_DIO     A4
#define PIN_OUTPUT      9

#define LED_TYPE        WS2811
#define COLOR_ORDER     GRB
#define NUM_LEDS        8

#define MODE_SOLID      0
#define MODE_RAINBOW    1

#define COLOR_RED       0
#define COLOR_GREEN     1
#define COLOR_BLUE      2

CRGB leds[NUM_LEDS];

RotaryEncoder encoder(PIN_ENCODER_A, PIN_ENCODER_B);
EasyButton btnEncoder(PIN_ENCODER_BTN, BTN_DEBOUNCE, true);
TM1651 display(PIN_LED_CLK, PIN_LED_DIO);

Timer timer;

bool config_mode = false;

int16_t oldPos = 0;

uint8_t mode = MODE_RAINBOW;
uint8_t color = COLOR_RED;
uint8_t hue = 0;

uint8_t interval = 50;
uint8_t brightness = 100;
uint8_t red = 255;
uint8_t green = 0;
uint8_t blue = 0;

void setup() {
  // Setup display
  display.displaySet(BRIGHT_TYPICAL);
  // Setup encoder
  btnEncoder.onPressed(onPressedEncoder);
  btnEncoder.onPressedFor(1000, onLongPressedEncoder);
  PcInt::attachInterrupt(PIN_ENCODER_A, encoderInterruptHandler, CHANGE);
  PcInt::attachInterrupt(PIN_ENCODER_B, encoderInterruptHandler, CHANGE);
  // Display test
  display.displayInteger(0);
  delay(500);
  // Setup leds
  FastLED.addLeds<LED_TYPE, PIN_OUTPUT, COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  // Leds test
  setColor(255, 0, 0);
  display.displayInteger(1);
  FastLED.show();
  delay(500);
  setColor(0, 255, 0);
  display.displayInteger(2);
  FastLED.show();
  delay(500);
  setColor(0, 0, 255);
  display.displayInteger(3);
  FastLED.show();
  delay(500);
  // Initialize timer
  timer.init(interval);
}

void loop() {
  int newPos = encoder.getPosition();
  btnEncoder.read();
  timer.update();
  // If the encoder changed, act depending on the current state
  if (newPos != oldPos) {
    RotaryEncoder::Direction dir = encoder.getDirection();
    if (config_mode) {
      switch (mode) {
        case MODE_SOLID:
          switch (color) {
            case COLOR_RED:
              red = getValue(dir, red, 0, 255);
            break;
            case COLOR_GREEN:
              green = getValue(dir, green, 0, 255);
            break;
            case COLOR_BLUE:
              blue = getValue(dir, blue, 0, 255);
            break;
          }
        break;
        case MODE_RAINBOW:
          interval = getValue(dir, interval, 10, 180);
        break;
      }
    } else {
      // Set brightness
      brightness = getValue(dir, brightness, 0, 100);
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
      case MODE_RAINBOW:
        display.displayInteger(interval);
      break;
    }
  } else {
    display.displayInteger(brightness);
  }
  if ( timer.hasFinished() ) {
    hue++;
    timer.init(interval);
  }
  // Update leds
  switch (mode) {
    case MODE_SOLID:
      setColor(red, green, blue);
    break;
    case MODE_RAINBOW:
      fill_rainbow( leds, NUM_LEDS, hue, 7);
    break;
  }
  FastLED.setBrightness(brightness);
  FastLED.show();
}

void setColor(uint8_t red, uint8_t green, uint8_t blue) {
  for(int i = 0; i < NUM_LEDS; i++) {   
    leds[i] = CRGB(red, green, blue);
  }
}

uint8_t getValue(RotaryEncoder::Direction dir, uint8_t value, uint8_t min, uint8_t max) {
  switch (dir) {
    case RotaryEncoder::Direction::CLOCKWISE:
      return (value < max) ? (value + 1) : max;
    break;
    case RotaryEncoder::Direction::COUNTERCLOCKWISE:
      return (value > min) ? (value - 1) : min;
    break;
    default:
      return value;
  }
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
      case MODE_RAINBOW:
        config_mode = false;
      break;
    }
  } else {
    switch (mode) {
      case MODE_SOLID:
        mode = MODE_RAINBOW;
        break;
      case MODE_RAINBOW:
        mode = MODE_SOLID;
        break;
    }
  }
}

void onLongPressedEncoder() {
  if (! config_mode ) {
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