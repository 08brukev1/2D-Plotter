#ifndef ALPHABET.h
#define ALPHABET.h

#include <Arduino.h>
#include <ESP32Servo.h>
#include <U8g2lib.h>
#include <Wire.h>

class alphabet
{
private:
  /* data */
public:
  

  void drawA();
};

void alphabet::drawA(){
  penDown();
  moveXY_DDA(LETTER_W / 4, LETTER_H, 1000);
  moveXY_DDA(LETTER_W / 4, -LETTER_H, 1000);
  penUp();
  moveXY_DDA(-LETTER_W / 2.5, LETTER_H / 2, 1000);
  penDown();
  moveXY_DDA(LETTER_W / 4, 0, 1000);
  penUp();
  moveXY_DDA(LETTER_W * 0.75 + SPACE_W, -LETTER_H * 0.5, 1000);
}



#endif // MACRO


void drawA()
{
  
}

void drawB()
{
  float r = LETTER_H / 4;

  penDown();
  moveXY_DDA(0, LETTER_H, 800); // linker Stamm

  moveXY_DDA(LETTER_W / 8, 0, 1000);
  drawArc(r, PI / 2, -PI / 2, 30, 1000); // obere Rundung
  moveXY_DDA(-LETTER_W / 8, 0, 1000);
  moveXY_DDA(LETTER_W / 6, 0, 1000);
  drawArc(r, PI / 2, -PI / 2, 30, 1000); // untere Rundung
  moveXY_DDA(-LETTER_W / 6, 0, 1000);
  delay(300);
  penUp();
  moveXY_DDA(LETTER_W + SPACE_W, 0, 1000);
}

void drawC()
{
  float r = LETTER_H / 2;

  penUp();
  moveXY_DDA(r + LETTER_W / 6, LETTER_H, 800);

  penDown();
  moveXY_DDA(-LETTER_W / 6, 0, 800);
  drawArc(r, PI / 2, 3 * PI / 2, 30, 800);
  moveXY_DDA(LETTER_W / 6, 0, 800);
  penUp();
  moveXY_DDA(r + SPACE_W, 0, 1000);
}

void drawD()
{
  float r = LETTER_H / 2;

  penDown();
  moveXY_DDA(0, LETTER_H, 800);

  drawArc(r, PI / 2, -PI / 2, 60, 800);

  penUp();
  moveXY_DDA(LETTER_W + SPACE_W, 0, 1000);
}

void drawE()
{
  penDown();
  moveXY_DDA(0, LETTER_H, 1000);
  moveXY_DDA(LETTER_W / 1.5, 0, 1000);
  penUp();
  moveXY_DDA(-LETTER_W / 1.5, -LETTER_H / 2, 1000);
  penDown();
  moveXY_DDA(LETTER_W / 2, 0, 1000);
  penUp();
  moveXY_DDA(-LETTER_W / 2, -LETTER_H / 2, 1000);
  penDown();
  moveXY_DDA(LETTER_W / 1.5, 0, 1000);
  penUp();
  moveXY_DDA(SPACE_W, 0, 1000);
}

void drawF()
{
  penDown();
  moveXY_DDA(0, LETTER_H, 1000);
  moveXY_DDA(LETTER_W, 0, 1000);
  penUp();
  moveXY_DDA(-LETTER_W, -LETTER_H / 2, 1000);
  penDown();
  moveXY_DDA(LETTER_W * 0.7, 0, 1000);
  penUp();
  moveXY_DDA(LETTER_W * 0.3 + SPACE_W, -LETTER_H / 2, 1000);
}

void drawG()
{
  float r = LETTER_H / 2;

  penUp();
  moveXY_DDA(r, r, 800);

  penDown();
  drawArc(r, 0.3, 2 * PI - 0.3, 80, 800);

  moveXY_DDA(-r / 2, 0, 800);
  penUp();
  moveXY_DDA(r / 2, 0, 800);
  penDown();
  moveXY_DDA(0, -LETTER_H / 4, 800);

  penUp();
  moveXY_DDA(SPACE_W, 0, 1000);
}

