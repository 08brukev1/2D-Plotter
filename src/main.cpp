#include <Arduino.h>
#include <ESP32Servo.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <Bounce2.h>

// States
#define STATE_IDLE 0
#define STATE_DRAWING 1
#define HOMING 2
#define NEW_LINE 3
#define AFTER_HOMING_NEWLINE 4
#define RESET_POSITION 5
#define MOVEXY 6

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
#define STEP_DELAY_US 1200
#define STEP_DELAY_US_FREE 400
long variableStepDelayUs = STEP_DELAY_US;
// Servo settings
#define SERVO_UP 30
#define SERVO_DOWN 90
// Letter settings
float LETTER_W = 0.8;
float LETTER_H = 1.0;
float SPACE_W = 0.2;
float LINE_H = 1.6;
float MAX_WIDTH = 14.0;
float MAX_HEIGTH = 14.0;

bool settings = false;
String wholeSetting = "";

int state = HOMING;
int letter_state = 0;
int draw_state = 0;

float resetPositionX = 0.0;
float resetPositionY = 0.0;

float calculatedResetX = 0.0;
float calculatedResetY = 0.0;

bool countRealWidth = false;
float maxWidth = 0.0;

bool countPosition = false;
bool writeStatus = true;
bool goTrough = true;
bool heightAlreadySet = false;

long stepsY = 0;
long stepsX = 0;
long maxSteps = 0;
long errorX = 0;
long errorY = 0;

unsigned long vergangeneZeit = 0;
int debouncePenUp = 100;
int debouncePenDown = 200;

char c;
int letterToDraw;

bool S_Button = true;
bool drawing_Done = false;
bool actualLetterDone = true;

void read();
void transsitions();
void actions();
void startMOVEXY();

// Servo Objekt
Servo penServo;

Bounce MeinTaster;
// Startpunkt
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
float currentX = 0.0;
float currentY = 0.0;

void read()
{
  if (MeinTaster.rose() && (state == STATE_IDLE || state == STATE_DRAWING))
  {
    S_Button = !S_Button;
  }
  if ((state == HOMING && digitalRead(Sensor1) && digitalRead(Sensor2)) || (state == NEW_LINE && digitalRead(Sensor1)))
  {
    state = AFTER_HOMING_NEWLINE;
  }
  if (Serial.available() && state == STATE_IDLE && S_Button && actualLetterDone)
  {
    state = STATE_DRAWING;
    c = Serial.read();
    actualLetterDone = false;
  }
  else if (state == STATE_DRAWING && !S_Button)
  {
    state = STATE_IDLE;
  }
}

void transsitions()
{
  if (state == STATE_DRAWING)
  {
    if (currentX + LETTER_W > MAX_WIDTH)
    {
      if (currentY + LINE_H > MAX_HEIGTH)
      {
        state = HOMING;
      }
      else
        state = NEW_LINE;
    }

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
    }
    else if (c == '\r')
    {
    }
    else if ((c == '\n') && !settings)
      state == NEW_LINE;
    else if (!settings)
      drawLetter(c);
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
    break;

  case STATE_DRAWING:
    digitalWrite(LED_READY, LOW);
    digitalWrite(LED_BUSY, HIGH);

    char bigCurrent[2] = {(c == ' ' ? '_' : c), '\0'}; // Leerzeichen → _

    // Nächsten Buchstaben auslesen
    char nextChar = '-';
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
    motorsEnable();
    startMOVEXY();
    letters[letterToDraw - '!'].key;
    break;
  case NEW_LINE:
    penUp();
    digitalWrite(LED_READY, LOW);
    digitalWrite(LED_BUSY, HIGH);
    motorsEnable();
    variableStepDelayUs = STEP_DELAY_US_FREE;
    currentY = 0.0;
    moveXY_DDA(-1, 0, variableStepDelayUs);
    variableStepDelayUs = STEP_DELAY_US;
    if (!heightAlreadySet)
    {
      currentY = currentY + LINE_H;
      heightAlreadySet = true;
    }
    break;

  case HOMING:
    penUp();
    digitalWrite(LED_READY, LOW);
    digitalWrite(LED_BUSY, HIGH);
    motorsEnable();
    variableStepDelayUs = STEP_DELAY_US_FREE;
    currentY = 0.0;
    moveXY_DDA(-1, 1, variableStepDelayUs);
    variableStepDelayUs = STEP_DELAY_US;
    break;
  case AFTER_HOMING_NEWLINE:
    digitalWrite(LED_READY, LOW);
    digitalWrite(LED_BUSY, HIGH);
    motorsEnable();
    variableStepDelayUs = STEP_DELAY_US_FREE;
    moveXY_DDA(0, -LINE_H * 1.5, variableStepDelayUs);
    variableStepDelayUs = STEP_DELAY_US;
    state = STATE_IDLE;
    heightAlreadySet = false;
    break;

  case RESET_POSITION:
    penUp();
    countPosition = false;
    calculatedResetX = maxWidth - resetPositionX;

    moveXY_DDA(calculatedResetX + SPACE_W, -resetPositionY, STEP_DELAY_US);
    resetPositionX = 0.0;
    resetPositionY = 0.0;
    calculatedResetX = 0.0;
    calculatedResetY = 0.0;
    maxWidth = 0.0;
    state = STATE_IDLE;
    break;
  default:
    break;
  }
}
// Stift anheben
void penUp()
{
  penServo.write(SERVO_UP);
}
// Stift absenken
void penDown()
{
  penServo.write(SERVO_DOWN);
}
// Bewegung in X und Y Richtung

