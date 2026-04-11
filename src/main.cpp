#include <Arduino.h>
#include <ESP32Servo.h>
#include <U8g2lib.h>
#include <Wire.h>
#include <alphabet.cpp>

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

  penServo.attach(SERVO_PIN, 500, 2400);
  penUp();

  u8g2.begin();

  while (!digitalRead(Sensor1))
  {
    moveXY_DDA(1, 0, 500);
  }
  

  motorsDisable();
}

// Neue Zeile
void newLine()
{
  penUp();
  moveXY_DDA(-currentX, -LINE_H, 1000);
  currentX = 0.0;
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
    u8g2.drawStr(0, 15, "Senden");        // Text zeichnen
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
      else if ((c >= 'a' && c <= 'z') && !settings)
        c -= 32;
      drawLetter(c); // Buchstaben zeichnen
    }

    digitalWrite(BUILTIN_LED, LOW); // LED aus
    motorsDisable();
  }
  // digitalWrite(LED_ON, LOW);
  // digitalWrite(LED_OFF, HIGH);
  delay(1000);
}
