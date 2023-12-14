#include <NewPing.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "button.h"

/*------- PIN SETTINGS -------*/
//  Motor
#define MOTOR_L1 2
#define MOTOR_L2 4 

#define MOTOR_R1 33
#define MOTOR_R2 32

// Line sensor
#define LINE_SENSOR_LEFT  23
#define LINE_SENSOR_RIGHT 14
#define LINE_SENSOR_BACK  17

// Sonar sensor
#define FRONT_HC_TRIG 26
#define FRONT_HC_ECHO 27

#define LEFT_HC_TRIG 18
#define LEFT_HC_ECHO 19

#define RIGHT_HC_TRIG 25
#define RIGHT_HC_ECHO 5

NewPing sonarFront(FRONT_HC_TRIG, FRONT_HC_ECHO, 150);
NewPing sonarLeft(LEFT_HC_TRIG, LEFT_HC_ECHO, 150);
NewPing sonarRight(RIGHT_HC_TRIG, RIGHT_HC_ECHO, 150);

// IR Sensor
#define IR_SENSOR_1 15
#define IR_SENSOR_2 13
//#define IR_SENSOR_3 12

// Button
#define BUTTON 16
button btn(BUTTON);

// comment if you want to use serialport
#define SETTINGS_BUTTON_1 1
#define SETTINGS_BUTTON_2 3

// Display
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 32

#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire);

//------------------------------------------------------------//
int tornTime = 200; // ms
int rotateTime = ((tornTime * 2)  - 60);
int startFightDelay = 400;
int straightDelay = 200;

int sonarDistance = 70; // max 100 sm
int IRDistanceFront = 70; // max 80 sm
int IRDistanceBack = 30; // max 80 sm
const unsigned long measurementInterval = 100;

enum Position
{
  NONE,
  STRAIGHT,
  BACK,
  LEFT,
  RIGHT,
  ROTATE
} roboSumo;

enum Mode
{
  FIGHT_NONE,
  FIGHT_STRAIGHT,
  FIGHT_BACK,
  FIGHT_LEFT,
  FIGHT_RIGHT,
} roboMode;

void setup()
{
  Serial.begin(115200);

  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);

  pinMode(MOTOR_L1, OUTPUT);
  pinMode(MOTOR_L2, OUTPUT);
  pinMode(MOTOR_R1, OUTPUT);
  pinMode(MOTOR_R2, OUTPUT);

  pinMode(LINE_SENSOR_LEFT, INPUT);
  pinMode(LINE_SENSOR_RIGHT, INPUT);
  pinMode(LINE_SENSOR_BACK, INPUT);

  pinMode(IR_SENSOR_1, INPUT);
  pinMode(IR_SENSOR_2, INPUT);

  // OFF TX/RX
  pinMode(SETTINGS_BUTTON_1, INPUT_PULLUP);
  pinMode(SETTINGS_BUTTON_2, INPUT_PULLUP);

  roboMode = buttonSettings();
  displayMode(roboMode);

  displayLogo();
}

void loop()
{
  static bool initialSetupDone = false;

  // Check if the button is clicked
  if (btn.click() && !initialSetupDone)
  {
    displayCount(5); // 5 sec to fight
    startFight(roboMode); // get fight mode type
    initialSetupDone = true;
  }

  if (initialSetupDone)
  {
    Drive(); // main
    //Drive2();
  }
}

/*------- MOTIONS -------*/
// robot straight
void posStraight()
{
  Serial.println("Straight");

  digitalWrite(MOTOR_L1, LOW);
  digitalWrite(MOTOR_L2, HIGH);

  digitalWrite(MOTOR_R1, LOW);
  digitalWrite(MOTOR_R2, HIGH);
}

// robot back
void posBack()
{
  Serial.println("Back");

  digitalWrite(MOTOR_L1, HIGH);
  digitalWrite(MOTOR_L2, LOW);

  digitalWrite(MOTOR_R1, HIGH);
  digitalWrite(MOTOR_R2, LOW);

  delay(400);
}

