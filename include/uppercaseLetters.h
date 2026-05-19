#if !defined(UPPERCASELETTERS_H)
#define UPPERCASELETTERS_H

extern void penUp();
extern void moveXY_DDA(float x_cm, float y_cm, int stepDelayUs);
extern void penDown();
extern void drawArc(float radius_cm, float startAngle, float endAngle, int segments, int stepDelayUs);

void drawA()
{
  switch (letter_state)
  {
  case 0:
    penDown();
    break;
  case 1:
    moveXY_DDA(LETTER_W / 4, LETTER_H, variableStepDelayUs);
    break;
  case 2:
    moveXY_DDA(LETTER_W / 4, -LETTER_H, variableStepDelayUs);
    break;
  case 3:
    penUp();
    break;
  case 4:
    moveXY_DDA(-LETTER_W / 2.5, LETTER_H / 2, variableStepDelayUs);
    break;
  case 5:
    penDown();
    break;
  case 6:
    moveXY_DDA(LETTER_W / 4, 0, variableStepDelayUs);
    break;
  case 7:
    penUp();
    break;
  case 8:
    state = RESET_POSITION;
    letter_state = 0;
    ServoState = 0;
    break;
  }
}

void drawB()
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
    moveXY_DDA(LETTER_W / 8, 0, variableStepDelayUs);
    break;
  case 3:
    drawArc(r, PI / 2, -PI / 2, 5, variableStepDelayUs);
    break;
  case 4:
    moveXY_DDA(-LETTER_W / 8, 0, variableStepDelayUs);
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
    penUp();
    break;
  case 9:
    state = RESET_POSITION;
    letter_state = 0;
    ServoState = 0;
    break;
  }
}

void drawC()
{
  float r = LETTER_H / 2;

  switch (letter_state)
  {
  case 0:
    penUp();
    break;
  case 1:
    moveXY_DDA(r + LETTER_W / 6, 0, variableStepDelayUs);
    break;
  case 2:
    penDown();
    break;
  case 3:
    moveXY_DDA(-LETTER_W / 6, 0, variableStepDelayUs);
    break;
  case 4:
    drawArc(r, PI * 3 / 2, PI / 2, 7, variableStepDelayUs);
    break;
  case 5:
    moveXY_DDA(LETTER_W / 6, 0, variableStepDelayUs);
    break;
  case 6:
    penUp();
    break;
  case 7:
    state = RESET_POSITION;
    letter_state = 0;
    ServoState = 0;
    break;
  }
}

void drawD()
{
  float r = LETTER_H / 2;

  switch (letter_state)
  {
  case 0:
    penDown();
    break;
  case 1:
    moveXY_DDA(0, LETTER_H, variableStepDelayUs);
    break;
  case 2:
    drawArc(r, PI / 2, -PI / 2, 10, variableStepDelayUs);
    break;
  case 3:
    penUp();
    break;
  case 4:
    state = RESET_POSITION;
    letter_state = 0;
    ServoState = 0;
    break;
  }
}

void drawE()
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
    moveXY_DDA(LETTER_W / 1.5, 0, variableStepDelayUs);
    break;
  case 3:
    penUp();
    break;
  case 4:
    moveXY_DDA(-LETTER_W / 1.5, -LETTER_H / 2, variableStepDelayUs);
    break;
  case 5:
    penDown();
    break;
  case 6:
    moveXY_DDA(LETTER_W / 2, 0, variableStepDelayUs);
    break;
  case 7:
    penUp();
    break;
  case 8:
    moveXY_DDA(-LETTER_W / 2, -LETTER_H / 2, variableStepDelayUs);
    break;
  case 9:
    penDown();
    break;
  case 10:
    moveXY_DDA(LETTER_W / 1.5, 0, variableStepDelayUs);
    break;
  case 11:
    penUp();
    break;
  case 12:
    state = RESET_POSITION;
    letter_state = 0;
    ServoState = 0;
    break;
  }
}

