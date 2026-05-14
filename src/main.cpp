#include <Arduino.h>
#include <Bounce2.h>
#include <ESP32Servo.h>
#include <U8g2lib.h>
#include <Wire.h>

// States
#define STATE_IDLE 0
#define STATE_DRAWING 1
#define STATE_HOMING 2
#define STATE_NEW_LINE 3
#define AFTER_HOMING_NEWLINE 4
#define RESET_POSITION 5
#define MOVEXY 6
#define STATE_READING 7
#define STATE_CLEARBUFFER 8

// Motor 1
#define DIR1_PIN 18
#define STEP1_PIN 19
#define ENABLE1_PIN 23
// Motor 2
#define DIR2_PIN 15
#define STEP2_PIN 2
#define ENABLE2_PIN 17
// Servo
#define SERVO_PIN 13
// Display
#define SCL 22
#define SDA 21
// LEDs
#define LED_READY 26
#define LED_BUSY 27
// Taster
#define Taster 36
// Sensoren
#define Sensor1 14
#define Sensor2 33
// Step settings
#define STEPS_PER_MM 4.76
#define STEPS_PER_CM (STEPS_PER_MM * 10.0)
#define STEP_DELAY_US 700
#define STEP_DELAY_US_FREE 300
long variableStepDelayUs = STEP_DELAY_US;
// Servo settings
#define SERVO_UP 30
#define SERVO_DOWN 90
// Letter settings
double LETTER_W = 0.8;
double LETTER_H = 1.0;
double SPACE_W = 0.2;
double LINE_H = 1.2;
double MAX_WIDTH = 14;
double MAX_HEIGTH = 14;

bool settings = false;
String wholeSetting = "";

int state = STATE_IDLE; // changed from homing
int letter_state = 0;
int remember_letter_state = 0;
int draw_state = 0;
int arc_state = 0;
int drawArc_state = 0;
int oldLetterState;

double resetPositionX = 0.0;
double resetPositionY = 0.0;

double calculatedResetX = 0.0;
double calculatedResetY = 0.0;

double stepErrorX = 0;
double stepErrorY = 0;

float tempX = 0.0;
float tempY = 0.0;

double arcRadius = 0.0;

double prevX = 0.0;
double prevY = 0.0;
double angleStep = 0.0;

double theta = 0.0;
double x = 0.0;
double y = 0.0;
double dx = 0.0;
double dy = 0.0;

bool countRealWidth = false;
double maxWidth = 0.0;

bool countPosition = false;
bool writeStatus = true;
bool goTrough = true;
bool heightAlreadySet = false;
bool drawConnection = false;

long stepsY = 0;
long stepsX = 0;
long maxSteps = 0;
long errorX = 0;
long errorY = 0;
long stepCounter = 0;

long long XYTime = micros();
long long servoTime = 0;

long long firstTime = millis();
long long secondTime = millis();

long arcCounter = 0;
int savedSegments = 0;

unsigned long vergangeneZeit = 0;
int debouncePenUp = 100;
int debouncePenDown = 200;

int XYState = 0;
int ServoState = 0;
int stateNewLine = 0;

char c;
char nextChar;
int letterToDraw;

bool S_Button = false;
bool drawing_Done = false;
bool actualLetterDone = true;
bool displayUpdate = false;
bool sensor1Triggered = false;
bool sensor2Triggered = false;
bool arcReset = false;
bool clearBuffer = true;
bool penCal = false;
bool wasCR = false;
bool goToNewLine = false;

bool drawDotEnd = false;
bool drawCommaEnd = false;

void read();
void transsitions();
void actions();
void motorsEnable();
void motorsDisable();
void drawLetter(char c);
void penUp();
void penDown();
void moveXY_DDA(float x_cm, float y_cm, int stepDelayUs);

