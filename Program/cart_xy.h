#pragma once
#include <Arduino.h>
#include <GyverStepper2.h>

class cart_xy {
private:
  GStepper2<STEPPER2WIRE> left;
  GStepper2<STEPPER2WIRE> right;
  GStepper2<STEPPER2WIRE> y;

  float stepsPerMmX;
  float stepsPerMmY;
  int maxSpeed;
  int maxAccel;

public:
  cart_xy(int res,
          int l_dir, int l_step,
          int r_dir, int r_step,
          int y_dir, int y_step,
          int maxSpeed = 32000,
          int maxAccel = 3200,
          float stepsMmX = 23.02,
          float stepsMmY = 3200)
    : left(res, l_step, l_dir),
      right(res, r_step, r_dir),
      y(res, y_step, y_dir),
      stepsPerMmX(stepsMmX),
      stepsPerMmY(stepsMmY),
      maxSpeed(maxSpeed),
      maxAccel(maxAccel)
  {
    left.reverse(-1);

    left.setMaxSpeed(maxSpeed);
    right.setMaxSpeed(maxSpeed);
    y.setMaxSpeed(maxSpeed);

    left.setAcceleration(maxAccel);
    right.setAcceleration(maxAccel);
    y.setAcceleration(maxAccel);
  }

  void setZero() {
    left.reset();
    right.reset();
    y.reset();
  }

  void setPos(float xMm, float yMm) {
    long xSteps = xMm * stepsPerMmX;
    long ySteps = yMm * stepsPerMmY;
    left.setTarget(xSteps);
    right.setTarget(xSteps);
    y.setTarget(ySteps);
  }

  void setPosX(float Mm) {
    long xSteps = Mm * stepsPerMmX;
    left.setTarget(xSteps);
    right.setTarget(xSteps);
  }

  void setPosY(float Mm) {
    long ySteps = Mm * stepsPerMmY;
    y.setTarget(ySteps);
  }

  bool tick() {
    bool l = left.tick();
    bool r = right.tick();
    bool yy = y.tick();
    return l || r || yy;
  }

  float getX() {
    return (left.getCurrent() + right.getCurrent()) / 2.0 / stepsPerMmX;
  }

  float getY() {
    return y.getCurrent() / stepsPerMmY;
  }

  void gotoPos(float x, float y) {
    setPos(x, y);
    unsigned long start = millis();
    while (tick() && (millis() - start < 30000)) {
    }
  }

  void gotoPosX(float Mm) {
    setPosX(Mm);
    unsigned long start = millis();
    while (tick() && (millis() - start < 30000)) {
    }
  }

  void gotoPosY(float Mm) {
    setPosY(Mm);
    unsigned long start = millis();
    while (tick() && (millis() - start < 30000)) {

    }
  }
};