void moveXY_DDA(float x_cm, float y_cm, int stepDelayUs)
{
  switch (draw_state)
  {
  case 0:
    draw_state = 1;
    break;
  case 1:
    draw_
    break;
  
  default:
    break;
  }
  stepsX = lround(fabs(x_cm * STEPS_PER_CM));
  stepsY = lround(fabs(y_cm * STEPS_PER_CM));

  currentX = currentX + x_cm;

  if (countPosition)
  {
    resetPositionX = resetPositionX + x_cm;
    resetPositionY = resetPositionY + y_cm;

    if (resetPositionX > maxWidth)
      maxWidth = resetPositionX;
  }

  digitalWrite(DIR1_PIN, x_cm >= 0 ? LOW : HIGH);
  digitalWrite(DIR2_PIN, y_cm >= 0 ? HIGH : LOW);

  maxSteps = max(stepsX, stepsY);
  if (maxSteps == 0)
    return;

  errorX = 0;
  errorY = 0;
}

void moveXY_DDA(float x_cm, float y_cm, int stepDelayUs)
{
  for (long i = 0; i < maxSteps; i++)
  {
    

    delayMicroseconds(stepDelayUs);

    digitalWrite(STEP1_PIN, LOW);
    digitalWrite(STEP2_PIN, LOW);

    delayMicroseconds(stepDelayUs);
  }
}

void drawArc(float radius_cm, float startAngle, float endAngle, int segments, int stepDelayUs)
{
  stepDelayUs = stepDelayUs * 2;
  float angleStep = (endAngle - startAngle) / segments;

  float prevX = radius_cm * cos(startAngle);
  float prevY = radius_cm * sin(startAngle);

  for (int i = 1; i <= segments; i++)
  {
    float theta = startAngle + i * angleStep;

    float x = radius_cm * cos(theta);
    float y = radius_cm * sin(theta);

    float dx = x - prevX;
    float dy = y - prevY;

    moveXY_DDA(dx, dy, stepDelayUs);

    prevX = x;
    prevY = y;
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
  penUp();
  moveXY_DDA(0, r, variableStepDelayUs);
  penDown();
  drawArc(r, PI, 2 * PI, 20, variableStepDelayUs);
  moveXY_DDA(0, LETTER_H / 3, variableStepDelayUs);
  drawArc(r, 0, PI, 20, variableStepDelayUs);
  moveXY_DDA(0, -LETTER_H / 3, variableStepDelayUs);
  penUp();
}

void draw1()
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
    moveXY_DDA(-LETTER_W / 3, -LETTER_H / 4, variableStepDelayUs);
    break;
  case 3:
    penUp();
    state = STATE_IDLE;
    actualLetterDone = true;
    break;
  }
}

void draw2()
{
  float r = LETTER_H / 4;
  penUp();
  moveXY_DDA(0, LETTER_H * 0.8, variableStepDelayUs);
  penDown();
  drawArc(r, PI, 0, 30, variableStepDelayUs);
  moveXY_DDA(-2 * r, -(LETTER_H * 0.67), variableStepDelayUs);
  moveXY_DDA(2 * r, 0, variableStepDelayUs);
  penUp();
}

void draw3()
{
  float r = LETTER_H / 4;
  penUp();
  moveXY_DDA(0, LETTER_H, variableStepDelayUs);
  penDown();
  moveXY_DDA(LETTER_W / 4, 0, variableStepDelayUs);
  drawArc(r, PI / 2, -PI / 2, 30, variableStepDelayUs);
  drawArc(r, PI / 2, -PI / 2, 30, variableStepDelayUs);
  moveXY_DDA(-LETTER_W / 4, 0, variableStepDelayUs);
  penUp();
}

void draw4()
{
  penUp();
  moveXY_DDA(0, LETTER_H, variableStepDelayUs);
  penDown();
  moveXY_DDA(0, -LETTER_H / 2, variableStepDelayUs);
  moveXY_DDA(LETTER_W / 2, 0, variableStepDelayUs);
  penUp();
  moveXY_DDA(-LETTER_W / 4, LETTER_H / 2, variableStepDelayUs);
  penDown();
  moveXY_DDA(0, -LETTER_H, variableStepDelayUs);
  penUp();
}

void draw5()
{
  float r = LETTER_H / 4;
  penDown();
  moveXY_DDA(LETTER_W / 4, 0, variableStepDelayUs);
  drawArc(r, -PI / 2, PI / 2, 20, variableStepDelayUs);
  moveXY_DDA(-LETTER_W / 4, 0, variableStepDelayUs);
  moveXY_DDA(0, LETTER_H / 2, variableStepDelayUs);
  moveXY_DDA(r + LETTER_W / 4, 0, variableStepDelayUs);
  penUp();
}