void drawA();
void drawB();
void drawC();
void drawD();
void drawE();
void drawF();
void drawG();
void drawH();
void drawI();
void drawJ();
void drawK();
void drawL();
void drawM();
void drawN();
void drawO();
void drawP();
void drawQ();
void drawR();
void drawS();
void drawT();
void drawU();
void drawV();
void drawW();
void drawX();
void drawY();
void drawZ();
void drawa();
void drawb();
void drawc();
void drawd();
void drawe();
void drawf();
void drawg();
void drawh();
void drawi();
void drawj();
void drawk();
void drawl();
void drawm();
void drawn();
void drawo();
void drawp();
void drawq();
void drawr();
void draws();
void drawt();
void drawu();
void drawv();
void draww();
void drawx();
void drawy();
void drawz();
void drawDot();
void drawComma();
void drawExclamation();
void drawQuestion();
void drawColon();
void drawEqual();
void drawPlus();
void drawMinus();
void drawPercent();
void drawParenthesisOpen();
void drawParenthesisClose();
void drawQuote();
void drawSpace();
void draw0();
void draw1();
void draw2();
void draw3();
void draw4();
void draw5();
void draw6();
void draw7();
void draw8();
void draw9();

struct Entry
{
  char key;
  void (*func)();
};

Entry letters[] = {
    // Space
    {' ', drawSpace},

    // Lowercase
    {'a', drawa},
    {'b', drawb},
    {'c', drawc},
    {'d', drawd},
    {'e', drawe},
    {'f', drawf},
    {'g', drawg},
    {'h', drawh},
    {'i', drawi},
    {'j', drawj},
    {'k', drawk},
    {'l', drawl},
    {'m', drawm},
    {'n', drawn},
    {'o', drawo},
    {'p', drawp},
    {'q', drawq},
    {'r', drawr},
    {'s', draws},
    {'t', drawt},
    {'u', drawu},
    {'v', drawv},
    {'w', draww},
    {'x', drawx},
    {'y', drawy},
    {'z', drawz},

    // Uppercase
    {'A', drawA},
    {'B', drawB},
    {'C', drawC},
    {'D', drawD},
    {'E', drawE},
    {'F', drawF},
    {'G', drawG},
    {'H', drawH},
    {'I', drawI},
    {'J', drawJ},
    {'K', drawK},
    {'L', drawL},
    {'M', drawM},
    {'N', drawN},
    {'O', drawO},
    {'P', drawP},
    {'Q', drawQ},
    {'R', drawR},
    {'S', drawS},
    {'T', drawT},
    {'U', drawU},
    {'V', drawV},
    {'W', drawW},
    {'X', drawX},
    {'Y', drawY},
    {'Z', drawZ},

    // Numbers
    {'0', draw0},
    {'1', draw1},
    {'2', draw2},
    {'3', draw3},
    {'4', draw4},
    {'5', draw5},
    {'6', draw6},
    {'7', draw7},
    {'8', draw8},
    {'9', draw9},

    // Symbols
    {'.', drawDot},
    {',', drawComma},
    {'!', drawExclamation},
    {'?', drawQuestion},
    {':', drawColon},
    {'=', drawEqual},
    {'+', drawPlus},
    {'-', drawMinus},
    {'%', drawPercent},
    {'(', drawParenthesisOpen},
    {')', drawParenthesisClose},
    {'"', drawQuote}};

// Servo Objekt
Servo penServo;

Bounce MeinTaster;
// Startpunkt
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
float currentX = 0.0;
float currentY = 0.0;

void read()
{
  if (MeinTaster.rose() && ((state == STATE_DRAWING) || (state == STATE_READING) || (state == RESET_POSITION)))
  {
    S_Button = true;
  }

  if (MeinTaster.rose() && state == STATE_IDLE)
  {
    firstTime = millis();
  }

  if (MeinTaster.fell() && state == STATE_IDLE)
  {
    secondTime = millis();
    if (secondTime - firstTime > 2000)
    {
      state = STATE_HOMING;
    }
  }

  if (digitalRead(Sensor1))
    sensor1Triggered = true;
  else
    sensor1Triggered = false;

  if (digitalRead(Sensor2))
    sensor2Triggered = true;
  else
    sensor2Triggered = false;

  if ((state == STATE_HOMING && digitalRead(Sensor1) && digitalRead(Sensor2)) ||
      (state == STATE_NEW_LINE && digitalRead(Sensor1)))
  {
    state = AFTER_HOMING_NEWLINE;
    letter_state = 0;
    draw_state = 0;
  }
}

