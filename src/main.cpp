#include <Arduino.h>
#include <Bounce2.h>
#include <ESP32Servo.h>
#include <U8g2lib.h>
#include <Wire.h>
/*
---------------------------------------------------------------------
Kevin Brunner | Lukas Giordani
This is the code for a FÜLA project: "Flotter Plotter". This project is a 2D-Plotter
whitch can write the whole alpabet, numbers and special characters.
---------------------------------------------------------------------
*/

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

// ----------------------- Variables to reset ------------------------------
double resetPositionX = 0.0;    // Counts the exact position after each step for the x-achsis
double resetPositionY = 0.0;    // Counts the exact position after each step for the y-achsis

double calculatedResetX = 0.0;  // X reset-length for each character: MaxWidth - resetPositionX
bool countPosition = false;   // Bool if it should count the current X position or not

float currentX = 0.0;     // Variable to reset the achsis. These two are used for new line and homing
float currentY = 0.0;     

// ----------------------- Variables for drawArc | roundings ------------------------------
double prevX = 0.0;             // Saves the value of the start for each rounding - X-Achsis
double prevY = 0.0;             // Here for Y-Achsis
double angleStep = 0.0;         // Calculation of each segment and their angle

double theta = 0.0;
double x = 0.0;
double y = 0.0;
double dx = 0.0;
double dy = 0.0;

long arcCounter = 0;       // Counts written segments
int segments = 0;          // Amount of segments to draw for a rounding

// ----------------------- Variables for normal moves in X and Y ------------------------------
float stepsX = 0.0;           // Calculates the steps for each move
float stepsY = 0.0;           // Same for Y-Achsis

long maxSteps = 0;            
long errorX = 0;              
long errorY = 0;
long stepCounter = 0;
int XYState = 0;

long long XYTime = micros();
double maxWidth = 0.0;
bool drawConnection = false;    // Bool if the plotter should draw a minus after a new line because the word continues
bool arcReset = false;          // If a new rounding begins, this bool goes on true to reset variables for the drawArc function

// ----------------------- Servo variables ------------------------------
long long servoTime = 0;
long long ButtonFirstTime = millis();
long long ButtonSecondTime = millis();
int servo_state = 0;   // State for a state machine in the servo functions

// ----------------------- New line / homing ------------------------------
int stateNewLine = 0;   // State for a mini state machine which switches trough the newLine state

bool goToNewLine = false;       // Indicates if the plotter should draw a "-", "," or "." to finish the line properly
bool drawDotEnd = false;        // Shows if it should draw a dot
bool drawCommaEnd = false;      // Shows if it should draw a comma

// ----------------------- Display variables ------------------------------
bool displayUpdate = false;     // To update the display when a new character arrives
char nextChar;          // Next char to draw - only used to show it on the display

// ----------------------- States of the inputs ------------------------------
bool S_Button = false;          // State of the button
bool sensor1Triggered = false;  
bool sensor2Triggered = false;

// ----------------------- General variables ------------------------------
char c;                 // Char to draw
int letterToDraw;       // Variable to select the character in the struct and the matching function
bool clearBuffer = true;        // After stopping the writing process, the buffer will be read out. This variable shows if the buffer has to be read out or not
bool actualLetterDone = true;   // Shows if the current letter is done or not

// These header files contain all characters of the plotter
#include "symbolsNumbers.h"
#include "lowercaseLetters.h"
#include "uppercaseLetters.h"

void read();
void transsitions();
void actions();
void motorsEnable();
void motorsDisable();
void drawLetter(char c);
void penUp();
void penDown();
void moveXY_DDA(float x_cm, float y_cm, int stepDelayUs);

// Uppercase letters
void drawA();  void drawB();  void drawC();  void drawD();  void drawE();
void drawF();  void drawG();  void drawH();  void drawI();  void drawJ();
void drawK();  void drawL();  void drawM();  void drawN();  void drawO();
void drawP();  void drawQ();  void drawR();  void drawS();  void drawT();
void drawU();  void drawV();  void drawW();  void drawX();  void drawY();
void drawZ();