void draw6()
{
  float r = LETTER_H / 4;

  moveXY_DDA(r, 0, variableStepDelayUs);
  penDown();
  drawArc(r, PI * 3 / 2, -PI / 2, 20, variableStepDelayUs);
  penUp();
  drawArc(r, -PI / 2, PI, 20, variableStepDelayUs);
  penDown();
  moveXY_DDA(0, (LETTER_H / 3) + r, variableStepDelayUs);
  drawArc(r, PI, PI * 0.2, 20, variableStepDelayUs);
  penUp();
}

void draw7()
{
  penDown();
  moveXY_DDA(LETTER_W * 0.7, LETTER_H, variableStepDelayUs);
  moveXY_DDA(-LETTER_W * 0.7, 0, variableStepDelayUs);
  penUp();
}

void draw8()
{
  float r = LETTER_H / 4;
  penUp();
  moveXY_DDA(r, LETTER_H * 0.75, variableStepDelayUs);
  penDown();
  drawArc(r, 0, 2 * PI, 40, variableStepDelayUs);
  penUp();
  moveXY_DDA(0, -LETTER_H / 2, variableStepDelayUs);
  penDown();
  drawArc(r, 0, 2 * PI, 40, variableStepDelayUs);
  penUp();
}

void draw9()
{
  float r = LETTER_H / 4;

  moveXY_DDA(0, r, variableStepDelayUs);
  penDown();
  drawArc(r, PI * 1.1, 2 * PI, 20, variableStepDelayUs);
  moveXY_DDA(0, LETTER_H / 2, variableStepDelayUs);
  drawArc(r, 0, 2 * PI, 20, variableStepDelayUs);
  penUp();
}

void drawDot()
{
  penDown();
  moveXY_DDA(0, 0.1, variableStepDelayUs);
  penUp();
}

void drawComma()
{
  penDown();
  moveXY_DDA(0.1, -0.15, variableStepDelayUs);
  penUp();
}

void drawExclamation()
{
  penDown();
  moveXY_DDA(0, LETTER_H * 0.8, variableStepDelayUs);
  penUp();
  moveXY_DDA(0, -LETTER_H, variableStepDelayUs);
  penDown();
  moveXY_DDA(0, 0.1, variableStepDelayUs);
  penUp();
}

void drawQuestion()
{
  float r = LETTER_H / 4;
  penUp();
  moveXY_DDA(0, LETTER_H, variableStepDelayUs);
  penDown();
  drawArc(r, PI, 2 * PI, 20, variableStepDelayUs);
  moveXY_DDA(0, -LETTER_H / 2, variableStepDelayUs);
  penUp();
  moveXY_DDA(0, -LETTER_H / 2, variableStepDelayUs);
  penDown();
  moveXY_DDA(0, 0.1, variableStepDelayUs);
  penUp();
}

void drawColon()
{
  moveXY_DDA(0, LETTER_H / 4, variableStepDelayUs);
  penDown();
  moveXY_DDA(0, 0.06, variableStepDelayUs);
  penUp();
  moveXY_DDA(0, LETTER_H / 4, variableStepDelayUs);
  penDown();
  moveXY_DDA(0, 0.06, variableStepDelayUs);
  penUp();
}

void drawEqual()
{
  moveXY_DDA(0, LETTER_H / 4, variableStepDelayUs);
  penDown();
  moveXY_DDA(LETTER_W / 3, 0, variableStepDelayUs);
  penUp();
  moveXY_DDA(-LETTER_W / 3, LETTER_H / 6, variableStepDelayUs);
  penDown();
  moveXY_DDA(LETTER_W / 3, 0, variableStepDelayUs);
  penUp();
}

void drawPlus()
{
  moveXY_DDA(0, LETTER_H / 2, variableStepDelayUs);
  penDown();
  moveXY_DDA(LETTER_W / 3, 0, variableStepDelayUs);
  penUp();
  moveXY_DDA(-LETTER_W / 6, LETTER_H / 6, variableStepDelayUs);
  penDown();
  moveXY_DDA(0, -LETTER_H / 3, variableStepDelayUs);
  penUp();
}

void drawMinus()
{
  moveXY_DDA(0, LETTER_H / 2, variableStepDelayUs);
  penDown();
  moveXY_DDA(LETTER_W / 3, 0, variableStepDelayUs);
  penUp();
}

void drawSlash()
{
  penDown();
  moveXY_DDA(LETTER_W / 2, LETTER_H, variableStepDelayUs);
  penUp();
}
void drawPercent()
{
  penDown();
  moveXY_DDA(LETTER_W / 2, LETTER_H, variableStepDelayUs);
  penUp();
  moveXY_DDA(-LETTER_W / 2, 0, variableStepDelayUs);
  penDown();
  drawArc(0.05, 0, 2 * PI, 20, variableStepDelayUs);
  penUp();
  moveXY_DDA(LETTER_W / 2, -(LETTER_H - 0.1), variableStepDelayUs);
  penDown();
  drawArc(0.05, 0, 2 * PI, 20, variableStepDelayUs);
  penUp();
}

void drawParenthesisOpen()
{
  float r = LETTER_H / 2;
  penDown();
  drawArc(r, PI / 2, 3 * PI / 2, 30, variableStepDelayUs);
  penUp();
}

void drawParenthesisClose()
{
  float r = LETTER_H / 2;
  penDown();
  drawArc(r, -PI / 2, PI / 2, 30, variableStepDelayUs);
  penUp();
}