void transsitions()
{
  if (state == STATE_READING)
  {
    if (settings && (c != '{') && (c != '}'))
      wholeSetting = wholeSetting + c;
    if (c == '{')
      settings = true;
    else if (c == '}')
    {
      settings = false;
      LETTER_H = wholeSetting.toFloat();
      LETTER_W = LETTER_H * 0.8;
      LINE_H = LETTER_H * 1.6;
      wholeSetting = "";
      state = STATE_IDLE;
    }
    else if (c == '\r')
    {
    }
    else if ((c == '\n') && !settings)
    {
      state = STATE_IDLE;
      actualLetterDone = true;
      displayUpdate = false;
      letter_state = 0;
    }
    else if (!settings)
      drawLetter(c);
    else
    {
      state = STATE_IDLE;
      actualLetterDone = true;
    }
  }
  if ((state == STATE_IDLE || state == STATE_DRAWING || state == STATE_READING) && S_Button && actualLetterDone)
  {
    state = STATE_CLEARBUFFER;
    clearBuffer = true;
    S_Button = false;
    actualLetterDone = false;
  }
  if (Serial.available() && state != RESET_POSITION && actualLetterDone && state != STATE_NEW_LINE && state != AFTER_HOMING_NEWLINE && state != STATE_HOMING)
  {
    if (currentX + (LETTER_W * 2.5) > MAX_WIDTH)
    {
      if (currentY + LINE_H > MAX_HEIGTH)
      {
        state = STATE_HOMING;
      }
      else
        state = STATE_NEW_LINE;
      stateNewLine = 0;
    }
    else
    {
      state = STATE_READING;
      c = Serial.read();
      actualLetterDone = false;
      displayUpdate = true;
      letter_state = 0;
    }
  }
}