// robot left
void posLeft()
{
  Serial.println("Left");

  digitalWrite(MOTOR_L1, LOW);
  digitalWrite(MOTOR_L2, HIGH);

  digitalWrite(MOTOR_R1, HIGH);
  digitalWrite(MOTOR_R2, LOW);

  delay(tornTime);
}

// robot right
void posRight()
{
  Serial.println("Right");

  digitalWrite(MOTOR_L1, HIGH);
  digitalWrite(MOTOR_L2, LOW);

  digitalWrite(MOTOR_R1, LOW);
  digitalWrite(MOTOR_R2, HIGH);

  delay(tornTime - 20);
}

// robot rotate
void posRotate()
{
  Serial.println("Rotate");

  digitalWrite(MOTOR_L1, HIGH);
  digitalWrite(MOTOR_L2, LOW);

  digitalWrite(MOTOR_R1, LOW);
  digitalWrite(MOTOR_R2, HIGH);

  delay(rotateTime);
}

// robot stop
void posStop()
{
  Serial.println("Stop");

  digitalWrite(MOTOR_L1, LOW);
  digitalWrite(MOTOR_L2, LOW);

  digitalWrite(MOTOR_R1, LOW);
  digitalWrite(MOTOR_R2, LOW);
}

// Recognizing the black line
Position lineSensor()
{
  if (digitalRead(LINE_SENSOR_LEFT) == HIGH)
     return LEFT;

  if (digitalRead(LINE_SENSOR_RIGHT) == HIGH)
    return RIGHT;

  if (digitalRead(LINE_SENSOR_BACK) == HIGH)
    return BACK;

  if (digitalRead(LINE_SENSOR_LEFT) == HIGH && digitalRead(LINE_SENSOR_RIGHT) == HIGH)
     return ROTATE;

  return NONE;
}

Position sonar()
{
  unsigned long currentTime = millis();
  static unsigned long lastMeasurementTime;

  if (currentTime - lastMeasurementTime >= measurementInterval)
  {
    lastMeasurementTime = currentTime;

    unsigned long frontDistance = sonarFront.ping_cm();
    unsigned long leftDistance = sonarLeft.ping_cm();
    unsigned long rightDistance = sonarRight.ping_cm();


    if (frontDistance != 0 && frontDistance < sonarDistance)
      return STRAIGHT;

    if (leftDistance != 0 && leftDistance < sonarDistance)
      return LEFT;

    if (rightDistance != 0 && rightDistance < sonarDistance)
      return RIGHT;

  }  
  
  return NONE;
}

Position IRSensor() {
  unsigned long currentTime = millis();
  static unsigned long lastMeasurementTime;
  static int lastIr1Distance = -1;
  static int lastIr2Distance = -1;

  if (currentTime - lastMeasurementTime >= measurementInterval) {
    lastMeasurementTime = currentTime;

    int ir1_distance_cm = 29.988 * pow(analogRead(IR_SENSOR_1) * (3.3 / 4096), -1.173);
    int ir2_distance_cm = 29.988 * pow(analogRead(IR_SENSOR_2) * (3.3 / 4096), -1.173);

    if (ir1_distance_cm <= IRDistanceFront) {
      if (ir1_distance_cm == lastIr1Distance) {
        lastIr1Distance = ir1_distance_cm;
        lastIr2Distance = ir2_distance_cm;
        return STRAIGHT;
      }
    }

    if (ir2_distance_cm <= IRDistanceBack) {
      if (ir2_distance_cm == lastIr2Distance) {
        lastIr2Distance = ir2_distance_cm;
        lastIr1Distance = ir1_distance_cm;
        return BACK;
      }
    }

    lastIr1Distance = ir1_distance_cm;
    lastIr2Distance = ir2_distance_cm;
  }
  
  return NONE;
}

