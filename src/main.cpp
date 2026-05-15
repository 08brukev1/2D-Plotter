#include <Arduino.h>
#include <Bounce2.h>
#include <ESP32Servo.h>
#include <U8g2lib.h>
#include <Wire.h>
// ----------------------- States ------------------------------
#define STATE_IDLE 0                 // Idle state - waiting to read
#define STATE_DRAWING 1              // State while drawing
#define STATE_HOMING 2               // State reset the plotter - x and y-achsis go to zero
#define STATE_NEW_LINE 3             // State after finishing a line
#define STATE_AFTER_HOMING_NEWLINE 4 // State after every homing and new line to reset all parameters and going one line down
#define RESET_POSITION 5             // State after every character to reset their position, so that all characters align properly
#define STATE_READING 7              // After seeing that characters were sent to the plotter
#define STATE_CLEARBUFFER 8          // To clear the buffer, after stopping the writing process - All characters will be read out the buffer
// ----------------------- Motor1 = X-Achsis ------------------------------
#define DIR1_PIN 18    // Pin for the direction
#define STEP1_PIN 19   // Pin for each step
#define ENABLE1_PIN 23 // Motor state: LOW = OFF | HIGH = ON
// ----------------------- Motor2 = Y-Achsis ------------------------------
#define DIR2_PIN 15
#define STEP2_PIN 2
#define ENABLE2_PIN 17
// ----------------------- Servo ------------------------------
#define SERVO_PIN 13
#define debouncePenUp 100
#define debouncePenDown 200
// ----------------------- Display pins ------------------------------
#define SCL 22
#define SDA 21
// ----------------------- LED pins ------------------------------
#define LED_READY 26
#define LED_BUSY 27
// ----------------------- Button pin ------------------------------
#define Button 36
// ----------------------- Sensor pins ------------------------------
#define Sensor1 14 // Sensor for the x-achsis
#define Sensor2 33 // Sensor for the y-achsis
// ----------------------- Constant motor values --------------------
#define STEPS_PER_MM 4.76                  // How many steps per millimeter
#define STEPS_PER_CM (STEPS_PER_MM * 10.0) // mm steps converted to cm

// Delay for each step, this parameter controlls the speed of the motor
#define STEP_DELAY_US 700      // Normal speed while writing
#define STEP_DELAY_US_FREE 300 // Speed while homing
// ----------------------- Values for the servo --------------------
#define SERVO_UP 30   // Position servo up
#define SERVO_DOWN 90 // Position servo down

#include "symbolsNumbers.h"
#include "lowercaseLetters.h"
#include "uppercaseLetters.h"

// ----------------------- Motor settings in cm --------------------
double LETTER_W = 0.8;  // Width for each character
double LETTER_H = 1.0;  // Heigth for each character
double SPACE_W = 0.2;   // Space between each character
double LINE_H = 1.2;    // Heigth of every line
double MAX_WIDTH = 14;  // Maximum width to draw
double MAX_HEIGTH = 14; // Maximum heigth to draw

// Speed of the motor
long variableStepDelayUs = STEP_DELAY_US; // This value can be set to STEP_DELAY_US for drawing or STEP_DELAY_US_FREE for homing or new line
// ----------------------- Setting variables ------------------------------
bool settings = false;    // This bool shows, if the plotter reads the heigth or other characters to write
String wholeSetting = ""; // This is the string which contains the heigth
// ----------------------- State variables------------------------------
int state = STATE_IDLE;        // Main state of the plotter
int letter_state = 0;          // State for each character - every character is a mini state machine which is controlled by this state
int remember_letter_state = 0; // State to save the letter_state, when the plotter draws a rounding. More explained in the drawArc function
int draw_state = 0;            // State which is used in the moveXY_DDA function for a mini state machine. More explained in the moveXY_DDA function 
int arc_state = 0;             // State in the drawArc function for a mini state machine
int drawSegments = 0;          // State to draw each segement in the drawArc function
// ----------------------- Variables to reset characters ------------------------------
double resetPositionX = 0.0;    // Counts the exact position after each step for the x-achsis
double resetPositionY = 0.0;    // Counts the exact position after each step for the y-achsis

double calculatedResetX = 0.0;  // X reset-length for each character: MaxWidth - resetPositionX
bool countPosition = false;   // Bool if it should count the current X position or not

// ----------------------- Variables for drawArc | roundings ------------------------------
double prevX = 0.0;             // Saves the value of the start for each rounding - X-Achsis
double prevY = 0.0;             // Here for Y-Achsis
double angleStep = 0.0;         // Calculation of each segment and their angle

double theta = 0.0;
double x = 0.0;
double y = 0.0;
double dx = 0.0;
double dy = 0.0;

// ----------------------- Variables for normal moves in X and Y ------------------------------
float stepsX = 0.0;           // Calculates the steps for each move
float stepsY = 0.0;           // Same for Y-Achsis

long maxSteps = 0;
long errorX = 0;
long errorY = 0;
long stepCounter = 0;

