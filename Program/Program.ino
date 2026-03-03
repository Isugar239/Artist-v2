#include <GyverOLED.h>
#include <Wire.h>
#include "GyverOLEDMenu.h"

#include "cart_xy.h"
#include "encoder.h"

#include "marker.h"

GyverOLED<SSD1306_128x64> oled;
OledMenu<10, GyverOLED<SSD1306_128x64>> menu(&oled);

const int PIN_LED = 13;
const int PIN_L_DIR = 2;
const int PIN_L_STEP = 4;
const int PIN_R_DIR = 3;
const int PIN_R_STEP = 5;
const int PIN_Y_DIR = 16;
const int PIN_Y_STEP = 18;

const int PIN_ENC_A = 10;
const int PIN_ENC_B = 11;
const int PIN_ENC_BTN = 12;

const int PIN_MARKER = 9;
const int MARKER_UP = 90;
const int MARKER_DOWN = 270;





const float STEPS_X = 23.02;
const float STEPS_Y = 400;
const int MAX_SPEED = 9000;
const int MAX_ACCEL = 2200;


const float X_OFFSET = 38.0;


Encoder enc(PIN_ENC_A, PIN_ENC_B, PIN_ENC_BTN);


Marker black(PIN_MARKER, MARKER_UP, MARKER_DOWN);
cart_xy IvanTM(16,
               PIN_L_DIR, PIN_L_STEP,
               PIN_R_DIR, PIN_R_STEP,
               PIN_Y_DIR, PIN_Y_STEP,
               MAX_SPEED, MAX_ACCEL,
               STEPS_X, STEPS_Y);

int gray = random(400, 600); //как карта ляжет

void moveTo(float x, float y) {
  black.up();
  IvanTM.gotoPos(x, y);
}

void drawLine(float x0, float y0, float x1, float y1) {
  moveTo(x0, y0);
  black.down();
  IvanTM.gotoPos(x1, y1);
  black.up();
}

void drawRect(float x0, float y0, float w, float h) {
  moveTo(x0, y0);
  black.down();
  IvanTM.gotoPos(x0 + w, y0);
  IvanTM.gotoPos(x0 + w, y0 + h);
  IvanTM.gotoPos(x0, y0 + h);
  IvanTM.gotoPos(x0, y0);
  black.up();
}

void calibrateGray() {
  int minV = 1023;
  int maxV = 0;
  float startX = IvanTM.getX();
  IvanTM.setPosX(startX + 150);
  while (IvanTM.tick()) {
    int v = analogRead(A1);
    if (v < minV) minV = v;
    if (v > maxV) maxV = v;
  }
  gray = (minV + maxV) / 2;
}

void blackline() {
  float startX = IvanTM.getX();
  IvanTM.setPosX(startX + 250);
  while (IvanTM.tick()) {
    int v = analogRead(A1);
    if (v < gray) {
      IvanTM.setPosX(IvanTM.getX());
      break;
    }
  }
  IvanTM.gotoPosX(IvanTM.getX() - X_OFFSET);
  IvanTM.setZero();
}

// ---- Taski ----
void func1() {
  digitalWrite(PIN_LED, 1);
  black.up();
  calibrateGray();
  blackline();
  digitalWrite(PIN_LED, 0);
}

void func2() { black.up(); }
void func3() { black.down(); }
void func4() { drawLine(0, 0, 200, 0); }
void func5() { drawLine(0, 0, 0, -100); drawLine(0, 0, 0, 100); }
void func6() { drawRect(10, 10, 120, 60); }
void func7() { moveTo(0, 0); }
void func8() {}
void func9() {}
void func10() {}

void (*menuFuncs[10])() = { func1, func2, func3, func4, func5, func6, func7, func8, func9, func10 };

void onItemChange(const int index, const void* val, const byte valType) {
  if (valType == VAL_ACTION) {
    if (index >= 0 && index < 10) {
      menuFuncs[index]();
    }
  }
}

void encoderCallback() {
  if (enc.dir != 0) {
    if (enc.dir == 1) {
      menu.selectNext(false);
    } else {
      menu.selectPrev(false);
    }
    oled.update();
  }
  if (enc.clicked) {
    menu.toggleChangeSelected();
    oled.update();
  }
}

void setup() {
  pinMode(PIN_LED, OUTPUT);

  oled.init();
  Wire.setClock(400000L);
  oled.clear();
  oled.update();

  Serial.begin(9600);
  black.begin();
  
  menu.onChange(onItemChange, false);
  menu.addItem(PSTR("Calibr"));
  menu.addItem(PSTR("Up"));
  menu.addItem(PSTR("Down"));
  menu.addItem(PSTR("LineX"));
  menu.addItem(PSTR("LineY"));
  menu.addItem(PSTR("Rect"));
  menu.addItem(PSTR("Home"));
  menu.addItem(PSTR("8"));
  menu.addItem(PSTR("9"));
  menu.addItem(PSTR("10"));

  menu.showMenu(true);

  func5();
}

void loop() {
  enc.tick();
  encoderCallback();
  IvanTM.tick();
}
