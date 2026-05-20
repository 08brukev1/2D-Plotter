#include <Arduino.h>

#if !defined(LOWERCASELETTERS_H)
#define LOWERCASELETTERS_H

extern void penUp();
extern void moveXY_DDA(float x_cm, float y_cm, int stepDelayUs);
extern void penDown();
extern void drawArc(float radius_cm, float startAngle, float endAngle, int segments, int stepDelayUs);

void drawa()
{
  float r = LETTER_H * 0.25;
  switch (letter_state)
  {
  case 0:
    penUp();
    break;
  case 1:
    moveXY_DDA(LETTER_W / 2, 0, variableStepDelayUs);
    break;
  case 2:
    penDown();
    break;
  case 3:
    moveXY_DDA(0, LETTER_H * 0.6, variableStepDelayUs);
    break;
  case 4:
    penUp();
    break;
  case 5:
    moveXY_DDA(0, -LETTER_H * 0.3, variableStepDelayUs);
    break;
  case 6:
    penDown();
    break;
  case 7:
    drawArc(r, 0, 2 * PI, 10, variableStepDelayUs);
    break;
  case 8:
    penUp();
    break;
  case 9:
    state = RESET_POSITION;
    letter_state = 0;
    servo_state = 0;
    break;
  }
}

void drawb()
{
  float r = LETTER_H / 4;
  switch (letter_state)
  {
  case 0:
    penDown();
    break;
  case 1:
    moveXY_DDA(0, LETTER_H, variableStepDelayUs);
    break;
  case 2:
    penUp();
    break;
  case 3:
    moveXY_DDA(0, -LETTER_H / 2, variableStepDelayUs);
    break;
  case 4:
    penDown();
    break;
  case 5:
    moveXY_DDA(LETTER_W / 6, 0, variableStepDelayUs);
    break;
  case 6:
    drawArc(r, PI / 2, -PI / 2, 5, variableStepDelayUs);
    break;
  case 7:
    moveXY_DDA(-LETTER_W / 6, 0, variableStepDelayUs);
    break;
  case 8:
    moveXY_DDA(0, LETTER_H / 8, variableStepDelayUs);
    break;
  case 9:
    penUp();
    break;
  case 10:
    state = RESET_POSITION;
    letter_state = 0;
    servo_state = 0;
    break;
  }
}

void drawc()
{
  float r = LETTER_H * 0.3;
  switch (letter_state)
  {
  case 0:
    penUp();
    break;
  case 1:
    moveXY_DDA(r * 2.2, LETTER_H * 0.5, variableStepDelayUs);
    break;
  case 2:
    penDown();
    break;
  case 3:
    drawArc(r, PI * 0.3, PI * 1.7, 5, variableStepDelayUs);
    break;
  case 4:
    penUp();
    break;
  case 5:
    state = RESET_POSITION;
    letter_state = 0;
    servo_state = 0;
    break;
  }
}

void drawd()
{
  float r = LETTER_H * 0.25;
  switch (letter_state)
  {
  case 0:
    penUp();
    break;
  case 1:
    moveXY_DDA(r * 2, LETTER_H, variableStepDelayUs);
    break;
  case 2:
    penDown();
    break;
  case 3:
    moveXY_DDA(0, -LETTER_H, variableStepDelayUs);
    break;
  case 4:
    moveXY_DDA(-LETTER_W * 0.2, 0, variableStepDelayUs);
    break;
  case 5:
    drawArc(r, (PI * 3) / 2, PI / 2, 5, variableStepDelayUs);
    break;
  case 6:
    moveXY_DDA(LETTER_W * 0.2, 0, variableStepDelayUs);
    break;
  case 7:
    penUp();
    break;
  case 8:
    state = RESET_POSITION;
    letter_state = 0;
    servo_state = 0;
    break;
  }
}

void drawe()
{
  float r = LETTER_H * 0.3;
  switch (letter_state)
  {
  case 0:
    penUp();
    break;
  case 1:
    moveXY_DDA(0, LETTER_H * 0.35, variableStepDelayUs);
    break;
  case 2:
    penDown();
    break;
  case 3:
    moveXY_DDA(r * 2, 0, variableStepDelayUs);
    break;
  case 4:
    drawArc(r, 0, PI * 1.8, 10, variableStepDelayUs);
    break;
  case 5:
    penUp();
    break;
  case 6:
    state = RESET_POSITION;
    letter_state = 0;
    servo_state = 0;
    break;
  }
}