void drawH()
{
  penDown();
  moveXY_DDA(0, LETTER_H, 1000);
  penUp();
  moveXY_DDA(LETTER_W / 2, 0, 1000);
  penDown();
  moveXY_DDA(0, -LETTER_H, 1000);
  penUp();
  moveXY_DDA(-LETTER_W / 2, LETTER_H / 2, 1000);
  penDown();
  moveXY_DDA(LETTER_W / 2, 0, 1000);
  penUp();
  moveXY_DDA(SPACE_W, -LETTER_H / 2, 1000);
}

void drawI()
{
  penDown();
  moveXY_DDA(LETTER_W / 2, 0, 1000);
  moveXY_DDA(-LETTER_W / 4, 0, 1000);
  moveXY_DDA(0, LETTER_H, 1000);
  penUp();
  moveXY_DDA(-LETTER_W / 4, 0, 1000);
  penDown();
  moveXY_DDA(LETTER_W / 2, 0, 1000);
  penUp();
  moveXY_DDA(SPACE_W, -LETTER_H, 1000);
}

void drawJ()
{
  penUp();
  moveXY_DDA(LETTER_W, LETTER_H, 1000);
  penDown();
  moveXY_DDA(0, -LETTER_H * 0.8, 1000);
  moveXY_DDA(-LETTER_W / 2, -LETTER_H * 0.2, 1000);
  moveXY_DDA(-LETTER_W / 2, LETTER_H * 0.2, 1000);
  penUp();
  moveXY_DDA(LETTER_W + SPACE_W, 0, 1000);
}

void drawK()
{
  penDown();
  moveXY_DDA(0, LETTER_H, 1000);
  penUp();
  moveXY_DDA(LETTER_W / 2, -LETTER_H, 1000);
  penDown();
  moveXY_DDA(-LETTER_W / 2, LETTER_H / 2, 1000);
  moveXY_DDA(LETTER_W / 2, LETTER_H / 2, 1000);
  penUp();
  moveXY_DDA(SPACE_W, -LETTER_H, 1000);
}

void drawL()
{
  moveXY_DDA(0, LETTER_H, 1000);
  penDown();
  moveXY_DDA(0, -LETTER_H, 1000);
  moveXY_DDA(LETTER_W / 2, 0, 1000);
  penUp();
  moveXY_DDA(SPACE_W, 0, 1000);
}

void drawM()
{
  penDown();
  moveXY_DDA(0, LETTER_H, 1000);
  moveXY_DDA(LETTER_W / 2, -LETTER_H / 2, 1000);
  moveXY_DDA(LETTER_W / 2, LETTER_H / 2, 1000);
  moveXY_DDA(0, -LETTER_H, 1000);
  penUp();
  moveXY_DDA(SPACE_W, 0, 1000);
}

void drawN()
{
  penDown();
  moveXY_DDA(0, LETTER_H, 1000);
  moveXY_DDA(LETTER_W, -LETTER_H, 1000);
  moveXY_DDA(0, LETTER_H, 1000);
  penUp();
  moveXY_DDA(SPACE_W, -LETTER_H, 1000);
}

void drawO()
{
  float r = LETTER_H / 2;

  penUp();
  moveXY_DDA(r, r, 800);

  penDown();
  drawArc(r, 0, 2 * PI, 120, 800);

  penUp();
  moveXY_DDA(r + SPACE_W, -r, 1000);
}

void drawP()
{
  float r = LETTER_H / 4;

  penDown();
  moveXY_DDA(0, LETTER_H, 800);

  moveXY_DDA(LETTER_W / 8, 0, 1000);
  drawArc(r, PI / 2, -PI / 2, 30, 1000);
  moveXY_DDA(-LETTER_W / 8, 0, 1000);

  penUp();
  moveXY_DDA(LETTER_W / 2 + SPACE_W, -LETTER_H / 2, 1000);
}

void drawQ()
{
  float r = LETTER_H / 2;

  moveXY_DDA(0, LETTER_H / 2, 1000);

  penUp();
  moveXY_DDA(r, 0, 800);

  penDown();
  drawArc(r, 0, 2 * PI, 120, 800);
  penUp();
  moveXY_DDA(-r / 3, -r / 2, 1000);
  penDown();
  moveXY_DDA(r / 3, -r / 3, 800);

  penUp();
  moveXY_DDA(r + SPACE_W, r / 3, 1000);
}