// Lowercase letters
void drawa();  void drawb();  void drawc();  void drawd();  void drawe();
void drawf();  void drawg();  void drawh();  void drawi();  void drawj();
void drawk();  void drawl();  void drawm();  void drawn();  void drawo();
void drawp();  void drawq();  void drawr();  void draws();  void drawt();
void drawu();  void drawv();  void draww();  void drawx();  void drawy();
void drawz();

// Special characters
void drawDot();               void drawComma();            void drawExclamation();
void drawQuestion();          void drawColon();            void drawEqual();
void drawPlus();              void drawMinus();            void drawPercent();
void drawParenthesisOpen();   void drawParenthesisClose();
void drawQuote();             void drawSpace();

// Numbers
void draw0();  void draw1();  void draw2();  void draw3();  void draw4();
void draw5();  void draw6();  void draw7();  void draw8();  void draw9();

struct Entry          // Struct to see which function has to be called. It contains all characters and function for each characters
{
  char key;
  void (*func)();
};

Entry letters[] = {
  // Lowercase letters
  {'a', drawa}, {'b', drawb}, {'c', drawc}, {'d', drawd}, {'e', drawe},
  {'f', drawf}, {'g', drawg}, {'h', drawh}, {'i', drawi}, {'j', drawj},
  {'k', drawk}, {'l', drawl}, {'m', drawm}, {'n', drawn}, {'o', drawo},
  {'p', drawp}, {'q', drawq}, {'r', drawr}, {'s', draws}, {'t', drawt},
  {'u', drawu}, {'v', drawv}, {'w', draww}, {'x', drawx}, {'y', drawy},
  {'z', drawz},

  // Uppercase letters
  {'A', drawA}, {'B', drawB}, {'C', drawC}, {'D', drawD}, {'E', drawE},
  {'F', drawF}, {'G', drawG}, {'H', drawH}, {'I', drawI}, {'J', drawJ},
  {'K', drawK}, {'L', drawL}, {'M', drawM}, {'N', drawN}, {'O', drawO},
  {'P', drawP}, {'Q', drawQ}, {'R', drawR}, {'S', drawS}, {'T', drawT},
  {'U', drawU}, {'V', drawV}, {'W', drawW}, {'X', drawX}, {'Y', drawY},
  {'Z', drawZ},

  // Numbers
  {'0', draw0}, {'1', draw1}, {'2', draw2}, {'3', draw3}, {'4', draw4},
  {'5', draw5}, {'6', draw6}, {'7', draw7}, {'8', draw8}, {'9', draw9},

  // Special characters
  {'.', drawDot},                 {',', drawComma},
  {'!', drawExclamation},         {'?', drawQuestion},
  {':', drawColon},               {'=', drawEqual},
  {'+', drawPlus},                {'-', drawMinus},
  {'%', drawPercent},             {'(', drawParenthesisOpen},
  {')', drawParenthesisClose},    {'"', drawQuote},
  {' ', drawSpace}
  };
// ----------------------- Initializing objects ------------------------------
Servo penServo;
Bounce button;
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);