void drawf()
{
  float r = LETTER_H * 0.2;
  switch (letter_state)
  {
  case 0:
    penUp();
    break;
  case 1:
    moveXY_DDA(r * 1.5, LETTER_H, variableStepDelayUs);
    break;
  case 2:
    penDown();
    break;
  case 3:
    drawArc(r, PI / 2, PI, 5, variableStepDelayUs);
    break;
  case 4:
    moveXY_DDA(0, -LETTER_H * 0.8, variableStepDelayUs);
    break;
  case 5:
    penUp();
    break;
  case 6:
    moveXY_DDA(-r * 0.5, LETTER_H * 0.5, variableStepDelayUs);
    break;
  case 7:
    penDown();
    break;
  case 8:
    moveXY_DDA(r * 1.5, 0, variableStepDelayUs);
    break;
  case 9:
    penUp();
    break;
  case 10:
    state = RESET_POSITION;
    letter_state = 0;
    servo_state = 0;
    break;
  }
}

void drawg()
{
  float r = LETTER_H * 0.2;
  switch (letter_state)
  {
  case 0:
    penUp();
    break;
  case 1:
    moveXY_DDA(0, -LETTER_H * 0.2, variableStepDelayUs);
    break;
  case 2:
    penDown();
    break;
  case 3:
    drawArc(r, PI, 2 * PI, 10, variableStepDelayUs);
    break;
  case 4:
    moveXY_DDA(0, LETTER_H * 0.7, variableStepDelayUs);
    break;
  case 5:
    penUp();
    break;
  case 6:
    moveXY_DDA(0, -LETTER_H * 0.2, variableStepDelayUs);
    break;
  case 7:
    penDown();
    break;
  case 8:
    drawArc(r, 0, 2 * PI, 15, variableStepDelayUs);
    break;
  case 9:
    moveXY_DDA(0, LETTER_H / 4, variableStepDelayUs);
    break;
  case 10:
    penUp();
    break;
  case 11:
    state = RESET_POSITION;
    letter_state = 0;
    servo_state = 0;
    break;
  }
}

void drawh()
{
  float r = LETTER_H * 0.2;
  switch (letter_state)
  {
  case 0:
    penDown();
    break;
  case 1:
    moveXY_DDA(0, LETTER_H, variableStepDelayUs);
    break;
  case 2:
    penUp();
    break;
  case 3:
    moveXY_DDA(0, -LETTER_H / 1.5, variableStepDelayUs);
    break;
  case 4:
    penDown();
    break;
  case 5:
    drawArc(r, PI, 0, 10, variableStepDelayUs);
    break;
  case 6:
    moveXY_DDA(0, -(LETTER_H - LETTER_H / 1.5), variableStepDelayUs);
    break;
  case 7:
    penUp();
    break;
  case 8:
    state = RESET_POSITION;
    letter_state = 0;
    servo_state = 0;
    break;
  }
}

void drawi()
{
  switch (letter_state)
  {
  case 0:
    penDown();
    break;
  case 1:
    moveXY_DDA(0, LETTER_H * 0.5, variableStepDelayUs);
    break;
  case 2:
    penUp();
    break;
  case 3:
    moveXY_DDA(0, LETTER_H / 4, variableStepDelayUs);
    break;
  case 4:
    penDown();
    break;
  case 5:
    moveXY_DDA(0, LETTER_H * 0.05, variableStepDelayUs);
    break;
  case 6:
    penUp();
    break;
  case 7:
    state = RESET_POSITION;
    letter_state = 0;
    servo_state = 0;
    break;
  }
}

void drawj()
{
  float r = LETTER_H * 0.15;
  switch (letter_state)
  {
  case 0:
    penDown();
    break;
  case 1:
    drawArc(r, PI * 1.4, 2 * PI, 10, variableStepDelayUs);
    break;
  case 2:
    moveXY_DDA(0, LETTER_H * 0.5, variableStepDelayUs);
    break;
  case 3:
    penUp();
    break;
  case 4:
    moveXY_DDA(0, LETTER_H / 4, variableStepDelayUs);
    break;
  case 5:
    penDown();
    break;
  case 6:
    moveXY_DDA(0, LETTER_H * 0.05, variableStepDelayUs);
    break;
  case 7:
    penUp();
    break;
  case 8:
    state = RESET_POSITION;
    letter_state = 0;
    servo_state = 0;
    break;
  }
}