long long XYTime = micros();
double maxWidth = 0.0;

bool drawConnection = false;    // Bool if the plotter should draw a minus after a new line because the word continues

// ----------------------- Servo variables ------------------------------
long long servoTime = 0;
long long ButtonFirstTime = millis();
long long ButtonFirstTime = millis();

long arcCounter = 0;       // Counts written segments
int segments = 0;          // Amount of segments to draw for a rounding

int XYState = 0;
int ServoState = 0;
int stateNewLine = 0;   // State for a mini state machine which switches trough the newLine state

char c;                 // Char to draw
char nextChar;          // Next char to draw - only used to show it on the display
int letterToDraw;       // Variable to select the character in the struct and the matching function

bool S_Button = false;          // State of the button
bool actualLetterDone = true;   // Shows if the current letter is done or not
bool displayUpdate = false;     // To update the display when a new character arrives
bool sensor1Triggered = false;  
bool sensor2Triggered = false;
bool arcReset = false;          // If a new rounding begins, this bool goes on true to reset variables for the drawArc function
bool clearBuffer = true;        // After stopping the writing process, the buffer will be read out. This variable shows if the buffer has to be read out or not
bool goToNewLine = false;       // Indicates if the plotter should draw a "-", "," or "." to finish the line properly

bool drawDotEnd = false;        // Shows if it should draw a dot
bool drawCommaEnd = false;      // Shows if it should draw a comma

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
    ButtonFirstTime = millis();
  }

  if (MeinTaster.fell() && state == STATE_IDLE)
  {
    ButtonFirstTime = millis();
    if (ButtonFirstTime - ButtonFirstTime > 2000)
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
    state = STATE_AFTER_HOMING_NEWLINE;
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
      actualLetterDone = true;
      displayUpdate = false;
      letter_state = 0;
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
  if (Serial.available() && state != RESET_POSITION && actualLetterDone && state != STATE_NEW_LINE && state != STATE_AFTER_HOMING_NEWLINE && state != STATE_HOMING || settings)
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
    if (LETTER_H != 1.0)
    {
      LETTER_W = 0.8;
      LINE_H = 1.6;
      LETTER_H = 1.0;
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
        else
          drawCommaEnd = false;
        if (Serial.peek() == '.')
        {
          drawDotEnd = true;
          Serial.read();
        }
        else
          drawCommaEnd = false;
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
        }
        else
          drawMinus();
      }
      else
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
      currentY = currentY + LINE_H;
      variableStepDelayUs = STEP_DELAY_US;
      state = STATE_AFTER_HOMING_NEWLINE;
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
  case STATE_AFTER_HOMING_NEWLINE:
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
      Serial.println(maxWidth);
      resetPositionX = 0.0;
      resetPositionY = 0.0;
      calculatedResetX = 0.0;
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
    stepsX = fabs(x_cm * STEPS_PER_CM);
    stepsY = fabs(y_cm * STEPS_PER_CM);

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
          drawSegments = 2;
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
    drawSegments = 0;
    segments = segments;

    angleStep = (endAngle - startAngle) / segments;

    prevX = radius_cm * cos(startAngle);
    prevY = radius_cm * sin(startAngle);
    remember_letter_state = letter_state;
    arcReset = true;
    break;

  case 1:
    switch (drawSegments)
    {
    case 0:
      arcCounter++;

      theta = startAngle + arcCounter * angleStep;
      x = radius_cm * cos(theta);
      y = radius_cm * sin(theta);
      dx = x - prevX;
      dy = y - prevY;
      drawSegments = 1;
      variableStepDelayUs = variableStepDelayUs * 2;
      break;

    case 1:
      moveXY_DDA(dx, dy, variableStepDelayUs);
      letter_state = remember_letter_state;
      break;

    case 2: // kommt von moveXY_DDA via drawSegments++
      prevX = x;
      prevY = y;

      variableStepDelayUs = STEP_DELAY_US;

      if (arcCounter >= segments)
      {
        arc_state = 0;
        drawSegments = 0;
        letter_state++; // erst JETZT letter_state weiterzählen
        arcReset = true;
      }
      else
      {
        drawSegments = 0;
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

  pinMode(Button, INPUT);

  pinMode(LED_BUSY, OUTPUT);
  pinMode(LED_READY, OUTPUT);

  pinMode(SERVO_PIN, OUTPUT);
  digitalWrite(SERVO_PIN, LOW);

  penServo.write(SERVO_UP);
  penServo.attach(SERVO_PIN, 500, 2400);
  MeinTaster.attach(Button);
  MeinTaster.interval(40);
  u8g2.begin();

  servoTime = millis();
  // state = STATE_IDLE;

  resetPositionX = 0.0;
  resetPositionY = 0.0;
  calculatedResetX = 0.0;
  maxWidth = 0.0;
  letter_state = 0;
  ServoState = 0;
  actualLetterDone = true;
}

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
  state = STATE_DRAWING;
}

void loop()
{
  MeinTaster.update();

  read();
  transsitions();
  actions();
}
