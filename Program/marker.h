#pragma once
#include <Arduino.h>
#include <Servo.h>

class Marker {
private:
  Servo pen;
  int pin;
  int posUp;
  int posDown;

public:
  Marker(int pin, int up, int down)
    : pin(pin), posUp(up), posDown(down) {}

  void begin() {
    pen.attach(pin);
    up();
  }

  void up() {
    pen.write(posUp);
    delay(300);
  }

  void down() {
    pen.write(posDown);
    delay(300);
  }

  void dot() {
    down();
    up();
  }
};
