#if !defined(SYMBOLSNUMBERS_H)
#define SYMBOLSNUMBERS_H

extern void penUp();
extern void moveXY_DDA(float x_cm, float y_cm, int stepDelayUs);
extern void penDown();
extern void drawArc(float radius_cm, float startAngle, float endAngle, int segments, int stepDelayUs);

void draw0()
{
  float r = LETTER_H / 3;
  switch (letter_state)
  {
  case 0:
    penUp();
    break;
  case 1:
    moveXY_DDA(0, r, variableStepDelayUs);
    break;
  case 2:
    penDown();
    break;
  case 3:
    drawArc(r, PI, 2 * PI, 10, variableStepDelayUs);
    break;
  case 4:
    moveXY_DDA(0, LETTER_H / 3, variableStepDelayUs);
    break;
  case 5:
    drawArc(r, 0, PI, 10, variableStepDelayUs);
    break;
  case 6:
    moveXY_DDA(0, -LETTER_H / 3, variableStepDelayUs);
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

void draw1()
{
  switch (letter_state)
  {
  case 0:
    actualLetterDone = false;
    penUp();
    break;
  case 1:
    moveXY_DDA(LETTER_W / 3, 0, variableStepDelayUs);
    break;
  case 2:
    penDown();
    break;
  case 3:
    moveXY_DDA(0, LETTER_H, variableStepDelayUs);
    break;
  case 4:
    moveXY_DDA(-LETTER_W / 3, -LETTER_H / 4, variableStepDelayUs);
    break;
  case 5:
    penUp();
  case 6:
    state = RESET_POSITION;
    letter_state = 0;
    ServoState = 0;
    break;
  }
}

void draw2()
{
  float r = LETTER_H / 4;

  switch (letter_state)
  {
  case 0:
    actualLetterDone = false;
    penUp();
    break;
  case 1:
    moveXY_DDA(LETTER_H / 2, 0, variableStepDelayUs);
    break;
  case 2:
    penDown();
    break;
  case 3:
    moveXY_DDA(2 * -r, 0, variableStepDelayUs);
    break;
  case 4:
    moveXY_DDA(2 * r, (LETTER_H * 0.67), variableStepDelayUs);
    break;
  case 5:
    drawArc(r, 0, PI, 7, variableStepDelayUs);
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

void draw3()
{
  float r = LETTER_H / 4;

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
    moveXY_DDA(LETTER_W / 4, 0, variableStepDelayUs);
    break;
  case 4:
    drawArc(r, PI / 2, -PI / 2, 4, variableStepDelayUs);
    break;
  case 5:
    drawArc(r, PI / 2, -PI / 2, 4, variableStepDelayUs);
    break;
  case 6:
    moveXY_DDA(-LETTER_W / 4, 0, variableStepDelayUs);
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

void draw4()
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
    moveXY_DDA(0, -LETTER_H / 2, variableStepDelayUs);
    break;
  case 4:
    moveXY_DDA(LETTER_W / 2, 0, variableStepDelayUs);
    break;
  case 5:
    penUp();
    break;
  case 6:
    moveXY_DDA(-LETTER_W / 4, LETTER_H / 2, variableStepDelayUs);
    break;
  case 7:
    penDown();
    break;
  case 8:
    moveXY_DDA(0, -LETTER_H, variableStepDelayUs);
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
void draw5()
{
  float r = LETTER_H / 4;

  switch (letter_state)
  {
  case 0:
    penDown();
    break;
  case 1:
    moveXY_DDA(LETTER_W / 4, 0, variableStepDelayUs);
    break;
  case 2:
    drawArc(r, -PI / 2, PI / 2, 5, variableStepDelayUs);
    break;
  case 3:
    moveXY_DDA(-LETTER_W / 4, 0, variableStepDelayUs);
    break;
  case 4:
    moveXY_DDA(0, LETTER_H / 2, variableStepDelayUs);
    break;
  case 5:
    moveXY_DDA(r + LETTER_W / 4, 0, variableStepDelayUs);
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

void draw6()
{
  float r = LETTER_H / 4;

  switch (letter_state)
  {
  case 0:
    moveXY_DDA(r, 0, variableStepDelayUs);
    break;
  case 1:
    penDown();
    break;
  case 2:
    drawArc(r, PI * 3 / 2, -PI / 2, 10, variableStepDelayUs);
    break;
  case 3:
    penUp();
    break;
  case 4:
    drawArc(r, -PI / 2, PI, 5, variableStepDelayUs);
    break;
  case 5:
    penDown();
    break;
  case 6:
    moveXY_DDA(0, (LETTER_H / 3) + r, variableStepDelayUs);
    break;
  case 7:
    drawArc(r, PI, PI * 0.2, 5, variableStepDelayUs);
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

void draw7()
{
  switch (letter_state)
  {
  case 0:
    penDown();
    break;
  case 1:
    moveXY_DDA(LETTER_W * 0.7, LETTER_H, variableStepDelayUs);
    break;
  case 2:
    moveXY_DDA(-LETTER_W * 0.7, 0, variableStepDelayUs);
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
void draw8()
{
  float r = LETTER_H / 4;

  switch (letter_state)
  {
  case 0:
    penUp();
    break;
  case 1:
    moveXY_DDA(r, LETTER_H * 0.75, variableStepDelayUs);
    break;
  case 2:
    penDown();
    break;
  case 3:
    drawArc(r, 0, 2 * PI, 10, variableStepDelayUs);
    break;
  case 4:
    penUp();
    break;
  case 5:
    moveXY_DDA(0, -LETTER_H / 2, variableStepDelayUs);
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
    ServoState = 0;
    break;
  }
}

void draw9()
{
  float r = LETTER_H / 4;

  switch (letter_state)
  {
  case 0:
    moveXY_DDA(0, r, variableStepDelayUs);
    break;
  case 1:
    penDown();
    break;
  case 2:
    drawArc(r, PI * 1.1, 2 * PI, 10, variableStepDelayUs);
    break;
  case 3:
    moveXY_DDA(0, LETTER_H / 2, variableStepDelayUs);
    break;
  case 4:
    drawArc(r, 0, 2 * PI, 10, variableStepDelayUs);
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

void drawDot()
{
  switch (letter_state)
  {
  case 0:
    penDown();
    break;
  case 1:
    moveXY_DDA(0, 0.1, variableStepDelayUs);
    break;
  case 2:
    penUp();
    break;
  case 3:
    state = RESET_POSITION;
    letter_state = 0;
    ServoState = 0;
    stateNewLine++;
    break;
  }
}

void drawComma()
{
  switch (letter_state)
  {
  case 0:
    penDown();
    break;
  case 1:
    moveXY_DDA(0.1, -0.15, variableStepDelayUs);
    break;
  case 2:
    penUp();
    break;
  case 3:
    state = RESET_POSITION;
    letter_state = 0;
    ServoState = 0;
    stateNewLine++;
    break;
  }
}

void drawExclamation()
{
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
    moveXY_DDA(0, LETTER_H * 0.7, variableStepDelayUs);
    break;
  case 4:
    penUp();
    break;
  case 5:
    moveXY_DDA(0, -LETTER_H, variableStepDelayUs);
    break;
  case 6:
    penDown();
    break;
  case 7:
    moveXY_DDA(0, 0.1, variableStepDelayUs);
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

void drawQuestion()
{
  float r = LETTER_H / 4;

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
    drawArc(r, PI, 2 * PI, 20, variableStepDelayUs);
    break;
  case 4:
    moveXY_DDA(0, -LETTER_H / 2, variableStepDelayUs);
    break;
  case 5:
    penUp();
    break;
  case 6:
    moveXY_DDA(0, -LETTER_H / 2, variableStepDelayUs);
    break;
  case 7:
    penDown();
    break;
  case 8:
    moveXY_DDA(0, 0.1, variableStepDelayUs);
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

void drawColon()
{
  switch (letter_state)
  {
  case 0:
    moveXY_DDA(0, LETTER_H / 4, variableStepDelayUs);
    break;
  case 1:
    penDown();
    break;
  case 2:
    moveXY_DDA(0, 0.06, variableStepDelayUs);
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
    moveXY_DDA(0, 0.06, variableStepDelayUs);
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

void drawEqual()
{
  switch (letter_state)
  {
  case 0:
    moveXY_DDA(0, LETTER_H / 4, variableStepDelayUs);
    break;
  case 1:
    penDown();
    break;
  case 2:
    moveXY_DDA(LETTER_W / 3, 0, variableStepDelayUs);
    break;
  case 3:
    penUp();
    break;
  case 4:
    moveXY_DDA(-LETTER_W / 3, LETTER_H / 6, variableStepDelayUs);
    break;
  case 5:
    penDown();
    break;
  case 6:
    moveXY_DDA(LETTER_W / 3, 0, variableStepDelayUs);
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

void drawPlus()
{
  switch (letter_state)
  {
  case 0:
    moveXY_DDA(0, LETTER_H / 2, variableStepDelayUs);
    break;
  case 1:
    penDown();
    break;
  case 2:
    moveXY_DDA(LETTER_W / 3, 0, variableStepDelayUs);
    break;
  case 3:
    penUp();
    break;
  case 4:
    moveXY_DDA(-LETTER_W / 6, LETTER_H / 6, variableStepDelayUs);
    break;
  case 5:
    penDown();
    break;
  case 6:
    moveXY_DDA(0, -LETTER_H / 3, variableStepDelayUs);
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

void drawMinus()
{
  switch (letter_state)
  {
  case 0:
    moveXY_DDA(0, LETTER_H / 2, variableStepDelayUs);
    break;
  case 1:
    penDown();
    break;
  case 2:
    moveXY_DDA(LETTER_W / 3, 0, variableStepDelayUs);
    break;
  case 3:
    penUp();
    break;
  case 4:
    state = RESET_POSITION;
    actualLetterDone = true;
    letter_state = 0;
    ServoState = 0;
    stateNewLine++;

    break;
  }
}

void drawSlash()
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
    state = RESET_POSITION;
    letter_state = 0;
    ServoState = 0;
    break;
  }
}

#endif // SYMBOLSNUMBERS_H