void drawR()
{
  drawP();
  moveXY_DDA(-(LETTER_W / 2 + SPACE_W), LETTER_H / 1.95, 1000);
  penDown();
  moveXY_DDA(LETTER_W / 2, -LETTER_H / 2, 1000);
  penUp();
  delay(100);
  moveXY_DDA(SPACE_W, 0, 1000);
}

void drawS()
{
  float r = LETTER_H / 4;

  penUp();
  moveXY_DDA(r + LETTER_W / 5, LETTER_H, 800);

  penDown();
  moveXY_DDA(-LETTER_W / 5, 0, 1000);
  penDown();
  drawArc(r, PI / 2, 3 * PI / 2, 30, 1000);
  drawArc(r, PI / 2, -PI / 2, 30, 1000);
  moveXY_DDA(-LETTER_W / 5, 0, 1000);
  penUp();
  moveXY_DDA(LETTER_W / 5 + r + SPACE_W, 0, 1000);
}

void drawT()
{
  moveXY_DDA(0, LETTER_H, 1000);
  penDown();
  moveXY_DDA(LETTER_W, 0, 1000);
  penUp();
  moveXY_DDA(-LETTER_W / 2, 0, 1000);
  penDown();
  moveXY_DDA(0, -LETTER_H, 1000);
  penUp();
  moveXY_DDA(LETTER_W / 2 + SPACE_W, 0, 1000);
}
void drawU()
{
  float r = LETTER_W / 2;

  moveXY_DDA(0, LETTER_H, 1000);

  penDown();
  moveXY_DDA(0, -(LETTER_H - r), 800);

  drawArc(r, -PI, 0, 30, 800);

  moveXY_DDA(0, LETTER_H - r, 800);

  penUp();
  moveXY_DDA(SPACE_W, -LETTER_H, 1000);
}

void drawV()
{
  moveXY_DDA(0, LETTER_H, 1000);
  penDown();
  moveXY_DDA(LETTER_W / 2, -LETTER_H, 1000);
  moveXY_DDA(LETTER_W / 2, LETTER_H, 1000);
  penUp();
  moveXY_DDA(SPACE_W, -LETTER_H, 1000);
}

void drawW()
{
  moveXY_DDA(0, LETTER_H, 1000);
  penDown();
  moveXY_DDA(LETTER_W / 4, -LETTER_H, 1000);
  moveXY_DDA(LETTER_W / 4, LETTER_H / 2, 1000);
  moveXY_DDA(LETTER_W / 4, -LETTER_H / 2, 1000);
  moveXY_DDA(LETTER_W / 4, LETTER_H, 1000);
  penUp();
  moveXY_DDA(SPACE_W, -LETTER_H, 1000);
}

void drawX()
{
  penDown();
  moveXY_DDA(LETTER_W, LETTER_H - LETTER_H / 4, 1000);
  penUp();
  moveXY_DDA(-LETTER_W, 0, 1000);
  penDown();
  moveXY_DDA(LETTER_W, -LETTER_H + LETTER_H / 4, 1000);
  penUp();
  delay(100);
  moveXY_DDA(SPACE_W, 0, 1000);
}

void drawY()
{
  penUp();
  moveXY_DDA(0, LETTER_H, 1000);
  penDown();
  moveXY_DDA(LETTER_W / 2, -LETTER_H / 2, 1000);
  moveXY_DDA(LETTER_W / 2, LETTER_H / 2, 1000);
  penUp();
  moveXY_DDA(-LETTER_W / 2, -LETTER_H / 2, 1000);
  penDown();
  moveXY_DDA(0, -LETTER_H / 2, 1000);
  penUp();
  moveXY_DDA(LETTER_W / 2 + SPACE_W, 0, 1000);
}

void drawZ()
{
  moveXY_DDA(LETTER_W, 0, 1000);
  penDown();
  moveXY_DDA(-LETTER_W, 0, 1000);
  moveXY_DDA(LETTER_W, LETTER_H, 1000);
  moveXY_DDA(-LETTER_W, 0, 1000);
  penUp();
  moveXY_DDA(SPACE_W, -LETTER_H, 1000);
}