void actions()
{
  switch (state)
  {
  case STATE_IDLE:
    digitalWrite(LED_BUSY, LOW);
    digitalWrite(LED_READY, HIGH);
    // === READY SCREEN ===
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB14_tr);
    u8g2.drawStr(0, 20, "Ready to");
    u8g2.drawStr(0, 40, "read");
    u8g2.setFont(u8g2_font_ncenB10_tr);
    u8g2.drawStr(0, 60, "You can send!");
    u8g2.sendBuffer();
    motorsDisable();
    if (S_Button || penCal)
    {
      penCal = true;
      penDown();
      if (ServoState == 0)
      {
        penCal = false;
      }
    }

    break;

  case STATE_DRAWING:
  {
    digitalWrite(LED_READY, LOW);
    digitalWrite(LED_BUSY, HIGH);

    if (displayUpdate)
    {
      char bigCurrent[2] = {(c == ' ' ? '_' : c), '\0'}; // Leerzeichen → _

      // Nächsten Buchstaben auslesen
      nextChar = '-';
      if (Serial.available())
        nextChar = Serial.peek();

      char bigNext[2] = {(nextChar == ' ' ? '_' : nextChar), '\0'}; // Leerzeichen → _

      // === DISPLAY AUFBAU ===
      u8g2.clearBuffer();

      // --- LINKS: Current ---
      u8g2.setFont(u8g2_font_ncenB08_tr);
      u8g2.drawStr(0, 10, "Current:");
      u8g2.setFont(u8g2_font_ncenB24_tr);
      u8g2.drawStr(0, 45, bigCurrent);

      // Trennlinie in der Mitte
      u8g2.drawLine(63, 0, 63, 64);

      // --- RECHTS: Next ---
      u8g2.setFont(u8g2_font_ncenB08_tr);
      u8g2.drawStr(68, 10, "Next:");
      u8g2.setFont(u8g2_font_ncenB24_tr);
      u8g2.drawStr(68, 45, bigNext);

      u8g2.sendBuffer();
      displayUpdate = false;
    }

    motorsEnable();

    letters[letterToDraw - '!'].func();

    break;
  }

  case STATE_NEW_LINE:
    digitalWrite(LED_READY, LOW);
    digitalWrite(LED_BUSY, HIGH);
    motorsEnable();
    actualLetterDone = false;
    variableStepDelayUs = STEP_DELAY_US_FREE;
    switch (stateNewLine)
    {
    case 0:
      if (Serial.peek() != ' ')
      {
        drawConnection = true;
        actualLetterDone = false;
        if (Serial.peek() == ',')
        {
          drawCommaEnd = true;
          Serial.read();
        }
        if (Serial.peek() == '.')
        {
          drawDotEnd = true;
          Serial.read();
        }
      }
      else
      {
        char readBuffer = Serial.read();
      }
      stateNewLine = 1;

      break;
    case 1:
      if (drawConnection)
      {
        letter_state = 0;
        drawConnection = false;
        goToNewLine = true;
      }
      if (goToNewLine)
      {
        if (drawDotEnd)
        {
          drawDot();
        }
        if (drawCommaEnd)
        {
          drawComma();
        }else
          drawMinus();
      }else
        stateNewLine++;
      break;
    case 2:
      penUp();
      if (ServoState == 0)
      {
        stateNewLine++;
      }
      break;
    case 3:
      moveXY_DDA(-1, 0, variableStepDelayUs);
      if (sensor1Triggered)
      {
        stateNewLine++;
      }
      break;
    case 4:
      moveXY_DDA(0, -LINE_H, variableStepDelayUs);
      if (draw_state == 0)
      {
        stateNewLine++;
      }
      break;
    case 5:
      currentY = currentY + LINE_H;
      variableStepDelayUs = STEP_DELAY_US;
      state = AFTER_HOMING_NEWLINE;
      letter_state = 0;
      stateNewLine = 0;
      goToNewLine = false;
      drawCommaEnd = false;
      drawDotEnd = false;
      actualLetterDone = true;
      break;
    }
    break;

  case STATE_HOMING:
    variableStepDelayUs = STEP_DELAY_US_FREE;
    actualLetterDone = false;
    switch (letter_state)
    {
    case 0:
      penUp();
      drawConnection = false;
      break;

    case 1:
      if (!sensor1Triggered)
      {
        moveXY_DDA(-1, 0, variableStepDelayUs);
      }
      if (!sensor2Triggered)
      {
        moveXY_DDA(0, 1, variableStepDelayUs);
      }
      if (sensor1Triggered && sensor2Triggered)
      {
        letter_state++;
      }
      else
        letter_state = 1;
      break;
    case 2:
      letter_state = 0;
      break;
    }
    digitalWrite(LED_READY, LOW);
    digitalWrite(LED_BUSY, HIGH);
    motorsEnable();
    currentY = 0.0;
    variableStepDelayUs = STEP_DELAY_US;
    break;
  case AFTER_HOMING_NEWLINE:
    variableStepDelayUs = STEP_DELAY_US_FREE;
    switch (letter_state)
    {
    case 0:
      moveXY_DDA(LETTER_W / 2, -LINE_H * 1.5, variableStepDelayUs);
      break;

    case 1:
      state = STATE_IDLE;
      letter_state = 0;
      actualLetterDone = true;
      currentX = 0.0;
      break;
    }
    digitalWrite(LED_READY, LOW);
    digitalWrite(LED_BUSY, HIGH);
    motorsEnable();

    variableStepDelayUs = STEP_DELAY_US;
    heightAlreadySet = false;
    break;

  case RESET_POSITION:
    switch (letter_state)
    {
    case 0:
      penUp();
      motorsEnable();

      break;
    case 1:
      countPosition = false;
      calculatedResetX = maxWidth - resetPositionX;
      letter_state++;
      break;
    case 2:
      moveXY_DDA(calculatedResetX + SPACE_W, -resetPositionY, STEP_DELAY_US);
      break;
    case 3:
      Serial.println(calculatedResetX);
      Serial.println(calculatedResetY);
      Serial.println(maxWidth);
      resetPositionX = 0.0;
      resetPositionY = 0.0;
      tempX = 0.0;
      tempY = 0.0;
      calculatedResetX = 0.0;
      calculatedResetY = 0.0;
      maxWidth = 0.0;
      letter_state = 0;
      ServoState = 0;
      actualLetterDone = true;
      if (goToNewLine)
      {
        state = STATE_NEW_LINE;
      }
      else
        state = STATE_IDLE;
      goToNewLine = false;
      break;
    }
    break;
  case STATE_CLEARBUFFER:
    actualLetterDone = false;
    if (Serial.available() && clearBuffer)
    {
      char buffer = Serial.read();
      if (!Serial.available())
      {
        clearBuffer = false;
        actualLetterDone = true;
        state = STATE_IDLE;
      }
    }
    break;
  }
}
// Stift anheben
void penUp()
{
  switch (ServoState)
  {
  case 0:
    servoTime = millis();
    penServo.write(SERVO_UP);
    ServoState = 1;
    break;
  case 1:
    if (millis() - servoTime > debouncePenUp)
    {
      ServoState = 0;
      letter_state++;
    }
    break;
  }
}
// Stift absenken
void penDown()
{
  switch (ServoState)
  {
  case 0:
    servoTime = millis();
    penServo.write(SERVO_DOWN);
    ServoState = 1;
    break;
  case 1:
    if (millis() - servoTime > debouncePenDown)
    {
      ServoState = 0;
      letter_state++;
    }
    break;
  }
}