void drawk()
{
  switch (letter_state)
  {
  case 0:
    penDown();
    break;
  case 1:
    moveXY_DDA(0, LETTER_H, variableStepDelayUs);
    break;
  case 2:
    penUp();
    break;
  case 3:
    moveXY_DDA(LETTER_W * 0.3, -LETTER_H * 0.5, variableStepDelayUs);
    break;
  case 4:
    penDown();
    break;
  case 5:
    moveXY_DDA(-LETTER_W * 0.3, -LETTER_H * 0.25, variableStepDelayUs);
    break;
  case 6:
    moveXY_DDA(LETTER_W * 0.3, -LETTER_H * 0.25, variableStepDelayUs);
    break;
  case 7:
    penUp();
    break;
  case 8:
    state = RESET_POSITION;
    letter_state = 0;
    servo_state = 0;
    break;
  }
}

void drawl()
{
  switch (letter_state)
  {
  case 0:
    penUp();
    break;
  case 1:
    moveXY_DDA(0, LETTER_H, variableStepDelayUs);
    break;
  case 2:
    penDown();
    break;
  case 3:
    moveXY_DDA(0, -LETTER_H, variableStepDelayUs);
    break;
  case 4:
    moveXY_DDA(LETTER_W * 0.08, 0, variableStepDelayUs);
    break;
  case 5:
    penUp();
    break;
  case 6:
    state = RESET_POSITION;
    letter_state = 0;
    servo_state = 0;
    break;
  }
}

void drawm()
{
  float r = LETTER_H * 0.2;

  switch (letter_state)
  {
  case 0:
    penDown();
    break;
  case 1:
    moveXY_DDA(0, LETTER_H * 0.7, variableStepDelayUs);
    break;
  case 2:
    penUp();
    break;

  case 3:
    moveXY_DDA(0, -LETTER_H * 0.25, variableStepDelayUs);
    break;
  case 4:
    penDown();
    break;
  case 5:
    drawArc(r, PI, PI * 0.1, 10, variableStepDelayUs);
    break;
  case 6:
    moveXY_DDA(0, -(LETTER_H * 0.7 - LETTER_H * 0.25), variableStepDelayUs);
    break;
  case 7:
    penUp();
    break;

  case 8:
    moveXY_DDA(0, LETTER_H * 0.6 - LETTER_H * 0.25, variableStepDelayUs);
    break;
  case 9:
    penDown();
    break;
  case 10:
    drawArc(r, PI, PI * 0.1, 10, variableStepDelayUs);
    break;
  case 11:
    moveXY_DDA(0, -(LETTER_H * 0.7 - LETTER_H * 0.25), variableStepDelayUs);
    break;
  case 12:
    penUp();
    break;

  case 13:
    state = RESET_POSITION;
    letter_state = 0;
    servo_state = 0;
    break;
  }
}

void drawn()
{
  float r = LETTER_H * 0.2;

  switch (letter_state)
  {
  case 0:
    penDown();
    break;
  case 1:
    moveXY_DDA(0, LETTER_H * 0.7, variableStepDelayUs);
    break;
  case 2:
    penUp();
    break;
  case 3:
    moveXY_DDA(0, -LETTER_H * 0.25, variableStepDelayUs);
    break;
  case 4:
    penDown();
    break;
  case 5:
    drawArc(r, PI, PI * 0.1, 5, variableStepDelayUs);
    break;
  case 6:
    moveXY_DDA(0, -(LETTER_H * 0.7 - LETTER_H * 0.25), variableStepDelayUs);
    break;
  case 7:
    penUp();
    break;
  case 8:
    state = RESET_POSITION;
    letter_state = 0;
    servo_state = 0;
    break;
  }
}

void drawo()
{
  float r = LETTER_H * 0.3;

  switch (letter_state)
  {
  case 0:
    penUp();
    break;
  case 1:
    moveXY_DDA(2 * r, r, variableStepDelayUs);
    break;
  case 2:
    penDown();
    break;
  case 3:
    drawArc(r, 0, 2 * PI, 20, variableStepDelayUs);
    break;
  case 4:
    penUp();
    break;
  case 5:
    state = RESET_POSITION;
    letter_state = 0;
    servo_state = 0;
    break;
  }
}

void drawp()
{
  float r = LETTER_H / 4;

  switch (letter_state)
  {
  case 0:
    penDown();
    break;
  case 1:
    moveXY_DDA(LETTER_W / 6, 0, variableStepDelayUs);
    break;
  case 2:
    drawArc(r, -PI / 2, PI / 2, 5, variableStepDelayUs);
    break;
  case 3:
    moveXY_DDA(-LETTER_W / 6, 0, variableStepDelayUs);
    break;
  case 4:
    moveXY_DDA(0, -LETTER_H, variableStepDelayUs);
    break;
  case 5:
    moveXY_DDA(0, LETTER_H - 2 * r, variableStepDelayUs);
    break;
  case 6:
    penUp();
    break;
  case 7:
    state = RESET_POSITION;
    letter_state = 0;
    servo_state = 0;
    break;
  }
}