void drawQuote()
{
  moveXY_DDA(0, LETTER_H, variableStepDelayUs);
  penDown();
  moveXY_DDA(0, -LETTER_H * 0.2, variableStepDelayUs);
  penUp();
  moveXY_DDA(SPACE_W / 2, LETTER_H * 0.2, variableStepDelayUs);
  penDown();
  moveXY_DDA(0, -LETTER_H * 0.2, variableStepDelayUs);
  penUp();
}

void drawA()
{
  penDown();
  moveXY_DDA(LETTER_W / 4, LETTER_H, variableStepDelayUs);
  moveXY_DDA(LETTER_W / 4, -LETTER_H, variableStepDelayUs);
  penUp();
  moveXY_DDA(-LETTER_W / 2.5, LETTER_H / 2, variableStepDelayUs);
  penDown();
  moveXY_DDA(LETTER_W / 4, 0, variableStepDelayUs);
  penUp();
}

void drawB()
{
  float r = LETTER_H / 4;

  penDown();
  moveXY_DDA(0, LETTER_H, variableStepDelayUs); // linker Stamm

  moveXY_DDA(LETTER_W / 8, 0, variableStepDelayUs);
  drawArc(r, PI / 2, -PI / 2, 30, variableStepDelayUs); // obere Rundung
  moveXY_DDA(-LETTER_W / 8, 0, variableStepDelayUs);
  moveXY_DDA(LETTER_W / 6, 0, variableStepDelayUs);
  drawArc(r, PI / 2, -PI / 2, 30, variableStepDelayUs); // untere Rundung
  moveXY_DDA(-LETTER_W / 6, 0, variableStepDelayUs);
  penUp();
}

void drawC()
{
  float r = LETTER_H / 2;

  penUp();
  moveXY_DDA(r + LETTER_W / 6, LETTER_H, variableStepDelayUs);

  penDown();
  moveXY_DDA(-LETTER_W / 6, 0, variableStepDelayUs);
  drawArc(r, PI / 2, 3 * PI / 2, 30, variableStepDelayUs);
  moveXY_DDA(LETTER_W / 6, 0, variableStepDelayUs);
  penUp();
}

void drawD()
{
  float r = LETTER_H / 2;

  penDown();
  moveXY_DDA(0, LETTER_H, variableStepDelayUs); // linker Stamm

  drawArc(r, PI / 2, -PI / 2, 60, variableStepDelayUs); // Rundung
  penUp();
}

void drawE()
{
  penDown();
  moveXY_DDA(0, LETTER_H, variableStepDelayUs);
  moveXY_DDA(LETTER_W / 1.5, 0, variableStepDelayUs);
  penUp();
  moveXY_DDA(-LETTER_W / 1.5, -LETTER_H / 2, variableStepDelayUs);
  penDown();
  moveXY_DDA(LETTER_W / 2, 0, variableStepDelayUs);
  penUp();
  moveXY_DDA(-LETTER_W / 2, -LETTER_H / 2, variableStepDelayUs);
  penDown();
  moveXY_DDA(LETTER_W / 1.5, 0, variableStepDelayUs);
  penUp();
}

void drawF()
{
  penDown();
  moveXY_DDA(0, LETTER_H, variableStepDelayUs);
  moveXY_DDA(LETTER_W, 0, variableStepDelayUs);
  penUp();
  moveXY_DDA(-LETTER_W, -LETTER_H / 2, variableStepDelayUs);
  penDown();
  moveXY_DDA(LETTER_W * 0.7, 0, variableStepDelayUs);
  penUp();
}

void drawG()
{
  float r = LETTER_H / 2;

  penUp();
  moveXY_DDA(LETTER_W, r, variableStepDelayUs);

  penDown();
  drawArc(r, 0, 2 * PI - 0.3, 80, variableStepDelayUs);

  moveXY_DDA(-r / 2, 0, variableStepDelayUs);
  penUp();
  moveXY_DDA(r / 3, 0, variableStepDelayUs);
  penDown();
  moveXY_DDA(0, -LETTER_H / 3, variableStepDelayUs);

  penUp();
}

void drawH()
{
  penDown();
  moveXY_DDA(0, LETTER_H, variableStepDelayUs);
  penUp();
  moveXY_DDA(LETTER_W / 2, 0, variableStepDelayUs);
  penDown();
  moveXY_DDA(0, -LETTER_H, variableStepDelayUs);
  penUp();
  moveXY_DDA(-LETTER_W / 2, LETTER_H / 2, variableStepDelayUs);
  penDown();
  moveXY_DDA(LETTER_W / 2, 0, variableStepDelayUs);
  penUp();
}

void drawI()
{
  penDown();
  moveXY_DDA(LETTER_W / 2, 0, variableStepDelayUs);
  moveXY_DDA(-LETTER_W / 4, 0, variableStepDelayUs);
  moveXY_DDA(0, LETTER_H, variableStepDelayUs);
  penUp();
  moveXY_DDA(-LETTER_W / 4, 0, variableStepDelayUs);
  penDown();
  moveXY_DDA(LETTER_W / 2, 0, variableStepDelayUs);
  penUp();
}