void moveXY_DDA(float x_cm, float y_cm, int stepDelayUs)
{
  switch (draw_state)
  {
  case 0:
  {
    draw_state = 1;
    float stepsX_f = fabs(x_cm * STEPS_PER_CM);
    float stepsY_f = fabs(y_cm * STEPS_PER_CM);

    stepsX = stepsX_f;
    stepsY = stepsY_f;

    stepErrorX = 0;

    stepErrorX = stepErrorX + (stepsX_f - stepsX);
    stepErrorY = stepErrorY + (stepsY_f - stepsY);

    if (lround(stepErrorX) == 1)
    {
      stepsX = stepsX + lround(stepErrorX);
    }
    if (lround(stepErrorY) == 1)
    {
      stepsY = stepsY + lround(stepErrorY);
    }

    digitalWrite(DIR1_PIN, x_cm >= 0 ? LOW : HIGH);
    digitalWrite(DIR2_PIN, y_cm >= 0 ? HIGH : LOW);

    maxSteps = max(stepsX, stepsY);
    if (maxSteps == 0.0)
    {
      return;
    }

    if (arcReset)
    {
      errorX = 0;
      errorY = 0;
      arcReset = false;
    }
    XYState = 0;
    stepCounter = 0;

    XYTime = micros();
    break;
  }
  case 1:
    if ((micros() - XYTime > stepDelayUs))
    {
      if (XYState == 0)
      {
        errorX += stepsX;
        errorY += stepsY;
        if (errorX >= maxSteps)
        {
          digitalWrite(STEP1_PIN, HIGH);
          errorX -= maxSteps;
          if (x_cm < 0)
          {
            currentX -= 1.0 / STEPS_PER_CM;
          }
          else
            currentX += 1.0 / STEPS_PER_CM;

          if (countPosition)
          {
            if (x_cm < 0)
            {
              resetPositionX -= 1.0 / STEPS_PER_CM;
            }
            else
              resetPositionX += 1.0 / STEPS_PER_CM;
            if (resetPositionX > maxWidth)
              maxWidth = resetPositionX;
          }
        }

        if (errorY >= maxSteps)
        {
          digitalWrite(STEP2_PIN, HIGH);
          errorY -= maxSteps;
          if (countPosition)
          {
            if (y_cm < 0)
            {
              resetPositionY -= 1.0 / STEPS_PER_CM;
            }
            else
              resetPositionY += 1.0 / STEPS_PER_CM;
          }
        }
      }

      if (XYState == 2)
      {
        if (maxSteps <= stepCounter)
        {
          draw_state = 0;
          letter_state++;
          ServoState = 0;
          drawArc_state = 2;
        }
        else
          stepCounter++;
        XYState = -1;
      }

      if (XYState == 1)
      {
        digitalWrite(STEP1_PIN, LOW);
        digitalWrite(STEP2_PIN, LOW);
      }
      XYState++;
      XYTime = micros();
    }
    break;
  }
}