// main robot position
void Drive()
{
  Position Line = lineSensor();
  Position IR = IRSensor();
  Position Sonar = sonar();

  // front
  if (Line == NONE || (IR == STRAIGHT || Sonar == STRAIGHT))
  {
    posStraight();
  }

  // back
  if (Line == NONE && IR == BACK)
  {
    posBack();      
    posRotate();
  }

  // sonar
  if (Sonar == LEFT)
  {
    posLeft();
  }

  if (Sonar == RIGHT)
  {
    posRight();
  }

  // line
  switch (Line)
  {
    case LEFT:
      posBack();
      posLeft();
      delay(tornTime);
      break;

    case RIGHT:
      posBack();
      posRotate();
      break;

    case BACK:
      posStraight();
      break;

    case ROTATE:
      posBack();      
      posRotate();
      break;
  }
}

// test
void Drive2()
{
  Position IR = IRSensor();
  Position Sonar = sonar();

  // front
  if (IR == STRAIGHT || Sonar == STRAIGHT)
  {
    posStraight();
    delay(100);
  }
  else if (IR == NONE || Sonar == NONE)
  {
    posStop();
  }

  // back
  if (IR == BACK)
  {     
    posRotate();
  }

  // sonar
  if (Sonar == LEFT)
  {
    posLeft();
  }

  if (Sonar == RIGHT)
  {
    posRight();
  }
}

//------------ Button Settings --------//
Mode buttonSettings()
{

  if (digitalRead(SETTINGS_BUTTON_1) == HIGH && digitalRead(SETTINGS_BUTTON_2) == HIGH)
    return FIGHT_STRAIGHT;

  if (digitalRead(SETTINGS_BUTTON_1) == LOW && digitalRead(SETTINGS_BUTTON_2) == HIGH)
    return FIGHT_LEFT;

  if (digitalRead(SETTINGS_BUTTON_1) == HIGH && digitalRead(SETTINGS_BUTTON_2) == LOW)
    return FIGHT_RIGHT;

  if (digitalRead(SETTINGS_BUTTON_1) == LOW && digitalRead(SETTINGS_BUTTON_2) == LOW)
    return FIGHT_BACK;

  return FIGHT_NONE;
}

void startFight(Mode &robo)
{
  switch(robo)
  {
    case FIGHT_STRAIGHT:
      posStraight();
      break;
    
    case FIGHT_LEFT:
      posLeft();
      //posStraight();
      break;

    case FIGHT_RIGHT:
      posRight();
      //posStraight();
      break;

    case FIGHT_BACK:
      posRotate();
      //posStraight();
      break;
  }
}

//------------- Display -------------//
void displayLogo(void)
{
  display.clearDisplay();

  int centerY = (display.height() - 3 * 8) / 2;

  display.setTextSize(3);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(15, centerY);
  display.print("U-Prox");

  display.display();
}

void displayFight(void)
{
  display.clearDisplay();
  display.invertDisplay(true);

  int centerY = (display.height() - 3 * 8) / 2;

  display.setTextSize(3);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(23, centerY);
  display.print("FIGHT");

  display.display();
}

void displayCount(int count)
{
  for (int i = count; i > 0; i--)
  {
    display.clearDisplay();
    display.invertDisplay(false);

    int centerX = (display.width() / 2);
    int centerY = (display.height() - 3 * 8) / 2;

    display.setTextSize(3);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(centerX, centerY);
    display.print(i);

    display.display();
    delay(1000);
  }

  displayFight();
}

void displayMode(Mode &robo)
{
  display.clearDisplay();
  display.invertDisplay(false);

  int centerY = (display.height() - 3 * 8) / 2;

  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, centerY);
  display.print("MODE ");

  switch(robo)
  {
    case FIGHT_STRAIGHT:
      display.print("FRONT");
      break;

    case FIGHT_LEFT:
      display.print("LEFT");
      break;

    case FIGHT_RIGHT:
      display.print("RIGHT");
      break;

    case FIGHT_BACK:
      display.print("BACK");
      break;

    case FIGHT_NONE:
      display.print("NONE");
      break;
  }

  display.display();
  delay(1000);
}