void drawq()
{
  float r = LETTER_H / 4;

  switch (letter_state)
  {
  case 0:
    penUp();
    break;
  case 1:
    moveXY_DDA(SPACE_W * 1.5, 0, variableStepDelayUs);
    break;
  case 2:
    penDown();
    break;
  case 3:
    moveXY_DDA(-LETTER_W / 6, 0, variableStepDelayUs);
    break;
  case 4:
    drawArc(r, (PI * 3) / 2, PI / 2, 5, variableStepDelayUs);
    break;
  case 5:
    moveXY_DDA(LETTER_W / 6, 0, variableStepDelayUs);
    break;
  case 6:
    moveXY_DDA(0, -LETTER_H, variableStepDelayUs);
    break;
  case 7:
    moveXY_DDA(0, LETTER_H - 2 * r, variableStepDelayUs);
    break;
  case 8:
    penUp();
    break;
  case 9:
    state = RESET_POSITION;
    letter_state = 0;
    servo_state = 0;
    break;
  }
}

void drawr()
{
  float r = LETTER_H * 0.2;

  switch (letter_state)
  {
  case 0:
    penDown();
    break;
  case 1:
    moveXY_DDA(0, LETTER_H * 0.7, variableStepDelayUs);
    break;
  case 2:
    penUp();
    break;
  case 3:
    moveXY_DDA(0, -LETTER_H * 0.25, variableStepDelayUs);
    break;
  case 4:
    penDown();
    break;
  case 5:
    drawArc(r, PI, PI * 0.1, 5, variableStepDelayUs);
    break;
  case 6:
    penUp();
    break;
  case 7:
    state = RESET_POSITION;
    letter_state = 0;
    servo_state = 0;
    break;
  }
}

void draws()
{
  float r = LETTER_H / 7;

  switch (letter_state)
  {
  case 0:
    penDown();
    break;
  case 1:
    moveXY_DDA(LETTER_W / 6, 0, variableStepDelayUs);
    break;
  case 2:
    drawArc(r, -PI / 2, PI / 2, 5, variableStepDelayUs);
    break;
  case 3:
    drawArc(r, (PI * 3) / 2, PI / 2, 5, variableStepDelayUs);
    break;
  case 4:
    moveXY_DDA(LETTER_W / 6, 0, variableStepDelayUs);
    break;
  case 5:
    penUp();
    break;
  case 6:
    state = RESET_POSITION;
    letter_state = 0;
    servo_state = 0;
    break;
  }
}

void drawt()
{
  float r = LETTER_H * 0.1;

  switch (letter_state)
  {
  case 0:
    penUp();
    break;
  case 1:
    moveXY_DDA(LETTER_W * 0.1, LETTER_H, variableStepDelayUs);
    break;
  case 2:
    penDown();
    break;
  case 3:
    moveXY_DDA(0, -(LETTER_H - r), variableStepDelayUs);
    break;
  case 4:
    drawArc(r, PI, 2 * PI, 5, variableStepDelayUs);
    break;
  case 5:
    penUp();
    break;
  case 6:
    moveXY_DDA(-LETTER_W * 0.3 - r, LETTER_H * 0.65, variableStepDelayUs);
    break;
  case 7:
    penDown();
    break;
  case 8:
    moveXY_DDA(LETTER_W * 0.5, 0, variableStepDelayUs);
    break;
  case 9:
    penUp();
    break;
  case 10:
    state = RESET_POSITION;
    letter_state = 0;
    servo_state = 0;
    break;
  }
}

void drawu()
{
  float r = LETTER_H * 0.25;

  switch (letter_state)
  {
  case 0:
    penUp();
    break;
  case 1:
    moveXY_DDA(0, LETTER_H * 0.6, variableStepDelayUs);
    break;
  case 2:
    penDown();
    break;
  case 3:
    moveXY_DDA(0, -(LETTER_H * 0.6 - r), variableStepDelayUs);
    break;
  case 4:
    drawArc(r, PI, 2 * PI, 5, variableStepDelayUs);
    break;
  case 5:
    moveXY_DDA(0, LETTER_H * 0.6 - r, variableStepDelayUs);
    break;
  case 6:
    penUp();
    break;
  case 7:
    moveXY_DDA(0, -(LETTER_H * 0.6 - r), variableStepDelayUs);
    break;
  case 8:
    penDown();
    break;
  case 9:
    moveXY_DDA(0, -r, variableStepDelayUs);
    break;
  case 10:
    penUp();
    break;
  case 11:
    state = RESET_POSITION;
    letter_state = 0;
    servo_state = 0;
    break;
  }
}

