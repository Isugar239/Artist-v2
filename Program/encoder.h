#pragma once
#include <Arduino.h>

class Encoder {
private:
  int pinA;
  int pinB;
  int pinTouchButton;
  bool prevA;
  bool prevTouchBtn;

public:
  int dir;
  bool clicked;

  Encoder(int a, int b, int touchButton)
    : pinA(a),
      pinB(b),
      pinTouchButton(touchButton),
      prevA(false),
      prevTouchBtn(false),
      dir(0),
      clicked(false)
  {
    pinMode(pinA, INPUT_PULLUP);
    pinMode(pinB, INPUT_PULLUP);
    pinMode(pinTouchButton, INPUT_PULLUP);
    prevA = digitalRead(pinA);
    prevTouchBtn = digitalRead(pinTouchButton);
  }

  void tick() {
    bool currA = digitalRead(pinA);
    bool currB = digitalRead(pinB);

    dir = 0;
    if (!prevA && currA) {
      dir = currB ? -1 : 1;
    }
    prevA = currA;

    bool currTouch = digitalRead(pinTouchButton);
    if (currTouch != prevTouchBtn) {
      clicked = currTouch;
      prevTouchBtn = currTouch;
    }
  }
};
