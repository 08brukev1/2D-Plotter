#include <Arduino.h>
#include <ESP32Servo.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <Bounce2.h>

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

float resetPositionX = 0.0;
float resetPositionY = 0.0;

float calculatedResetX = 0.0;
float calculatedResetY = 0.0;

bool countRealWidth = false;
float maxWidth = 0.0;

bool countPosition = false;
bool writeStatus = true;

// Servo Objekt
Servo penServo;

Bounce MeinTaster;
// Startpunkt
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
float currentX = 0.0;
float currentY = 0.0;
// Stift anheben
void penUp()
{
  penServo.write(SERVO_UP);
  countRealWidth = false;
  delay(400);
}
// Stift absenken
void penDown()
{
  penServo.write(SERVO_DOWN);
  countRealWidth = true;
  delay(200);
}
// Bewegung in X und Y Richtung

void moveXY_DDA(float x_cm, float y_cm, int stepDelayUs)
{
  MeinTaster.update();
  if (MeinTaster.rose())
  {
    writeStatus = !writeStatus; 
  }
  long stepsX = lround(fabs(x_cm * STEPS_PER_CM));
  long stepsY = lround(fabs(y_cm * STEPS_PER_CM));

  currentX = currentX + x_cm;

  if (countPosition)
  {
    resetPositionX = resetPositionX + x_cm;
    resetPositionY = resetPositionY + y_cm;

    if (resetPositionX > maxWidth)
    {
      maxWidth = resetPositionX;
    }
  }

  digitalWrite(DIR1_PIN, x_cm >= 0 ? LOW : HIGH);
  digitalWrite(DIR2_PIN, y_cm >= 0 ? HIGH : LOW);

  long maxSteps = max(stepsX, stepsY);
  if (maxSteps == 0)
    return;

  long errorX = 0;
  long errorY = 0;

  for (long i = 0; i < maxSteps; i++)
  {
    errorX += stepsX;
    errorY += stepsY;

    if (errorX >= maxSteps)
    {
      digitalWrite(STEP1_PIN, HIGH);
      errorX -= maxSteps;
    }

    if (errorY >= maxSteps)
    {
      digitalWrite(STEP2_PIN, HIGH);
      errorY -= maxSteps;
    }

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
// Neue Zeile
void newLine()
{
  penUp();
  variableStepDelayUs = STEP_DELAY_US_FREE;
  while (!digitalRead(Sensor1))
  {
    moveXY_DDA(-1, 0, variableStepDelayUs);
  }
  moveXY_DDA(0, -LINE_H, variableStepDelayUs);
  currentY = currentY + LINE_H;
  currentX = 0.0;
  variableStepDelayUs = STEP_DELAY_US;
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
  penDown();
  moveXY_DDA(0, LETTER_H, variableStepDelayUs);
  moveXY_DDA(-LETTER_W / 3, -LETTER_H / 4, variableStepDelayUs);
  penUp();
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
  drawArc(r, (PI*3) / 2, PI / 2, 20, variableStepDelayUs);
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
  drawArc(r, (PI*3) / 2, PI / 2, 20, variableStepDelayUs);
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
  penUp();

  MeinTaster.attach(Taster);
  MeinTaster.interval(40);

  u8g2.begin();

  digitalWrite(LED_READY, HIGH);
  digitalWrite(LED_BUSY, LOW);

  variableStepDelayUs = STEP_DELAY_US_FREE;
  while (!digitalRead(Sensor1))
  {
    moveXY_DDA(-1, 0, variableStepDelayUs);
  }
  while (!digitalRead(Sensor2))
  {
    moveXY_DDA(0, 1, variableStepDelayUs);
  }
  moveXY_DDA(0, -LETTER_H * 1.5, variableStepDelayUs);
  variableStepDelayUs = STEP_DELAY_US;
  motorsDisable();
}

struct Entry
{
  char key;
  void (*func)();
};

Entry actions[] = {
  {' ', drawSpace},

  {'a', drawa}, {'b', drawb}, {'c', drawc}, {'d', drawd}, {'e', drawe},
  {'f', drawf}, {'g', drawg}, {'h', drawh}, {'i', drawi}, {'j', drawj},
  {'k', drawk}, {'l', drawl}, {'m', drawm}, {'n', drawn}, {'o', drawo},
  {'p', drawp}, {'q', drawq}, {'r', drawr}, {'s', draws}, {'t', drawt},
  {'u', drawu}, {'v', drawv}, {'w', draww}, {'x', drawx}, {'y', drawy},
  {'z', drawz},

  {'A', drawA}, {'B', drawB}, {'C', drawC}, {'D', drawD}, {'E', drawE},
  {'F', drawF}, {'G', drawG}, {'H', drawH}, {'I', drawI}, {'J', drawJ},
  {'K', drawK}, {'L', drawL}, {'M', drawM}, {'N', drawN}, {'O', drawO},
  {'P', drawP}, {'Q', drawQ}, {'R', drawR}, {'S', drawS}, {'T', drawT},
  {'U', drawU}, {'V', drawV}, {'W', drawW}, {'X', drawX}, {'Y', drawY},
  {'Z', drawZ},

  {'0', draw0}, {'1', draw1}, {'2', draw2}, {'3', draw3}, {'4', draw4},
  {'5', draw5}, {'6', draw6}, {'7', draw7}, {'8', draw8}, {'9', draw9},

  {'.', drawDot}, {',', drawComma}, {'!', drawExclamation}, {'?', drawQuestion},
  {':', drawColon}, {'=', drawEqual}, {'+', drawPlus}, {'-', drawMinus},
  {'%', drawPercent},

  {'(', drawParenthesisOpen}, {')', drawParenthesisClose}, {'"', drawQuote}
};

void drawLetter(char c)
{
  if (currentX + LETTER_W > MAX_WIDTH)
  {
    if (currentY + LINE_H > MAX_HEIGTH)
    {
      variableStepDelayUs = STEP_DELAY_US_FREE;
      currentY = 0.0;
      while (!digitalRead(Sensor1))
      {
        moveXY_DDA(-1, 0, variableStepDelayUs);
      }
      while (!digitalRead(Sensor2))
      {
        moveXY_DDA(0, 1, variableStepDelayUs);
      }
      moveXY_DDA(0, -LETTER_H * 1.5, variableStepDelayUs);
      variableStepDelayUs = STEP_DELAY_US;
    }
    else
      newLine();
  }

  resetPositionX = 0.0;
  resetPositionY = 0.0;

  countPosition = true;

  for (char i = '!'; i < '}'; i++)
  {
    if (actions[i - '!'].key == c)
    {
      actions[i - '!'].func();
      break;
    }
  }

  countPosition = false;
  calculatedResetX = maxWidth - resetPositionX;

  moveXY_DDA(calculatedResetX + SPACE_W, -resetPositionY, STEP_DELAY_US);
  resetPositionX = 0.0;
  resetPositionY = 0.0;
  calculatedResetX = 0.0;
  calculatedResetY = 0.0;
  maxWidth = 0.0;
}

void loop()
{
  MeinTaster.update();

  if (Serial.available() && writeStatus)
  {
    digitalWrite(BUILTIN_LED, HIGH);
    motorsEnable();

    // === SENDEN SCREEN ===
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB14_tr);
    u8g2.drawStr(0, 20, "Senden...");
    u8g2.sendBuffer();

    char c = Serial.read();
    char bigCurrent[2] = {(c == ' ' ? '_' : c), '\0'}; // Leerzeichen → _

    Serial.println(c);

    digitalWrite(LED_BUSY, HIGH);
    digitalWrite(LED_READY, LOW);

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

    // === LOGIK (unverändert) ===
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
    }else if (c == '\r'){

    }else if ((c == '\n') && !settings)
      newLine();
    else if (!settings)
      drawLetter(c);
    

    digitalWrite(BUILTIN_LED, LOW);
    motorsDisable();
  }else{
    digitalWrite(LED_BUSY, LOW);
    digitalWrite(LED_READY, HIGH);

    if (MeinTaster.rose())
    {
      writeStatus = !writeStatus; 
    }

    // === READY SCREEN ===
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB14_tr);
    u8g2.drawStr(0, 20, "Ready to");
    u8g2.drawStr(0, 40, "read");
    u8g2.setFont(u8g2_font_ncenB10_tr);
    u8g2.drawStr(0, 60, "You can send!");
    u8g2.sendBuffer();
  }
}