void drawJ()
{
  float r = LETTER_H * 0.2;
  moveXY_DDA(0, r, variableStepDelayUs);
  penDown();
  drawArc(r, PI, 2 * PI, 15, variableStepDelayUs);
  moveXY_DDA(0, LETTER_H * 0.7, variableStepDelayUs); // Stamm
  penUp();
}

void drawK()
{
  penDown();
  moveXY_DDA(0, LETTER_H, variableStepDelayUs);
  penUp();
  moveXY_DDA(LETTER_W / 2, -LETTER_H, variableStepDelayUs);
  penDown();
  moveXY_DDA(-LETTER_W / 2, LETTER_H / 2, variableStepDelayUs);
  moveXY_DDA(LETTER_W / 2, LETTER_H / 2, variableStepDelayUs);
  penUp();
}

void drawL()
{
  moveXY_DDA(0, LETTER_H, variableStepDelayUs);
  penDown();
  moveXY_DDA(0, -LETTER_H, variableStepDelayUs);
  moveXY_DDA(LETTER_W / 2, 0, variableStepDelayUs);
  penUp();
}

void drawM()
{
  penDown();
  moveXY_DDA(0, LETTER_H, variableStepDelayUs);
  moveXY_DDA(LETTER_W / 2, -LETTER_H / 2, variableStepDelayUs);
  moveXY_DDA(LETTER_W / 2, LETTER_H / 2, variableStepDelayUs);
  moveXY_DDA(0, -LETTER_H, variableStepDelayUs);
  penUp();
}

void drawN()
{
  penDown();
  moveXY_DDA(0, LETTER_H, variableStepDelayUs);
  moveXY_DDA(LETTER_W, -LETTER_H, variableStepDelayUs);
  moveXY_DDA(0, LETTER_H, variableStepDelayUs);
  penUp();
}

void drawO()
{
  float r = LETTER_H / 2;

  penUp();
  moveXY_DDA(LETTER_W, r, variableStepDelayUs);

  penDown();
  drawArc(r, 0, 2 * PI, 20, variableStepDelayUs);

  penUp();
}

void drawP()
{
  float r = LETTER_H / 4;

  penDown();
  moveXY_DDA(0, LETTER_H, variableStepDelayUs); // linker Stamm

  moveXY_DDA(LETTER_W / 8, 0, variableStepDelayUs);
  drawArc(r, PI / 2, -PI / 2, 30, variableStepDelayUs);
  moveXY_DDA(-LETTER_W / 8, 0, variableStepDelayUs);
  penUp();
}

void drawQ()
{
  float r = LETTER_H / 2;

  moveXY_DDA(0, LETTER_H / 2, variableStepDelayUs);

  penUp();
  moveXY_DDA(LETTER_W, 0, variableStepDelayUs);

  penDown();
  drawArc(r, 0, 2 * PI, 20, variableStepDelayUs);
  penUp();
  moveXY_DDA(-r / 3, -r / 2, variableStepDelayUs);
  penDown();
  moveXY_DDA(r / 3, -r / 3, variableStepDelayUs);

  penUp();
}

void drawR()
{
  float r = LETTER_H / 4;

  penDown();
  moveXY_DDA(0, LETTER_H, variableStepDelayUs); // linker Stamm

  moveXY_DDA(LETTER_W / 8, 0, variableStepDelayUs);
  drawArc(r, PI / 2, -PI / 2, 30, variableStepDelayUs);
  moveXY_DDA(-LETTER_W / 8, 0, variableStepDelayUs);
  moveXY_DDA(LETTER_W / 2, -LETTER_H / 2, variableStepDelayUs);
  penUp();
}

void drawS()
{
  float r = LETTER_H / 4;

  penDown();
  moveXY_DDA(LETTER_W / 5, 0, variableStepDelayUs);
  penDown();
  drawArc(r, -PI / 2, PI / 2, 20, variableStepDelayUs);
  drawArc(r, (PI * 3) / 2, PI / 2, 20, variableStepDelayUs);
  moveXY_DDA(LETTER_W / 5, 0, variableStepDelayUs);
  penUp();
}

void drawT()
{
  moveXY_DDA(0, LETTER_H, variableStepDelayUs);
  penDown();
  moveXY_DDA(LETTER_W, 0, variableStepDelayUs);
  penUp();
  moveXY_DDA(-LETTER_W / 2, 0, variableStepDelayUs);
  penDown();
  moveXY_DDA(0, -LETTER_H, variableStepDelayUs);
  penUp();
}
void drawU()
{
  float r = LETTER_W / 2;

  moveXY_DDA(0, LETTER_H, variableStepDelayUs);

  penDown();
  moveXY_DDA(0, -(LETTER_H - r), variableStepDelayUs);

  drawArc(r, -PI, 0, 30, variableStepDelayUs);

  moveXY_DDA(0, LETTER_H - r, variableStepDelayUs);

  penUp();
}

void drawV()
{
  moveXY_DDA(0, LETTER_H, variableStepDelayUs);
  penDown();
  moveXY_DDA(LETTER_W / 2, -LETTER_H, variableStepDelayUs);
  moveXY_DDA(LETTER_W / 2, LETTER_H, variableStepDelayUs);
  penUp();
}