void drawArc(float radius_cm, float startAngle, float endAngle, int segments, int stepDelayUs)
{
  switch (arc_state)
  {
  case 0:
    arc_state = 1;
    arcCounter = 0;
    drawArc_state = 0;
    savedSegments = segments;

    angleStep = (endAngle - startAngle) / segments;

    prevX = radius_cm * cos(startAngle);
    prevY = radius_cm * sin(startAngle);
    remember_letter_state = letter_state;
    arcReset = true;
    break;

  case 1:
    switch (drawArc_state)
    {
    case 0:
      arcCounter++;

      theta = startAngle + arcCounter * angleStep;
      x = radius_cm * cos(theta);
      y = radius_cm * sin(theta);
      dx = x - prevX;
      dy = y - prevY;
      drawArc_state = 1;
      variableStepDelayUs = variableStepDelayUs * 2;
      break;

    case 1:
      moveXY_DDA(dx, dy, variableStepDelayUs);
      letter_state = remember_letter_state;
      break;

    case 2: // kommt von moveXY_DDA via drawArc_state++
      prevX = x;
      prevY = y;

      variableStepDelayUs = STEP_DELAY_US;

      if (arcCounter >= savedSegments)
      {
        arc_state = 0;
        drawArc_state = 0;
        letter_state++; // erst JETZT letter_state weiterzählen
        arcReset = true;
      }
      else
      {
        drawArc_state = 0;
        arc_state = 1;
      }
      break;
    }
    break;
  }
}

void motorsEnable()
{
  digitalWrite(ENABLE1_PIN, LOW);
  digitalWrite(ENABLE2_PIN, LOW);
}

void motorsDisable()
{
  digitalWrite(ENABLE1_PIN, HIGH);
  digitalWrite(ENABLE2_PIN, HIGH);
}

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
// Funktionen der Kleinbuchstaben

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
    ServoState = 0;
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
    ServoState = 0;
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
    ServoState = 0;
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
    ServoState = 0;
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
    ServoState = 0;
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
    ServoState = 0;
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
    ServoState = 0;
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
    ServoState = 0;
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
    ServoState = 0;
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
    ServoState = 0;
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
    ServoState = 0;
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
    ServoState = 0;
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
    ServoState = 0;
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
    ServoState = 0;
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
    ServoState = 0;
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
    ServoState = 0;
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
    ServoState = 0;
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
    ServoState = 0;
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
    ServoState = 0;
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
    ServoState = 0;
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
    ServoState = 0;
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
    ServoState = 0;
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
    ServoState = 0;
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
    ServoState = 0;
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
    ServoState = 0;
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
    ServoState = 0;
    break;
  }
}

void setup()
{
  Serial.begin(115200);

  pinMode(DIR1_PIN, OUTPUT);
  pinMode(STEP1_PIN, OUTPUT);
  pinMode(ENABLE1_PIN, OUTPUT);

  pinMode(DIR2_PIN, OUTPUT);
  pinMode(STEP2_PIN, OUTPUT);
  pinMode(ENABLE2_PIN, OUTPUT);

  pinMode(Sensor1, INPUT);
  pinMode(Sensor2, INPUT);

  digitalWrite(ENABLE1_PIN, LOW);
  digitalWrite(ENABLE2_PIN, LOW);

  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(Taster, INPUT);

  pinMode(LED_BUSY, OUTPUT);
  pinMode(LED_READY, OUTPUT);

  pinMode(SERVO_PIN, OUTPUT);
  digitalWrite(SERVO_PIN, LOW);

  penServo.write(SERVO_UP);
  penServo.attach(SERVO_PIN, 500, 2400);
  MeinTaster.attach(Taster);
  MeinTaster.interval(40);
  u8g2.begin();

  servoTime = millis();
  // state = STATE_IDLE;

  resetPositionX = 0.0;
  resetPositionY = 0.0;
  tempX = 0.0;
  tempY = 0.0;
  calculatedResetX = 0.0;
  calculatedResetY = 0.0;
  maxWidth = 0.0;
  letter_state = 0;
  ServoState = 0;
  actualLetterDone = true;
}

void drawLetter(char c)
{
  resetPositionX = 0.0;
  resetPositionY = 0.0;
  tempX = 0.0;
  tempY = 0.0;
  countPosition = true;

  for (char i = '!'; i < '}'; i++)
  {
    if (letters[i - '!'].key == c)
    {
      letterToDraw = i;
      break;
    }
  }
  state = STATE_DRAWING;
}

void loop()
{
  MeinTaster.update();

  read();
  transsitions();
  actions();
}