void drawF()
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
    moveXY_DDA(LETTER_W, 0, variableStepDelayUs);
    break;
  case 3:
    penUp();
    break;
  case 4:
    moveXY_DDA(-LETTER_W, -LETTER_H / 2, variableStepDelayUs);
    break;
  case 5:
    penDown();
    break;
  case 6:
    moveXY_DDA(LETTER_W * 0.7, 0, variableStepDelayUs);
    break;
  case 7:
    penUp();
    break;
  case 8:
    state = RESET_POSITION;
    letter_state = 0;
    ServoState = 0;
    break;
  }
}

void drawG()
{
  float r = LETTER_H / 2;

  switch (letter_state)
  {
  case 0:
    penUp();
    break;
  case 1:
    moveXY_DDA(LETTER_W, r, variableStepDelayUs);
    break;
  case 2:
    penDown();
    break;
  case 3:
    drawArc(r, 0, 2 * PI - 0.3, 10, variableStepDelayUs);
    break;
  case 4:
    moveXY_DDA(-r / 2, 0, variableStepDelayUs);
    break;
  case 5:
    penUp();
    break;
  case 6:
    moveXY_DDA(r / 3, 0, variableStepDelayUs);
    break;
  case 7:
    penDown();
    break;
  case 8:
    moveXY_DDA(0, -LETTER_H / 3, variableStepDelayUs);
    break;
  case 9:
    penUp();
    break;
  case 10:
    state = RESET_POSITION;
    letter_state = 0;
    ServoState = 0;
    break;
  }
}

void drawH()
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
    moveXY_DDA(LETTER_W / 2, 0, variableStepDelayUs);
    break;
  case 4:
    penDown();
    break;
  case 5:
    moveXY_DDA(0, -LETTER_H, variableStepDelayUs);
    break;
  case 6:
    penUp();
    break;
  case 7:
    moveXY_DDA(-LETTER_W / 2, LETTER_H / 2, variableStepDelayUs);
    break;
  case 8:
    penDown();
    break;
  case 9:
    moveXY_DDA(LETTER_W / 2, 0, variableStepDelayUs);
    break;
  case 10:
    penUp();
    break;
  case 11:
    state = RESET_POSITION;
    letter_state = 0;
    ServoState = 0;
    break;
  }
}

void drawI()
{
  switch (letter_state)
  {
  case 0:
    penDown();
    break;
  case 1:
    moveXY_DDA(LETTER_W / 2, 0, variableStepDelayUs);
    break;
  case 2:
    moveXY_DDA(-LETTER_W / 4, 0, variableStepDelayUs);
    break;
  case 3:
    moveXY_DDA(0, LETTER_H, variableStepDelayUs);
    break;
  case 4:
    penUp();
    break;
  case 5:
    moveXY_DDA(-LETTER_W / 4, 0, variableStepDelayUs);
    break;
  case 6:
    penDown();
    break;
  case 7:
    moveXY_DDA(LETTER_W / 2, 0, variableStepDelayUs);
    break;
  case 8:
    penUp();
    break;
  case 9:
    state = RESET_POSITION;
    letter_state = 0;
    ServoState = 0;
    break;
  }
}

void drawJ()
{
  float r = LETTER_H * 0.2;

  switch (letter_state)
  {
  case 0:
    moveXY_DDA(0, r, variableStepDelayUs);
    break;
  case 1:
    penDown();
    break;
  case 2:
    drawArc(r, PI, 2 * PI, 10, variableStepDelayUs);
    break;
  case 3:
    moveXY_DDA(0, LETTER_H * 0.7, variableStepDelayUs);
    break;
  case 4:
    penUp();
    break;
  case 5:
    state = RESET_POSITION;
    letter_state = 0;
    ServoState = 0;
    break;
  }
}

void drawK()
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
    moveXY_DDA(LETTER_W / 2, -LETTER_H, variableStepDelayUs);
    break;
  case 4:
    penDown();
    break;
  case 5:
    moveXY_DDA(-LETTER_W / 2, LETTER_H / 2, variableStepDelayUs);
    break;
  case 6:
    moveXY_DDA(LETTER_W / 2, LETTER_H / 2, variableStepDelayUs);
    break;
  case 7:
    penUp();
    break;
  case 8:
    state = RESET_POSITION;
    letter_state = 0;
    ServoState = 0;
    break;
  }
}