void drawv()
{
  switch (letter_state)
  {
  case 0:
    penUp();
    break;
  case 1:
    moveXY_DDA(0, LETTER_H * 0.65, variableStepDelayUs);
    break;
  case 2:
    penDown();
    break;
  case 3:
    moveXY_DDA(LETTER_W * 0.4, -LETTER_H * 0.61, variableStepDelayUs);
    break;
  case 4:
    moveXY_DDA(LETTER_W * 0.4, LETTER_H * 0.61, variableStepDelayUs);
    break;
  case 5:
    penUp();
    break;
  case 6:
    state = RESET_POSITION;
    letter_state = 0;
    servo_state = 0;
    break;
  }
}

void draww()
{
  switch (letter_state)
  {
  case 0:
    penUp();
    break;
  case 1:
    moveXY_DDA(0, LETTER_H * 0.55, variableStepDelayUs);
    break;
  case 2:
    penDown();
    break;
  case 3:
    moveXY_DDA(LETTER_W / 4, -LETTER_H * 0.55, variableStepDelayUs);
    break;
  case 4:
    moveXY_DDA(LETTER_W / 4, LETTER_H * 0.35, variableStepDelayUs);
    break;
  case 5:
    moveXY_DDA(LETTER_W / 4, -LETTER_H * 0.35, variableStepDelayUs);
    break;
  case 6:
    moveXY_DDA(LETTER_W / 4, LETTER_H * 0.55, variableStepDelayUs);
    break;
  case 7:
    penUp();
    break;
  case 8:
    state = RESET_POSITION;
    letter_state = 0;
    servo_state = 0;
    break;
  }
}

void drawx()
{
  switch (letter_state)
  {
  case 0:
    penUp();
    break;
  case 1:
    moveXY_DDA(0, LETTER_H * 0.5, variableStepDelayUs);
    break;
  case 2:
    penDown();
    break;
  case 3:
    moveXY_DDA(LETTER_W * 0.6, -LETTER_H * 0.5, variableStepDelayUs);
    break;
  case 4:
    penUp();
    break;
  case 5:
    moveXY_DDA(-LETTER_W * 0.6, 0, variableStepDelayUs);
    break;
  case 6:
    penDown();
    break;
  case 7:
    moveXY_DDA(LETTER_W * 0.6, LETTER_H * 0.5, variableStepDelayUs);
    break;
  case 8:
    penUp();
    break;
  case 9:
    state = RESET_POSITION;
    letter_state = 0;
    servo_state = 0;
    break;
  }
}

void drawy()
{
  switch (letter_state)
  {
  case 0:
    penUp();
    break;
  case 1:
    moveXY_DDA(LETTER_W / 3, -LETTER_H / 2, variableStepDelayUs);
    break;
  case 2:
    penDown();
    break;
  case 3:
    moveXY_DDA(LETTER_W / 3, LETTER_H, variableStepDelayUs);
    break;
  case 4:
    penUp();
    break;
  case 5:
    moveXY_DDA(-LETTER_W / 2, 0, variableStepDelayUs);
    break;
  case 6:
    penDown();
    break;
  case 7:
    moveXY_DDA(LETTER_W / 3, -LETTER_H / 2, variableStepDelayUs);
    break;
  case 8:
    penUp();
    break;
  case 9:
    state = RESET_POSITION;
    letter_state = 0;
    servo_state = 0;
    break;
  }
}

void drawz()
{
  switch (letter_state)
  {
  case 0:
    penUp();
    break;
  case 1:
    moveXY_DDA(0, LETTER_H * 0.65, variableStepDelayUs);
    break;
  case 2:
    penDown();
    break;
  case 3:
    moveXY_DDA(LETTER_W * 0.7, 0, variableStepDelayUs);
    break;
  case 4:
    moveXY_DDA(-LETTER_W * 0.7, -LETTER_H * 0.55, variableStepDelayUs);
    break;
  case 5:
    moveXY_DDA(LETTER_W * 0.7, 0, variableStepDelayUs);
    break;
  case 6:
    penUp();
    break;
  case 7:
    state = RESET_POSITION;
    letter_state = 0;
    servo_state = 0;
    break;
  }
}


#endif // LOWERCASELETTERS_H