void drawW()
{
  moveXY_DDA(0, LETTER_H, variableStepDelayUs);
  penDown();
  moveXY_DDA(LETTER_W / 4, -LETTER_H, variableStepDelayUs);
  moveXY_DDA(LETTER_W / 4, LETTER_H / 2, variableStepDelayUs);
  moveXY_DDA(LETTER_W / 4, -LETTER_H / 2, variableStepDelayUs);
  moveXY_DDA(LETTER_W / 4, LETTER_H, variableStepDelayUs);
  penUp();
}

void drawX()
{
  penDown();
  moveXY_DDA(LETTER_W * 0.75, LETTER_H, variableStepDelayUs);
  penUp();
  moveXY_DDA(-LETTER_W * 0.75, 0, variableStepDelayUs);
  penDown();
  moveXY_DDA(LETTER_W * 0.75, -LETTER_H + LETTER_H / 4, variableStepDelayUs);
  penUp();
}

void drawY()
{
  penUp();
  moveXY_DDA(0, LETTER_H, variableStepDelayUs);
  penDown();
  moveXY_DDA(LETTER_W / 2, -LETTER_H / 2, variableStepDelayUs);
  moveXY_DDA(LETTER_W / 2, LETTER_H / 2, variableStepDelayUs);
  penUp();
  moveXY_DDA(-LETTER_W / 2, -LETTER_H / 2, variableStepDelayUs);
  penDown();
  moveXY_DDA(0, -LETTER_H / 2, variableStepDelayUs);
  penUp();
}

void drawZ()
{
  moveXY_DDA(LETTER_W, 0, variableStepDelayUs);
  penDown();
  moveXY_DDA(-LETTER_W, 0, variableStepDelayUs);
  moveXY_DDA(LETTER_W, LETTER_H, variableStepDelayUs);
  moveXY_DDA(-LETTER_W, 0, variableStepDelayUs);
  penUp();
}

// Funktionen der Kleinbuchstaben

void drawa()
{
  float r = LETTER_H * 0.25;

  // Start rechts oben vom Kreis
  penUp();
  moveXY_DDA(LETTER_W / 2, 0, variableStepDelayUs);
  // Kreis (unterer Teil)
  penDown();
  moveXY_DDA(0, LETTER_H * 0.6, variableStepDelayUs);
  penUp();
  moveXY_DDA(0, -LETTER_H * 0.3, variableStepDelayUs);
  penDown();
  drawArc(r, 0, 2 * PI, 40, variableStepDelayUs);
  penUp();
}

void drawb()
{
  float r = LETTER_H / 4;

  penDown();
  moveXY_DDA(0, LETTER_H, variableStepDelayUs); // linker Stamm
  penUp();
  moveXY_DDA(0, -LETTER_H / 2, variableStepDelayUs);
  penDown();
  moveXY_DDA(LETTER_W / 6, 0, variableStepDelayUs);
  drawArc(r, PI / 2, -PI / 2, 30, variableStepDelayUs); // untere Rundung
  moveXY_DDA(-LETTER_W / 6, 0, variableStepDelayUs);
  penUp();
}

void drawc()
{
  float r = LETTER_H * 0.3;
  penUp();
  moveXY_DDA(r * 2, LETTER_H * 0.5, variableStepDelayUs);
  penDown();
  drawArc(r, PI * 0.3, PI * 1.7, 20, variableStepDelayUs);
  penUp();
}

void drawd()
{
  float r = LETTER_H * 0.25;
  penUp();
  moveXY_DDA(r * 2, LETTER_H, variableStepDelayUs);
  penDown();
  moveXY_DDA(0, -LETTER_H, variableStepDelayUs);
  moveXY_DDA(-LETTER_W * 0.2, 0, variableStepDelayUs);
  drawArc(r, (PI * 3) / 2, PI / 2, 20, variableStepDelayUs);
  moveXY_DDA(LETTER_W * 0.2, 0, variableStepDelayUs);
  penUp();
}

void drawe()
{
  float r = LETTER_H * 0.3;
  penUp();
  moveXY_DDA(0, LETTER_H * 0.35, variableStepDelayUs);
  penDown();
  moveXY_DDA(r * 2, 0, variableStepDelayUs);        // Mittellinie
  drawArc(r, 0, PI * 1.7, 20, variableStepDelayUs); // obere Hälfte
  penUp();
}

void drawf()
{
  float r = LETTER_H * 0.2;
  penUp();
  moveXY_DDA(r * 1.5, LETTER_H, variableStepDelayUs);
  penDown();
  drawArc(r, PI / 2, PI, 20, variableStepDelayUs);     // Haken oben
  moveXY_DDA(0, -LETTER_H * 0.8, variableStepDelayUs); // Stamm runter
  penUp();
  moveXY_DDA(-r * 0.5, LETTER_H * 0.5, variableStepDelayUs);
  penDown();
  moveXY_DDA(r * 1.5, 0, variableStepDelayUs); // Querstrich
  penUp();
}

void drawg()
{
  float r = LETTER_H * 0.2;
  penUp();
  moveXY_DDA(0, -LETTER_H * 0.2, variableStepDelayUs);
  penDown();
  drawArc(r, PI, 2 * PI, 15, variableStepDelayUs);
  moveXY_DDA(0, LETTER_H * 0.7, variableStepDelayUs); // Stamm
  penUp();
  moveXY_DDA(0, -LETTER_H * 0.2, variableStepDelayUs);
  penDown();
  drawArc(r, 0, 2 * PI, 15, variableStepDelayUs);
  penUp();
}