void drawL()
{
  switch (letter_state)
  {
  case 0:
    moveXY_DDA(0, LETTER_H, variableStepDelayUs);
    break;
  case 1:
    penDown();
    break;
  case 2:
    moveXY_DDA(0, -LETTER_H, variableStepDelayUs);
    break;
  case 3:
    moveXY_DDA(LETTER_W / 1.5, 0, variableStepDelayUs);
    break;
  case 4:
    penUp();
    break;
  case 5:
    state = RESET_POSITION;
    letter_state = 0;
    ServoState = 0;
    break;
  }
}

void drawM()
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
    moveXY_DDA(LETTER_W / 2, -LETTER_H / 2, variableStepDelayUs);
    break;
  case 3:
    moveXY_DDA(LETTER_W / 2, LETTER_H / 2, variableStepDelayUs);
    break;
  case 4:
    moveXY_DDA(0, -LETTER_H, variableStepDelayUs);
    break;
  case 5:
    penUp();
    break;
  case 6:
    state = RESET_POSITION;
    letter_state = 0;
    ServoState = 0;
    break;
  }
}

void drawN()
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
    moveXY_DDA(LETTER_W, -LETTER_H, variableStepDelayUs);
    break;
  case 3:
    moveXY_DDA(0, LETTER_H, variableStepDelayUs);
    break;
  case 4:
    penUp();
    break;
  case 5:
    state = RESET_POSITION;
    letter_state = 0;
    ServoState = 0;
    break;
  }
}

void drawO()
{
  float r = LETTER_H / 2;
  switch (letter_state)
  {
  case 0:

    penUp();
    break;

  case 1:
    moveXY_DDA(LETTER_W * 1.5, r, variableStepDelayUs);
    break;
  case 2:
    penDown();
    break;
  case 3:
    drawArc(r, 0, 2 * PI, 15, variableStepDelayUs);
    break;
  case 4:
    penUp();
    break;
  case 5:
    state = RESET_POSITION;
    letter_state = 0;
    ServoState = 0;
    break;
  }
}

void drawP()
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
    moveXY_DDA(LETTER_W / 8, 0, variableStepDelayUs);
    break;
  case 3:
    drawArc(r, PI / 2, -PI / 2, 10, variableStepDelayUs);
    break;
  case 4:
    moveXY_DDA(-LETTER_W / 8, 0, variableStepDelayUs);
    break;
  case 5:
    penUp();
    break;
  case 6:
    state = RESET_POSITION;
    letter_state = 0;
    ServoState = 0;
    break;
  }
}

void drawQ()
{
  float r = LETTER_H / 2;

  switch (letter_state)
  {
  case 0:
    moveXY_DDA(0, LETTER_H / 2, variableStepDelayUs);
    break;
  case 1:
    penUp();
    break;
  case 2:
    moveXY_DDA(LETTER_W, 0, variableStepDelayUs);
    break;
  case 3:
    penDown();
    break;
  case 4:
    drawArc(r, 0, 2 * PI, 15, variableStepDelayUs);
    break;
  case 5:
    penUp();
    break;
  case 6:
    moveXY_DDA(-r / 3, -r / 2, variableStepDelayUs);
    break;
  case 7:
    penDown();
    break;
  case 8:
    moveXY_DDA(r / 3, -r / 3, variableStepDelayUs);
    break;
  case 9:
    penUp();
    break;
  case 10:
    state = RESET_POSITION;
    letter_state = 0;
    ServoState = 0;
    break;
  }
}

void drawR()
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
    moveXY_DDA(LETTER_W / 8, 0, variableStepDelayUs);
    break;
  case 3:
    drawArc(r, PI / 2, -PI / 2, 10, variableStepDelayUs);
    break;
  case 4:
    moveXY_DDA(-LETTER_W / 8, 0, variableStepDelayUs);
    break;
  case 5:
    moveXY_DDA(LETTER_W / 2, -LETTER_H / 2, variableStepDelayUs);
    break;
  case 6:
    penUp();
    break;
  case 7:
    state = RESET_POSITION;
    letter_state = 0;
    ServoState = 0;
    break;
  }
}