void read()   // Read function - This reads all inputs
{
  if (button.rose() && ((state == STATE_DRAWING) || (state == STATE_READING) || (state == RESET_POSITION))) // Reads button while the plotter is busy
  {
    S_Button = true;
  }

  if (button.rose() && state == STATE_IDLE)   // Sets the first time after pressing the button
  {
    ButtonFirstTime = millis();
  }

  if (button.fell() && state == STATE_IDLE) // Second button time
  {
    ButtonSecondTime = millis();
    if (ButtonSecondTime - ButtonFirstTime > 2000)  // This controlls if the pressed time is over 2s. If yes, then homing 
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

  // After each homing, the plotter goes over to STATE_AFTER_HOMING_NEWLINE
  if ((state == STATE_HOMING && digitalRead(Sensor1) && digitalRead(Sensor2)) ||
      (state == STATE_NEW_LINE && digitalRead(Sensor1))){
    state = STATE_AFTER_HOMING_NEWLINE; 
    letter_state = 0;
    draw_state = 0;
  }
}

void transsitions()
{
  if (state == STATE_READING)
  {
    // The following part controlls the input and sets individual states
    
    if (settings && (c != '{') && (c != '}'))   // If the settings variable is set to true, the plotter reads every input as a size setting
      wholeSetting = wholeSetting + c;
    if (c == '{') // Checks if the following inputs will be handled as size settings
      settings = true;
    else if (c == '}')  // Checks if the inputs are not settings anymore
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
    else if (c == '\r'){} // Ignores new line as input
    else if ((c == '\n') && !settings)
    {
      state = STATE_IDLE;
      actualLetterDone = true;
      displayUpdate = false;
      letter_state = 0;
    }
    else if (!settings) 
      drawLetter(c);  // Reads normally
    else
    {     // If theres an error, for example nothing above is true, the plotter goes to idle
      state = STATE_IDLE;
      actualLetterDone = true;
    }
  }
  if ((state == STATE_IDLE || state == STATE_DRAWING || state == STATE_READING) && S_Button && actualLetterDone)  // Checks if the button got pressed while writing
  {
    // Plotter clears the buffer, so if you continue with writing, the plotter doesn't continue with writing
    state = STATE_CLEARBUFFER;
    clearBuffer = true;
    S_Button = false;
    actualLetterDone = false;
  }
  // Reads input general input
  if (Serial.available() && state != RESET_POSITION && actualLetterDone && state != STATE_NEW_LINE && state != STATE_AFTER_HOMING_NEWLINE && state != STATE_HOMING || settings)
  {
    // Checks if the plotter is done with the current line(x-achsis)
    if (currentX + (LETTER_W * 2.5) > MAX_WIDTH)
    {
      if (currentY + LINE_H > MAX_HEIGTH) // If both achsis are done, it goes to homing
      {
        state = STATE_HOMING;
      }
      else
        state = STATE_NEW_LINE;
      stateNewLine = 0;
    }
    else
    { // It reads normally if the plotter has enough space to draw
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
    // Set LED state
    digitalWrite(LED_BUSY, LOW);
    digitalWrite(LED_READY, HIGH);
    // Ready screen
    u8g2.clearBuffer();  // Clears display buffer
    u8g2.setFont(u8g2_font_ncenB14_tr); // Sets the font size
    u8g2.drawStr(0, 20, "Ready to");    // Sends the text to the buffer
    u8g2.drawStr(0, 40, "read");
    u8g2.setFont(u8g2_font_ncenB10_tr);
    u8g2.drawStr(0, 60, "You can send!");
    u8g2.sendBuffer();    // Sends the buffer to the display
    motorsDisable();
    if (LETTER_H != 1.0)  // Sets default character size after each writing
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

    // Updates after every input
    if (displayUpdate)
    {
      char bigCurrent[2] = {(c == ' ' ? '_' : c), '\0'}; // Space = '_'

      // Nächsten Buchstaben auslesen
      nextChar = '-';
      if (Serial.available())
        nextChar = Serial.peek(); // Reads the serial buffer, but the character won't get lost

      char bigNext[2] = {(nextChar == ' ' ? '_' : nextChar), '\0'}; // Space = '_', just for the next char

      // Writing display
      u8g2.clearBuffer();

      
      u8g2.setFont(u8g2_font_ncenB08_tr);
      u8g2.drawStr(0, 10, "Current:");
      u8g2.setFont(u8g2_font_ncenB24_tr);
      u8g2.drawStr(0, 45, bigCurrent);

      u8g2.drawLine(63, 0, 63, 64);

      u8g2.setFont(u8g2_font_ncenB08_tr);
      u8g2.drawStr(68, 10, "Next:");
      u8g2.setFont(u8g2_font_ncenB24_tr);
      u8g2.drawStr(68, 45, bigNext);

      u8g2.sendBuffer();
      displayUpdate = false;
    }

    motorsEnable();

    letters[letterToDraw - '!'].func(); // Executes the function for each character
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
      // Checks if there has to be a ',', '.' or a '-' after a line
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
      if (servo_state == 0)
      {
        stateNewLine++;
      }
      break;
    case 3: // Resets the x-achsis to zero
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
        moveXY_DDA(-1, 0, variableStepDelayUs); // Reset x-achsi
      }
      if (!sensor2Triggered)
      {
        moveXY_DDA(0, 1, variableStepDelayUs); // Reset y-achsis
      }
      if (sensor1Triggered && sensor2Triggered)
      {
        letter_state++;   // Done if both are triggered
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
  case STATE_AFTER_HOMING_NEWLINE:    // This state just goes one line down. So it will be used after each homing and new line
    variableStepDelayUs = STEP_DELAY_US_FREE;
    switch (letter_state)
    {
    case 0:
      moveXY_DDA(LETTER_W / 2, -LINE_H * 1.5, variableStepDelayUs); // Also a bit forward to have enough space for bigger characters
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
    /*
    This state resets the position after each character. 
    It uses the calculatedReset variables to reset. These 
    variables count the exact steps after each move.
    */    
    switch (letter_state)
    {
    case 0:
      penUp();
      motorsEnable();

      break;
    case 1:
      countPosition = false;
      calculatedResetX = maxWidth - resetPositionX;   // This calculates the position to reset the character in x
      letter_state++;
      break;
    case 2:
      moveXY_DDA(calculatedResetX + SPACE_W, -resetPositionY, STEP_DELAY_US);
      break;
    case 3:
      // Reset variables
      resetPositionX = 0.0;
      resetPositionY = 0.0;
      calculatedResetX = 0.0;
      maxWidth = 0.0;
      letter_state = 0;
      servo_state = 0;
      actualLetterDone = true;
      if (goToNewLine)  // Checks if it has to go one more time in to STATE_NEW_LINE because of a '-' ect.
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
      // This reads the whole buffer
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

void penUp()
{
  switch (servo_state)
  {
  case 0:      // Like the "init" for each penUp and sets the pen to its position
    servoTime = millis();
    penServo.write(SERVO_UP);
    servo_state = 1;
    break;
  case 1:     // This state waits to give the servo time to move
    if (millis() - servoTime > debouncePenUp)
    {
      servo_state = 0;
      letter_state++;
    }
    break;
  }
}

void penDown()
{
  switch (servo_state)
  {
  case 0:   
    servoTime = millis();
    penServo.write(SERVO_DOWN);
    servo_state = 1;
    break;
  case 1: 
    if (millis() - servoTime > debouncePenDown)
    {
      servo_state = 0;
      letter_state++;
    }
    break;
  }
}

void moveXY_DDA(float x_cm, float y_cm, int stepDelayUs)
{
  switch (draw_state)
  {
  case 0:   // Init
  {
    draw_state = 1;
    stepsX = fabs(x_cm * STEPS_PER_CM); // Just positive values
    stepsY = fabs(y_cm * STEPS_PER_CM);

    digitalWrite(DIR1_PIN, x_cm >= 0 ? LOW : HIGH);   // Sets the direction
    digitalWrite(DIR2_PIN, y_cm >= 0 ? HIGH : LOW);

    maxSteps = max(stepsX, stepsY);
    if (maxSteps == 0.0)
    {
      return;
    }
    if (arcReset)   // If it is a part of a rounding, the plotter resets the following parameters
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
    if ((micros() - XYTime > stepDelayUs))  // Timer for each step. This controlls the speed
    {
      if (XYState == 0)
      {
        errorX += stepsX;
        errorY += stepsY;
        if (errorX >= maxSteps)
        {
          digitalWrite(STEP1_PIN, HIGH);    // First to high at the beginning of a step
          errorX -= maxSteps;
          if (x_cm < 0)   
          {
            currentX -= 1.0 / STEPS_PER_CM;   // Counts the steps
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
        // If all steps are done, letter_state will be incrementet and it goes to the next move
        if (maxSteps <= stepCounter)
        {
          draw_state = 0;
          letter_state++;
          servo_state = 0;
          drawSegments = 2;
        }
        else
          stepCounter++;
        XYState = -1;
      }

      if (XYState == 1)
      {
        digitalWrite(STEP1_PIN, LOW); // And after STEP_DELAY_US it goes to low
        digitalWrite(STEP2_PIN, LOW);
      }
      XYState++;
      XYTime = micros();
    }
    break;
  }
}

void drawArc(float radius_cm, float startAngle, float endAngle, int segments, int stepDelayUs)    // Function to draw roundings. Angles are from 0 to 2PI
{
  switch (arc_state)
  {
  case 0:
    arc_state = 1;
    arcCounter = 0;
    drawSegments = 0;

    angleStep = (endAngle - startAngle) / segments; // Calculates the angleStep

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

      // Calculates the angle of the step
      theta = startAngle + arcCounter * angleStep;
      x = radius_cm * cos(theta);
      y = radius_cm * sin(theta);
      dx = x - prevX;
      dy = y - prevY;

      drawSegments = 1;
      variableStepDelayUs = variableStepDelayUs * 2;  // Double the delay to make it more precise
      break;

    case 1:
      moveXY_DDA(dx, dy, variableStepDelayUs);    // Draws one segment
      letter_state = remember_letter_state;
      break;

    case 2:  
      prevX = x;
      prevY = y;

      variableStepDelayUs = STEP_DELAY_US;

      if (arcCounter >= segments)   // Checks if all segments are done 
      {
        arc_state = 0;
        drawSegments = 0;
        letter_state++; 
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


void setup()
{
  Serial.begin(115200);

  // ----------------------------- Set pinmodes ----------------------------------
  pinMode(DIR1_PIN, OUTPUT);
  pinMode(STEP1_PIN, OUTPUT);
  pinMode(ENABLE1_PIN, OUTPUT);

  pinMode(DIR2_PIN, OUTPUT);
  pinMode(STEP2_PIN, OUTPUT);
  pinMode(ENABLE2_PIN, OUTPUT);

  pinMode(Sensor1, INPUT);
  pinMode(Sensor2, INPUT);

  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(Button, INPUT);

  pinMode(LED_BUSY, OUTPUT);
  pinMode(LED_READY, OUTPUT);

  pinMode(SERVO_PIN, OUTPUT);
  digitalWrite(SERVO_PIN, LOW);

  digitalWrite(ENABLE1_PIN, LOW);
  digitalWrite(ENABLE2_PIN, LOW);

  // Set up servo and button
  penServo.write(SERVO_UP);
  penServo.attach(SERVO_PIN, 500, 2400);

  button.attach(Button);
  button.interval(40);
  u8g2.begin();

  servoTime = millis();

  resetPositionX = 0.0;
  resetPositionY = 0.0;
  calculatedResetX = 0.0;
  maxWidth = 0.0;
  letter_state = 0;
  servo_state = 0;
  actualLetterDone = true;
}

/*
This function searches the suitable function for each character and saves
it in a variable, so that the drawing state can use this function over an over a
again.
*/
void drawLetter(char c) 
{
  // Reset position for a new character
  resetPositionX = 0.0;
  resetPositionY = 0.0;
  countPosition = true;

  // Searches the function for a character in the struct "letters"
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
  button.update();

  read();
  transsitions();
  actions();
}
