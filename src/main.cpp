#include <Arduino.h>
#include <ESP32Servo.h>
#include <U8g2lib.h>
#include <Wire.h>

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
// #define LED_ON 26
// #define LED_OFF 27
// Taster
#define Taster 36
// Sensoren
#define Sensor1 14
#define Sensor2 33
// Step settings
#define STEPS_PER_MM 6.67
#define STEPS_PER_CM (STEPS_PER_MM * 10.0)
#define STEP_DELAY_US 1000
// Servo settings
#define SERVO_UP 30
#define SERVO_DOWN 90
// Letter settings
float LETTER_W = 0.8;
float LETTER_H = 1.0;
float SPACE_W = 0.2;
float LINE_H = 1.5;
float MAX_WIDTH = 11.0;

bool settings = false;
String wholeSetting = "";

// Servo Objekt
Servo penServo;
// Startpunkt
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
float currentX = 0.0;
// Stift anheben
void penUp()
{
  penServo.write(SERVO_UP);
  delay(400);
}
// Stift absenken
void penDown()
{
  penServo.write(SERVO_DOWN);
  delay(200);
}
// Bewegung in X und Y Richtung

void moveXY_DDA(float x_cm, float y_cm, int stepDelayUs)
{
  long stepsX = lround(fabs(x_cm * STEPS_PER_CM));
  long stepsY = lround(fabs(y_cm * STEPS_PER_CM));

  currentX = currentX + x_cm;

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

void drawCircle(float radius_cm)
{
  const int segments = 200; // Je höher desto runder
  const float angleStep = 2 * PI / segments;

  float prevX = radius_cm;
  float prevY = 0;

  penUp();
  moveXY_DDA(radius_cm, 0, 1200); // Zum Startpunkt
  penDown();

  for (int i = 1; i <= segments; i++)
  {
    float theta = i * angleStep;

    float x = radius_cm * cos(theta);
    float y = radius_cm * sin(theta);

    float dx = x - prevX;
    float dy = y - prevY;

    moveXY_DDA(dx, dy, 800); // Konstante Geschwindigkeit

    prevX = x;
    prevY = y;
  }

  penUp();
  moveXY_DDA(-radius_cm, 0, 1200); // Zurück zum Mittelpunkt
}

void drawArc(float radius_cm, float startAngle, float endAngle, int segments, int stepDelayUs)
{
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

  digitalWrite(ENABLE1_PIN, LOW);
  digitalWrite(ENABLE2_PIN, LOW);

  pinMode(LED_BUILTIN, OUTPUT);

  // pinMode(LED_OFF, OUTPUT);
  // pinMode(LED_ON, OUTPUT);

  pinMode(SERVO_PIN, OUTPUT);
  digitalWrite(SERVO_PIN, LOW);

  penServo.write(SERVO_UP);
  penServo.attach(SERVO_PIN, 500, 2400);
  penUp();

  u8g2.begin();

  motorsDisable();
}

// Neue Zeile
void newLine()
{
  penUp();
  moveXY_DDA(-currentX, -LINE_H, 1000);
  currentX = 0.0;
}

void drawA()
{
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

// Funktionen der Kleinbuchstaben

void drawa()
{
  float r = LETTER_H * 0.35;
  penUp();
  moveXY_DDA(r * 2, LETTER_H * 0.7, STEP_DELAY_US);
  penDown();
  drawArc(r, PI / 2, 5 * PI / 2, 60, STEP_DELAY_US);
  moveXY_DDA(0, -r, STEP_DELAY_US);
  penUp();
  moveXY_DDA(SPACE_W, r, STEP_DELAY_US);
}

void drawb()
{
  float r = LETTER_H / 4;

  penDown();
  moveXY_DDA(0, LETTER_H, 800); // linker Stamm
  penUp();
  moveXY_DDA(0, -LETTER_H / 2, 800);
  penDown();
  moveXY_DDA(LETTER_W / 6, 0, 1000);
  drawArc(r, PI / 2, -PI / 2, 30, 1000); // untere Rundung
  moveXY_DDA(-LETTER_W / 6, 0, 1000);
  delay(300);
  penUp();
  moveXY_DDA(LETTER_W + SPACE_W, 0, 1000);
}

void drawc()
{
  float r = LETTER_H * 0.3;
  penUp();
  moveXY_DDA(r * 2, LETTER_H * 0.4, STEP_DELAY_US);
  penDown();
  drawArc(r, PI * 0.3, PI * 1.7, 20, STEP_DELAY_US);
  penUp();
  moveXY_DDA(SPACE_W, LETTER_H * 0.25 * 0.4, STEP_DELAY_US);
}

void drawd()
{
  float r = LETTER_H * 0.25;
  penUp();
  moveXY_DDA(r * 2, LETTER_H, STEP_DELAY_US);
  penDown();
  moveXY_DDA(0, -LETTER_H, STEP_DELAY_US);
  moveXY_DDA(-LETTER_W * 0.2, 0, STEP_DELAY_US);
  drawArc(r, (PI * 3) / 2, PI / 2, 20, STEP_DELAY_US);
  moveXY_DDA(LETTER_W * 0.2, 0, STEP_DELAY_US);
  penUp();
  moveXY_DDA(SPACE_W, -2 * r, STEP_DELAY_US);
}

void drawe()
{
  float r = LETTER_H * 0.3;
  penUp();
  moveXY_DDA(0, LETTER_H * 0.35, STEP_DELAY_US);
  penDown();
  moveXY_DDA(r * 2, 0, STEP_DELAY_US);        // Mittellinie
  drawArc(r, 0, PI * 1.7, 20, STEP_DELAY_US); // obere Hälfte
  penUp();
  moveXY_DDA(SPACE_W, -LETTER_H * 0.1, STEP_DELAY_US);
}

void drawf()
{
  float r = LETTER_H * 0.2;
  penUp();
  moveXY_DDA(r * 1.5, LETTER_H, STEP_DELAY_US);
  penDown();
  drawArc(r, PI / 2, PI, 20, STEP_DELAY_US);     // Haken oben
  moveXY_DDA(0, -LETTER_H * 0.8, STEP_DELAY_US); // Stamm runter
  penUp();
  moveXY_DDA(-r * 0.5, LETTER_H * 0.5, STEP_DELAY_US);
  penDown();
  moveXY_DDA(r * 1.5, 0, STEP_DELAY_US); // Querstrich
  penUp();
  moveXY_DDA(SPACE_W, -LETTER_H * 0.5, STEP_DELAY_US);
}

void drawg()
{
  float r = LETTER_H * 0.35;
  penUp();
  moveXY_DDA(r * 2, LETTER_H * 0.7, STEP_DELAY_US);
  penDown();
  drawArc(r, PI / 2, 5 * PI / 2, 60, STEP_DELAY_US);   // Kreis
  moveXY_DDA(0, -(LETTER_H * 0.4 + r), STEP_DELAY_US); // Unterlänge
  drawArc(r, -PI / 2, PI / 2, 30, STEP_DELAY_US);      // Haken unten
  penUp();
  moveXY_DDA(SPACE_W, LETTER_H * 0.4, STEP_DELAY_US);
}

void drawh()
{
  float r = LETTER_H * 0.2;
  penDown();
  moveXY_DDA(0, LETTER_H, STEP_DELAY_US);
  penUp();
  moveXY_DDA(0, -LETTER_H / 1.5, STEP_DELAY_US);
  penDown();
  drawArc(r, PI, 0, 20, STEP_DELAY_US); // kurzer Schulter-Bogen
  moveXY_DDA(0, -(LETTER_H - LETTER_H / 1.5), STEP_DELAY_US);
  penUp();
  moveXY_DDA(SPACE_W, 0, STEP_DELAY_US);
}

void drawi()
{
  penUp();
  moveXY_DDA(LETTER_W / 2, 0, STEP_DELAY_US);
  penDown();
  moveXY_DDA(0, LETTER_H * 0.5, STEP_DELAY_US); // Stamm
  penUp();
  moveXY_DDA(0, LETTER_H / 4, STEP_DELAY_US);
  penDown();
  moveXY_DDA(0, LETTER_H * 0.05, STEP_DELAY_US); // Punkt
  penUp();
  moveXY_DDA(LETTER_W / 2 + SPACE_W, -(LETTER_H * 0.5 + LETTER_H / 4), STEP_DELAY_US);
}

void drawj()
{
  penUp();
  moveXY_DDA(LETTER_W * 0.75, LETTER_H * 0.75, STEP_DELAY_US);
  penDown();
  moveXY_DDA(0, -(LETTER_H * 0.75 + LETTER_H * 0.4), STEP_DELAY_US); // Stamm + Unterlänge
  float r = LETTER_H * 0.25;
  drawArc(r, -PI / 2, PI / 2, 30, STEP_DELAY_US); // Haken links
  penUp();
  moveXY_DDA(-LETTER_W * 0.25, LETTER_H * 0.95, STEP_DELAY_US);
  penDown();
  moveXY_DDA(0, LETTER_H * 0.05, STEP_DELAY_US); // Punkt
  penUp();
  moveXY_DDA(LETTER_W * 0.5 + SPACE_W, -LETTER_H * 1.05, STEP_DELAY_US);
}

void drawk()
{
  penDown();
  moveXY_DDA(0, LETTER_H, STEP_DELAY_US);
  penUp();
  moveXY_DDA(LETTER_W * 0.7, 0, STEP_DELAY_US);
  penDown();
  moveXY_DDA(-LETTER_W * 0.7, -LETTER_H * 0.5, STEP_DELAY_US);
  moveXY_DDA(LETTER_W * 0.7, -LETTER_H * 0.5, STEP_DELAY_US);
  penUp();
  moveXY_DDA(SPACE_W, 0, STEP_DELAY_US);
}

void drawl()
{
  penUp();
  moveXY_DDA(0, LETTER_H, STEP_DELAY_US);
  penDown();
  moveXY_DDA(0, -LETTER_H, STEP_DELAY_US);
  moveXY_DDA(LETTER_W * 0.08, 0, STEP_DELAY_US);
  penUp();
  moveXY_DDA(SPACE_W, 0, STEP_DELAY_US);
}

void drawm()
{
  float r = LETTER_H * 0.2;
  penDown();
  moveXY_DDA(0, LETTER_H * 0.7, STEP_DELAY_US);
  penUp();
  moveXY_DDA(0, -LETTER_H * 0.25, STEP_DELAY_US);
  penDown();
  drawArc(r, PI, PI * 0.1, 20, STEP_DELAY_US); // kurzer Schulter-Bogen
  moveXY_DDA(0, -(LETTER_H * 0.7 - LETTER_H * 0.25), STEP_DELAY_US);
  penUp();
  moveXY_DDA(0, LETTER_H * 0.6 - LETTER_H * 0.25, STEP_DELAY_US);
  penDown();
  drawArc(r, PI, PI * 0.1, 20, STEP_DELAY_US); // kurzer Schulter-Bogen
  moveXY_DDA(0, -(LETTER_H * 0.7 - LETTER_H * 0.25), STEP_DELAY_US);
  penUp();
  moveXY_DDA(SPACE_W, 0, STEP_DELAY_US);
}

void drawn()
{
  float r = LETTER_H * 0.2;
  penDown();
  moveXY_DDA(0, LETTER_H * 0.7, STEP_DELAY_US);
  penUp();
  moveXY_DDA(0, -LETTER_H * 0.25, STEP_DELAY_US);
  penDown();
  drawArc(r, PI, PI * 0.1, 20, STEP_DELAY_US); // kurzer Schulter-Bogen
  moveXY_DDA(0, -(LETTER_H * 0.7 - LETTER_H * 0.25), STEP_DELAY_US);
  penUp();
  moveXY_DDA(SPACE_W, 0, STEP_DELAY_US);
}

void drawo()
{
  float r = LETTER_H * 0.3;
  penUp();
  moveXY_DDA(2*r, r, STEP_DELAY_US);
  penDown();
  drawArc(r, 0, 2 * PI, 20, STEP_DELAY_US);
  penUp();
  moveXY_DDA(r + SPACE_W, -r, STEP_DELAY_US);
}

void drawp()
{
  float r = LETTER_H / 4;
  penDown();
  moveXY_DDA(LETTER_W / 6, 0, 1000);
  drawArc(r, -PI / 2, PI / 2, 30, 1000); // untere Rundung
  moveXY_DDA(-LETTER_W / 6, 0, 1000);
  moveXY_DDA(0, -LETTER_H, 1000);
  moveXY_DDA(0, LETTER_H - 2*r, 1000);
  penUp();
  moveXY_DDA(2*r + SPACE_W, 0, 1000);
}

void drawq()
{
  float r = LETTER_H / 4;
  penDown();
  moveXY_DDA(-LETTER_W / 6, 0, 1000);
  drawArc(r, (PI * 3) / 2, PI / 2, 30, 1000); // untere Rundung
  moveXY_DDA(LETTER_W / 6, 0, 1000);
  moveXY_DDA(0, -LETTER_H, 1000);
  moveXY_DDA(0, LETTER_H - 2*r, 1000);
  penUp();
  moveXY_DDA(2*r + SPACE_W, 0, 1000);
}

void drawr()
{
  float r = LETTER_H * 0.2;
  penDown();
  moveXY_DDA(0, LETTER_H * 0.7, STEP_DELAY_US);
  penUp();
  moveXY_DDA(0, -LETTER_H * 0.25, STEP_DELAY_US);
  penDown();
  drawArc(r, PI, PI * 0.1, 20, STEP_DELAY_US); // kurzer Schulter-Bogen
  penUp();
  moveXY_DDA(LETTER_W * 0.3 + SPACE_W, -LETTER_H * 0.44, STEP_DELAY_US);
}

void draws()
{
  float r = LETTER_H / 4 / 1.5;
  penUp();
  moveXY_DDA((r + LETTER_W / 5) / 1.5, LETTER_H / 1.5, STEP_DELAY_US);
  penDown();
  moveXY_DDA(-LETTER_W / 5 / 1.5, 0, STEP_DELAY_US);
  drawArc(r, PI / 2, 3 * PI / 2, 30, STEP_DELAY_US);
  drawArc(r, PI / 2, -PI / 2, 30, STEP_DELAY_US);
  moveXY_DDA(-LETTER_W / 5 / 1.5, 0, STEP_DELAY_US);
  penUp();
  moveXY_DDA(LETTER_W / 5 / 1.5 + r + SPACE_W, 0, STEP_DELAY_US);
}

void drawt()
{
  float r = LETTER_H * 0.1;
  penUp();
  moveXY_DDA(LETTER_W * 0.4, LETTER_H, STEP_DELAY_US);
  penDown();
  moveXY_DDA(0, -(LETTER_H - r), STEP_DELAY_US);
  drawArc(r, PI, 0, 15, STEP_DELAY_US); // Fußhaken
  penUp();
  moveXY_DDA(-LETTER_W * 0.4 - r, LETTER_H * 0.65, STEP_DELAY_US);
  penDown();
  moveXY_DDA(LETTER_W * 0.8, 0, STEP_DELAY_US); // Querstrich
  penUp();
  moveXY_DDA(LETTER_W * 0.2 + r + SPACE_W, -LETTER_H * 0.65, STEP_DELAY_US);
}

void drawu()
{
  float r = LETTER_H * 0.25;
  penUp();
  moveXY_DDA(0, LETTER_H * 0.7, STEP_DELAY_US);
  penDown();
  moveXY_DDA(0, -(LETTER_H * 0.7 - r), STEP_DELAY_US);
  drawArc(r, PI, 2 * PI, 30, STEP_DELAY_US);
  moveXY_DDA(0, LETTER_H * 0.7 - r, STEP_DELAY_US);
  penUp();
  moveXY_DDA(0, -(LETTER_H * 0.7 - r), STEP_DELAY_US);
  penDown();
  moveXY_DDA(0, -r, STEP_DELAY_US);
  penUp();
  moveXY_DDA(SPACE_W, 0, STEP_DELAY_US);
}

void drawv()
{
  penUp();
  moveXY_DDA(0, LETTER_H * 0.65, STEP_DELAY_US);
  penDown();
  moveXY_DDA(LETTER_W * 0.4, -LETTER_H * 0.61, STEP_DELAY_US);
  moveXY_DDA(LETTER_W * 0.4, LETTER_H * 0.61, STEP_DELAY_US);
  penUp();
  moveXY_DDA(SPACE_W, -LETTER_H * 0.65, STEP_DELAY_US);
}

void draww()
{
  penUp();
  moveXY_DDA(0, LETTER_H * 0.55, STEP_DELAY_US);
  penDown();
  moveXY_DDA(LETTER_W / 4, -LETTER_H * 0.55, STEP_DELAY_US);
  moveXY_DDA(LETTER_W / 4, LETTER_H * 0.35, STEP_DELAY_US);
  moveXY_DDA(LETTER_W / 4, -LETTER_H * 0.35, STEP_DELAY_US);
  moveXY_DDA(LETTER_W / 4, LETTER_H * 0.55, STEP_DELAY_US);
  penUp();
  moveXY_DDA(SPACE_W, -LETTER_H * 0.5, STEP_DELAY_US);
}

void drawx()
{
  penUp();
  moveXY_DDA(0, LETTER_H * 0.7, STEP_DELAY_US);
  penDown();
  moveXY_DDA(LETTER_W * 0.8, -LETTER_H * 0.7, STEP_DELAY_US);
  penUp();
  moveXY_DDA(-LETTER_W * 0.8, 0, STEP_DELAY_US);
  penDown();
  moveXY_DDA(LETTER_W * 0.8, LETTER_H * 0.7, STEP_DELAY_US);
  penUp();
  moveXY_DDA(SPACE_W, -LETTER_H * 0.7, STEP_DELAY_US);
}

void drawy()
{
  float r = LETTER_H * 0.25;
  penUp();
  moveXY_DDA(0, LETTER_H * 0.7, STEP_DELAY_US);
  penDown();
  moveXY_DDA(LETTER_W / 2, -LETTER_H * 0.35, STEP_DELAY_US);
  moveXY_DDA(LETTER_W / 2, LETTER_H * 0.35, STEP_DELAY_US);
  penUp();
  moveXY_DDA(-LETTER_W / 2, -LETTER_H * 0.35, STEP_DELAY_US);
  penDown();
  moveXY_DDA(0, -(LETTER_H * 0.35 + LETTER_H * 0.4), STEP_DELAY_US); // Unterlänge
  drawArc(r, -PI / 2, PI / 2, 20, STEP_DELAY_US);
  penUp();
  moveXY_DDA(LETTER_W / 2 + SPACE_W, LETTER_H * 0.4, STEP_DELAY_US);
}

void drawz()
{
  penUp();
  moveXY_DDA(0, LETTER_H * 0.65, STEP_DELAY_US);
  penDown();
  moveXY_DDA(LETTER_W * 0.7, 0, STEP_DELAY_US);
  moveXY_DDA(-LETTER_W * 0.7, -LETTER_H * 0.55, STEP_DELAY_US);
  moveXY_DDA(LETTER_W * 0.7, 0, STEP_DELAY_US);
  penUp();
  moveXY_DDA(SPACE_W, 0, STEP_DELAY_US);
}

void drawLetter(char c)
{
  if (currentX + LETTER_W > MAX_WIDTH)
    newLine();

  switch (c)
  {
  case 'A':
    drawA();
    break;
  case 'B':
    drawB();
    break;
  case 'C':
    drawC();
    break;
  case 'D':
    drawD();
    break;
  case 'E':
    drawE();
    break;
  case 'F':
    drawF();
    break;
  case 'G':
    drawG();
    break;
  case 'H':
    drawH();
    break;
  case 'I':
    drawI();
    break;
  case 'J':
    drawJ();
    break;
  case 'K':
    drawK();
    break;
  case 'L':
    drawL();
    break;
  case 'M':
    drawM();
    break;
  case 'N':
    drawN();
    break;
  case 'O':
    drawO();
    break;
  case 'P':
    drawP();
    break;
  case 'Q':
    drawQ();
    break;
  case 'R':
    drawR();
    break;
  case 'S':
    drawS();
    break;
  case 'T':
    drawT();
    break;
  case 'U':
    drawU();
    break;
  case 'V':
    drawV();
    break;
  case 'W':
    drawW();
    break;
  case 'X':
    drawX();
    break;
  case 'Y':
    drawY();
    break;
  case 'Z':
    drawZ();
    break;

    // Kleinbuchstaben
  case 'a':
    drawa();
    break;
  case 'b':
    drawb();
    break;
  case 'c':
    drawc();
    break;
  case 'd':
    drawd();
    break;
  case 'e':
    drawe();
    break;
  case 'f':
    drawf();
    break;
  case 'g':
    drawg();
    break;
  case 'h':
    drawh();
    break;
  case 'i':
    drawi();
    break;
  case 'j':
    drawj();
    break;
  case 'k':
    drawk();
    break;
  case 'l':
    drawl();
    break;
  case 'm':
    drawm();
    break;
  case 'n':
    drawn();
    break;
  case 'o':
    drawo();
    break;
  case 'p':
    drawp();
    break;
  case 'q':
    drawq();
    break;
  case 'r':
    drawr();
    break;
  case 's':
    draws();
    break;
  case 't':
    drawt();
    break;
  case 'u':
    drawu();
    break;
  case 'v':
    drawv();
    break;
  case 'w':
    draww();
    break;
  case 'x':
    drawx();
    break;
  case 'y':
    drawy();
    break;
  case 'z':
    drawz();
    break;

  case ' ':
    penUp();
    moveXY_DDA(LETTER_W, 0, 1000);
    currentX += LETTER_W;
    return;
  }
}

void loop()
{
  u8g2.clearBuffer();                   // Buffer leeren
  u8g2.setFont(u8g2_font_ncenB08_tr);   // Schriftart setzen
  u8g2.drawStr(0, 15, "Ready to read"); // Text zeichnen
  u8g2.drawStr(0, 35, "You can send!");
  u8g2.sendBuffer(); // Buffer an Display senden

  if (Serial.available())
  {
    digitalWrite(BUILTIN_LED, HIGH); // LED an bei Empfang
    motorsEnable();

    u8g2.clearBuffer();                 // Buffer leeren
    u8g2.setFont(u8g2_font_ncenB08_tr); // Schriftart setzen
    u8g2.drawStr(0, 15, "Senden");      // Text zeichnen
    u8g2.sendBuffer();
    while (Serial.available())
    {
      char c = Serial.read(); // Zeichenweise lesen

      char buffer[20];

      // digitalWrite(LED_ON, HIGH);
      // digitalWrite(LED_OFF, LOW);

      sprintf(buffer, "Current: %c", c);

      u8g2.clearBuffer();                 // Buffer leeren
      u8g2.setFont(u8g2_font_ncenB08_tr); // Schriftart setzen
      u8g2.drawStr(0, 15, buffer);        // Text zeichnen
      u8g2.sendBuffer();                  // Buffer an Display senden

      if (settings && (c != '{') && (c != '}'))
      {
        wholeSetting = wholeSetting + c;
      }

      if (c == '{')
      {
        settings = true;
      }
      else if (c == '}')
      {
        settings = false;
        LETTER_H = wholeSetting.toFloat();
        LETTER_W = LETTER_H * 0.8;
        wholeSetting = "";
      }
      else if ((c == '\n' || c == '\r') && !settings)
      {
        continue;
      }
      else if ((c >= 'A' && c <= 'Z') && (!settings) || (c >= 'a' && c <= 'z'))
        drawLetter(c); // Buchstaben zeichnen
    }

    digitalWrite(BUILTIN_LED, LOW); // LED aus
    motorsDisable();
  }
  // digitalWrite(LED_ON, LOW);
  // digitalWrite(LED_OFF, HIGH);
  delay(1000);
}