void drawS()
{
  float r = LETTER_H / 4;

  switch (letter_state)
  {
  case 0:
    penDown();
    break;
  case 1:
    moveXY_DDA(LETTER_W / 5, 0, variableStepDelayUs);
    break;
  case 2:
    drawArc(r, -PI / 2, PI / 2, 5, variableStepDelayUs);
    break;
  case 3:
    drawArc(r, (PI * 3) / 2, PI / 2, 5, variableStepDelayUs);
    break;
  case 4:
    moveXY_DDA(LETTER_W / 5, 0, variableStepDelayUs);
    break;
  case 5:
    penUp();
    break;
  case 6:
    state = RESET_POSITION;
    letter_state = 0;
    ServoState = 0;
    break;
  }
}

void drawT()
{
  switch (letter_state)
  {
  case 0:
    moveXY_DDA(0, LETTER_H, variableStepDelayUs);
    break;
  case 1:
    penDown();
    break;
  case 2:
    moveXY_DDA(LETTER_W, 0, variableStepDelayUs);
    break;
  case 3:
    penUp();
    break;
  case 4:
    moveXY_DDA(-LETTER_W / 2, 0, variableStepDelayUs);
    break;
  case 5:
    penDown();
    break;
  case 6:
    moveXY_DDA(0, -LETTER_H, variableStepDelayUs);
    break;
  case 7:
    penUp();
    break;
  case 8:
    state = RESET_POSITION;
    letter_state = 0;
    ServoState = 0;
    break;
  }
}

void drawU()
{
  float r = LETTER_W / 3;

  switch (letter_state)
  {
  case 0:
    moveXY_DDA(0, LETTER_H, variableStepDelayUs);
    break;
  case 1:
    penDown();
    break;
  case 2:
    moveXY_DDA(0, -(LETTER_H - r), variableStepDelayUs);
    break;
  case 3:
    drawArc(r, -PI, 0, 10, variableStepDelayUs);
    break;
  case 4:
    moveXY_DDA(0, LETTER_H - r, variableStepDelayUs);
    break;
  case 5:
    penUp();
    break;
  case 6:
    state = RESET_POSITION;
    letter_state = 0;
    ServoState = 0;
    break;
  }
}

void drawV()
{
  switch (letter_state)
  {
  case 0:
    moveXY_DDA(0, LETTER_H, variableStepDelayUs);
    break;
  case 1:
    penDown();
    break;
  case 2:
    moveXY_DDA(LETTER_W / 2, -LETTER_H, variableStepDelayUs);
    break;
  case 3:
    moveXY_DDA(LETTER_W / 2, LETTER_H, variableStepDelayUs);
    break;
  case 4:
    penUp();
    break;
  case 5:
    state = RESET_POSITION;
    letter_state = 0;
    ServoState = 0;
    break;
  }
}

void drawW()
{
  switch (letter_state)
  {
  case 0:
    moveXY_DDA(0, LETTER_H, variableStepDelayUs);
    break;
  case 1:
    penDown();
    break;
  case 2:
    moveXY_DDA(LETTER_W / 4, -LETTER_H, variableStepDelayUs);
    break;
  case 3:
    moveXY_DDA(LETTER_W / 4, LETTER_H / 2, variableStepDelayUs);
    break;
  case 4:
    moveXY_DDA(LETTER_W / 4, -LETTER_H / 2, variableStepDelayUs);
    break;
  case 5:
    moveXY_DDA(LETTER_W / 4, LETTER_H, variableStepDelayUs);
    break;
  case 6:
    penUp();
    break;
  case 7:
    state = RESET_POSITION;
    letter_state = 0;
    ServoState = 0;
    break;
  }
}

void drawX()
{
  switch (letter_state)
  {
  case 0:
    penDown();
    break;
  case 1:
    moveXY_DDA(LETTER_W * 0.75, LETTER_H, variableStepDelayUs);
    break;
  case 2:
    penUp();
    break;
  case 3:
    moveXY_DDA(-LETTER_W * 0.75, 0, variableStepDelayUs);
    break;
  case 4:
    penDown();
    break;
  case 5:
    moveXY_DDA(LETTER_W * 0.75, -LETTER_H, variableStepDelayUs);
    break;
  case 6:
    penUp();
    break;
  case 7:
    state = RESET_POSITION;
    letter_state = 0;
    ServoState = 0;
    break;
  }
}

