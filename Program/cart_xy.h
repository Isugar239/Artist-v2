#pragma once
#include <Arduino.h>
#include <GyverStepper2.h>
#include <GyverPlanner2.h>

class cart_xy {
private:
  GStepper2<STEPPER2WIRE> left;
  GStepper2<STEPPER2WIRE> right;
  GStepper2<STEPPER2WIRE> y;
  GPlanner2<STEPPER2WIRE, 3> planner;

  float stepsPerMmX;
  float stepsPerMmY;
  int maxSpeed;
  int maxAccel;
  int maxSpeedDiag;
  int maxAccelDiag;

  void queueTarget(int32_t xLeft, int32_t xRight, int32_t ySteps) {
    int32_t cur[3] = { planner.getCurrent(0), planner.getCurrent(1), planner.getCurrent(2) };
    int32_t dx = abs(xLeft - cur[0]) + abs(xRight - cur[1]);
    int32_t dy = abs(ySteps - cur[2]);

    int speed = (dx > 0 && dy > 0) ? maxSpeedDiag : maxSpeed;
    int accel = (dx > 0 && dy > 0) ? maxAccelDiag : maxAccel;
    if (speed < 1) speed = 1;
    if (accel < 1) accel = 1;

    int32_t tar[3] = { xLeft, xRight, ySteps };
    planner.brake();
    planner.clearBuffer();
    planner.setMaxSpeed(speed);
    planner.setAcceleration(accel);
    planner.addTarget(cur, 0);
    planner.addTarget(tar, 1);
    planner.start();
  }

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
      maxAccel(maxAccel),
      maxSpeedDiag(maxSpeed / 2),
      maxAccelDiag(maxAccel / 2)
  {
    left.reverse(-1);
    right.reverse(-1);
    y.reverse(-1);
    left.setMaxSpeed(maxSpeed);
    right.setMaxSpeed(maxSpeed);
    y.setMaxSpeed(maxSpeed);

    left.setAcceleration(maxAccel);
    right.setAcceleration(maxAccel);
    y.setAcceleration(maxAccel);

    planner.addStepper(0, left);
    planner.addStepper(1, right);
    planner.addStepper(2, y);
    planner.setMaxSpeed(maxSpeed);
    planner.setAcceleration(maxAccel);
    planner.start();
  }

  void setDiagLimits(int speed, int accel) {
    maxSpeedDiag = speed;
    maxAccelDiag = accel;
  }

  void setZero() {
    planner.brake();
    planner.clearBuffer();
    planner.reset();
  }

  void setPos(float xMm, float yMm) {
    int32_t xSteps = xMm * stepsPerMmX;
    int32_t ySteps = yMm * stepsPerMmY;
    queueTarget(xSteps, xSteps, ySteps);
  }

  void setPosX(float Mm) {
    int32_t xSteps = Mm * stepsPerMmX;
    int32_t ySteps = planner.getCurrent(2);
    queueTarget(xSteps, xSteps, ySteps);
  }

  void setPosY(float Mm) {
    int32_t ySteps = Mm * stepsPerMmY;
    int32_t xLeft = planner.getCurrent(0);
    int32_t xRight = planner.getCurrent(1);
    queueTarget(xLeft, xRight, ySteps);
  }

  bool tick() {
    return planner.tick();
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
  void zeroY(){
    planner.brake();
    planner.clearBuffer();

    planner.setSpeed(2, -maxSpeed/2);
    while (digitalRead(19)) {
      planner.tick();
    }
    planner.brake();
    int32_t cur[3] = { planner.getCurrent(0), planner.getCurrent(1), 0 };
    planner.setCurrent(cur);
  }
};