void drawh()
{
  float r = LETTER_H * 0.2;
  penDown();
  moveXY_DDA(0, LETTER_H, variableStepDelayUs);
  penUp();
  moveXY_DDA(0, -LETTER_H / 1.5, variableStepDelayUs);
  penDown();
  drawArc(r, PI, 0, 20, variableStepDelayUs); // kurzer Schulter-Bogen
  moveXY_DDA(0, -(LETTER_H - LETTER_H / 1.5), variableStepDelayUs);
  penUp();
}

void drawi()
{
  penDown();
  moveXY_DDA(0, LETTER_H * 0.5, variableStepDelayUs); // Stamm
  penUp();
  moveXY_DDA(0, LETTER_H / 4, variableStepDelayUs);
  penDown();
  moveXY_DDA(0, LETTER_H * 0.05, variableStepDelayUs); // Punkt
  penUp();
}

void drawj()
{
  float r = LETTER_H * 0.15;
  penDown();
  drawArc(r, PI * 1.4, 2 * PI, 15, variableStepDelayUs);
  moveXY_DDA(0, LETTER_H * 0.5, variableStepDelayUs); // Stamm
  penUp();
  moveXY_DDA(0, LETTER_H / 4, variableStepDelayUs);
  penDown();
  moveXY_DDA(0, LETTER_H * 0.05, variableStepDelayUs); // Punkt
  penUp();
}

void drawk()
{
  penDown();
  moveXY_DDA(0, LETTER_H, variableStepDelayUs);
  penUp();
  moveXY_DDA(LETTER_W * 0.3, -LETTER_H * 0.5, variableStepDelayUs);
  penDown();
  moveXY_DDA(-LETTER_W * 0.3, -LETTER_H * 0.25, variableStepDelayUs);
  moveXY_DDA(LETTER_W * 0.3, -LETTER_H * 0.25, variableStepDelayUs);
  penUp();
}

void drawl()
{
  penUp();
  moveXY_DDA(0, LETTER_H, variableStepDelayUs);
  penDown();
  moveXY_DDA(0, -LETTER_H, variableStepDelayUs);
  moveXY_DDA(LETTER_W * 0.08, 0, variableStepDelayUs);
  penUp();
}

void drawm()
{
  float r = LETTER_H * 0.2;
  penDown();
  moveXY_DDA(0, LETTER_H * 0.7, variableStepDelayUs);
  penUp();
  moveXY_DDA(0, -LETTER_H * 0.25, variableStepDelayUs);
  penDown();
  drawArc(r, PI, PI * 0.1, 20, variableStepDelayUs); // kurzer Schulter-Bogen
  moveXY_DDA(0, -(LETTER_H * 0.7 - LETTER_H * 0.25), variableStepDelayUs);
  penUp();
  moveXY_DDA(0, LETTER_H * 0.6 - LETTER_H * 0.25, variableStepDelayUs);
  penDown();
  drawArc(r, PI, PI * 0.1, 20, variableStepDelayUs); // kurzer Schulter-Bogen
  moveXY_DDA(0, -(LETTER_H * 0.7 - LETTER_H * 0.25), variableStepDelayUs);
  penUp();
}

void drawn()
{
  float r = LETTER_H * 0.2;
  penDown();
  moveXY_DDA(0, LETTER_H * 0.7, variableStepDelayUs);
  penUp();
  moveXY_DDA(0, -LETTER_H * 0.25, variableStepDelayUs);
  penDown();
  drawArc(r, PI, PI * 0.1, 20, variableStepDelayUs); // kurzer Schulter-Bogen
  moveXY_DDA(0, -(LETTER_H * 0.7 - LETTER_H * 0.25), variableStepDelayUs);
  penUp();
}

void drawo()
{
  float r = LETTER_H * 0.3;
  penUp();
  moveXY_DDA(2 * r, r, variableStepDelayUs);
  penDown();
  drawArc(r, 0, 2 * PI, 20, variableStepDelayUs);
  penUp();
}

void drawp()
{
  float r = LETTER_H / 4;
  penDown();
  moveXY_DDA(LETTER_W / 6, 0, variableStepDelayUs);
  drawArc(r, -PI / 2, PI / 2, 30, variableStepDelayUs); // untere Rundung
  moveXY_DDA(-LETTER_W / 6, 0, variableStepDelayUs);
  moveXY_DDA(0, -LETTER_H, variableStepDelayUs);
  moveXY_DDA(0, LETTER_H - 2 * r, variableStepDelayUs);
  penUp();
}

void drawq()
{
  penUp();
  moveXY_DDA(SPACE_W * 1.5, 0, variableStepDelayUs);
  float r = LETTER_H / 4;
  penDown();
  moveXY_DDA(-LETTER_W / 6, 0, variableStepDelayUs);
  drawArc(r, (PI * 3) / 2, PI / 2, 30, variableStepDelayUs); // untere Rundung
  moveXY_DDA(LETTER_W / 6, 0, variableStepDelayUs);
  moveXY_DDA(0, -LETTER_H, variableStepDelayUs);
  moveXY_DDA(0, LETTER_H - 2 * r, variableStepDelayUs);
  penUp();
}