void drawY()
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
    moveXY_DDA(LETTER_W / 2, -LETTER_H / 2, variableStepDelayUs);
    break;
  case 4:
    moveXY_DDA(LETTER_W / 2, LETTER_H / 2, variableStepDelayUs);
    break;
  case 5:
    penUp();
    break;
  case 6:
    moveXY_DDA(-LETTER_W / 2, -LETTER_H / 2, variableStepDelayUs);
    break;
  case 7:
    penDown();
    break;
  case 8:
    moveXY_DDA(0, -LETTER_H / 2, variableStepDelayUs);
    break;
  case 9:
    penUp();
    break;
  case 10:
    state = RESET_POSITION;
    letter_state = 0;
    ServoState = 0;
    break;
  }
}

void drawZ()
{
  switch (letter_state)
  {
  case 0:
    moveXY_DDA(LETTER_W, 0, variableStepDelayUs);
    break;
  case 1:
    penDown();
    break;
  case 2:
    moveXY_DDA(-LETTER_W, 0, variableStepDelayUs);
    break;
  case 3:
    moveXY_DDA(LETTER_W, LETTER_H, variableStepDelayUs);
    break;
  case 4:
    moveXY_DDA(-LETTER_W, 0, variableStepDelayUs);
    break;
  case 5:
    penUp();
    break;
  case 6:
    state = RESET_POSITION;
    letter_state = 0;
    ServoState = 0;
    break;
  }
}

/* ======= SPACE ======= */

void drawSpace()
{
  switch (letter_state)
  {
  case 0:
    penUp();
    break;
  case 1:
    moveXY_DDA(LETTER_W / 2, 0, STEP_DELAY_US);
    break;
  case 2:
    state = RESET_POSITION;
    letter_state = 0;
    ServoState = 0;
    break;
  }
}

void drawPercent()
{
  switch (letter_state)
  {
  case 0:
    penDown();
    break;
  case 1:
    moveXY_DDA(LETTER_W / 2, LETTER_H, variableStepDelayUs);
    break;
  case 2:
    penUp();
    break;
  case 3:
    moveXY_DDA(-LETTER_W / 2, 0, variableStepDelayUs);
    break;
  case 4:
    penDown();
    break;
  case 5:
    drawArc(0.05, 0, 2 * PI, 20, variableStepDelayUs);
    break;
  case 6:
    penUp();
    break;
  case 7:
    moveXY_DDA(LETTER_W / 2, -(LETTER_H - 0.1), variableStepDelayUs);
    break;
  case 8:
    penDown();
    break;
  case 9:
    drawArc(0.05, 0, 2 * PI, 20, variableStepDelayUs);
    break;
  case 10:
    penUp();
    break;
  case 11:
    state = RESET_POSITION;
    letter_state = 0;
    ServoState = 0;
    break;
  }
}

void drawParenthesisOpen()
{
  float r = LETTER_H / 2;

  switch (letter_state)
  {
  case 0:
    penDown();
    break;
  case 1:
    drawArc(r, PI / 2, 3 * PI / 2, 30, variableStepDelayUs);
    break;
  case 2:
    penUp();
    break;
  case 3:
    state = RESET_POSITION;
    letter_state = 0;
    ServoState = 0;
    break;
  }
}

void drawParenthesisClose()
{
  float r = LETTER_H / 2;

  switch (letter_state)
  {
  case 0:
    penDown();
    break;
  case 1:
    drawArc(r, -PI / 2, PI / 2, 30, variableStepDelayUs);
    break;
  case 2:
    penUp();
    break;
  case 3:
    state = RESET_POSITION;
    letter_state = 0;
    ServoState = 0;
    break;
  }
}

void drawQuote()
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
    moveXY_DDA(0, -LETTER_H * 0.2, variableStepDelayUs);
    break;
  case 4:
    penUp();
    break;
  case 5:
    moveXY_DDA(SPACE_W / 2, LETTER_H * 0.2, variableStepDelayUs);
    break;
  case 6:
    penDown();
    break;
  case 7:
    moveXY_DDA(0, -LETTER_H * 0.2, variableStepDelayUs);
    break;
  case 8:
    penUp();
    break;
  case 9:
    state = RESET_POSITION;
    letter_state = 0;
    ServoState = 0;
    break;
  }
}


#endif // UPPERCASELETTERS_H