void drawr()
{
  float r = LETTER_H * 0.2;
  penDown();
  moveXY_DDA(0, LETTER_H * 0.7, variableStepDelayUs);
  penUp();
  moveXY_DDA(0, -LETTER_H * 0.25, variableStepDelayUs);
  penDown();
  drawArc(r, PI, PI * 0.1, 20, variableStepDelayUs); // kurzer Schulter-Bogen
  penUp();
}

void draws()
{
  float r = LETTER_H / 7;
  penDown();
  moveXY_DDA(LETTER_W / 6, 0, variableStepDelayUs);
  drawArc(r, -PI / 2, PI / 2, 20, variableStepDelayUs);
  drawArc(r, (PI * 3) / 2, PI / 2, 20, variableStepDelayUs);
  moveXY_DDA(LETTER_W / 6, 0, variableStepDelayUs);
  penUp();
}

void drawt()
{
  float r = LETTER_H * 0.1;
  penUp();
  moveXY_DDA(LETTER_W * 0.1, LETTER_H, variableStepDelayUs);
  penDown();
  moveXY_DDA(0, -(LETTER_H - r), variableStepDelayUs);
  drawArc(r, PI, 2 * PI, 15, variableStepDelayUs); // Fußhaken
  penUp();
  moveXY_DDA(-LETTER_W * 0.3 - r, LETTER_H * 0.65, variableStepDelayUs);
  penDown();
  moveXY_DDA(LETTER_W * 0.5, 0, variableStepDelayUs); // Querstrich
  penUp();
}

void drawu()
{
  float r = LETTER_H * 0.25;
  penUp();
  moveXY_DDA(0, LETTER_H * 0.7, variableStepDelayUs);
  penDown();
  moveXY_DDA(0, -(LETTER_H * 0.7 - r), variableStepDelayUs);
  drawArc(r, PI, 2 * PI, 30, variableStepDelayUs); // Unterer Bogen
  moveXY_DDA(0, LETTER_H * 0.7 - r, variableStepDelayUs);
  penUp();
  moveXY_DDA(0, -(LETTER_H * 0.7 - r), variableStepDelayUs);
  penDown();
  moveXY_DDA(0, -r, variableStepDelayUs);
  penUp();
}

void drawv()
{
  penUp();
  moveXY_DDA(0, LETTER_H * 0.65, variableStepDelayUs);
  penDown();
  moveXY_DDA(LETTER_W * 0.4, -LETTER_H * 0.61, variableStepDelayUs);
  moveXY_DDA(LETTER_W * 0.4, LETTER_H * 0.61, variableStepDelayUs);
  penUp();
}

void draww()
{
  penUp();
  moveXY_DDA(0, LETTER_H * 0.55, variableStepDelayUs);
  penDown();
  moveXY_DDA(LETTER_W / 4, -LETTER_H * 0.55, variableStepDelayUs);
  moveXY_DDA(LETTER_W / 4, LETTER_H * 0.35, variableStepDelayUs);
  moveXY_DDA(LETTER_W / 4, -LETTER_H * 0.35, variableStepDelayUs);
  moveXY_DDA(LETTER_W / 4, LETTER_H * 0.55, variableStepDelayUs);
  penUp();
}

void drawx()
{
  penUp();
  moveXY_DDA(0, LETTER_H * 0.5, variableStepDelayUs);
  penDown();
  moveXY_DDA(LETTER_W * 0.6, -LETTER_H * 0.5, variableStepDelayUs);
  penUp();
  moveXY_DDA(-LETTER_W * 0.6, 0, variableStepDelayUs);
  penDown();
  moveXY_DDA(LETTER_W * 0.6, LETTER_H * 0.5, variableStepDelayUs);
  penUp();
}

void drawy()
{
  penUp();
  moveXY_DDA(LETTER_W / 3, -LETTER_H / 2, variableStepDelayUs);
  penDown();
  moveXY_DDA(LETTER_W / 3, LETTER_H, variableStepDelayUs);
  penUp();
  moveXY_DDA(-LETTER_W / 2, 0, variableStepDelayUs);
  penDown();
  moveXY_DDA(LETTER_W / 3, -LETTER_H / 2, variableStepDelayUs);
  penUp();
}

void drawz()
{
  penUp();
  moveXY_DDA(0, LETTER_H * 0.65, variableStepDelayUs);
  penDown();
  moveXY_DDA(LETTER_W * 0.7, 0, variableStepDelayUs);
  moveXY_DDA(-LETTER_W * 0.7, -LETTER_H * 0.55, variableStepDelayUs);
  moveXY_DDA(LETTER_W * 0.7, 0, variableStepDelayUs);
  penUp();
}

void drawSpace()
{
  penUp();
  moveXY_DDA(LETTER_W / 2, 0, STEP_DELAY_US);
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
}

struct Entry
{
  char key;
  void (*func)();
};

Entry letters[] = {
    {' ', drawSpace},

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

void drawLetter(char c)
{
  resetPositionX = 0.0;
  resetPositionY = 0.0;
  countPosition = true;

  for (char i = '!'; i < '}'; i++)
  {
    if (letters[i - '!'].key == c)
    {
      letterToDraw = i;
      break;
    }
  }
}

void loop()
{
  MeinTaster.update();

  read();
  transsitions();
  actions();
}