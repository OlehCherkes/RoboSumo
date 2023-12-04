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

NewPing sonarFront(FRONT_HC_TRIG, FRONT_HC_ECHO, 100);
NewPing sonarLeft(LEFT_HC_TRIG, LEFT_HC_ECHO, 100);
NewPing sonarRight(RIGHT_HC_TRIG, RIGHT_HC_ECHO, 100);

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
int tornTime = 170;
int rotateTime = ((rotateTime * 2) - 40);
int sonarDistance = 50; //30;
int IRDistance = 80;  // 40;
unsigned long lastMeasurementTime = 0;
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
    displayCount(); // 5 sec to fight
    startFight(roboMode); // get fight mode type
    initialSetupDone = true;
  }

  if (initialSetupDone)
  {
    robotPosition(); //after the initial setup
    //robotLine();
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
}

// robot left
void posLeft()
{
  Serial.println("Left");

  digitalWrite(MOTOR_L1, HIGH);
  digitalWrite(MOTOR_L2, LOW);

  digitalWrite(MOTOR_R1, LOW);
  digitalWrite(MOTOR_R2, HIGH);
}

// robot right
void posRight()
{
  Serial.println("Right");

  digitalWrite(MOTOR_L1, LOW);
  digitalWrite(MOTOR_L2, HIGH);

  digitalWrite(MOTOR_R1, HIGH);
  digitalWrite(MOTOR_R2, LOW);
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
  if (digitalRead(LINE_SENSOR_LEFT) == LOW)
     return LEFT;

  if (digitalRead(LINE_SENSOR_RIGHT) == LOW)
    return RIGHT;

  if (digitalRead(LINE_SENSOR_BACK) == LOW)
    return BACK;

  return NONE;
}

void robotLine()
{
  Position motor = lineSensor();

  switch (motor)
  {
    case BACK:
      posStraight();
      break;

    case LEFT:
      posRight();
      break;

    case RIGHT:
      posLeft();
      break;
    
    case NONE:
      posStop();
      break;
  }
}

Position sonar()
{
  unsigned long currentTime = millis();

  if (currentTime - lastMeasurementTime >= measurementInterval)
  {
    lastMeasurementTime = currentTime;

    if (sonarFront.ping_cm() != 0 && sonarFront.ping_cm() < sonarDistance)
      return STRAIGHT;

    if (sonarLeft.ping_cm() != 0 && sonarLeft.ping_cm() < sonarDistance)
      return LEFT;

    if (sonarRight.ping_cm() != 0 && sonarRight.ping_cm() < sonarDistance)
      return RIGHT;

    return NONE;
  }  
}

Position IRSensor()
{
  unsigned long currentTime = millis();

  if (currentTime - lastMeasurementTime >= measurementInterval)
  {
    lastMeasurementTime = currentTime;

    int ir1_distance_cm = 29.988 * pow(analogRead(IR_SENSOR_1) * (3.3 / 4096), -1.173);
    int ir2_distance_cm = 29.988 * pow(analogRead(IR_SENSOR_2) * (3.3 / 4096), -1.173);

    if (ir1_distance_cm <= IRDistance && (ir1_distance_cm < 20 || ir1_distance_cm > 30))
      return STRAIGHT;

    if (ir2_distance_cm <= IRDistance && (ir2_distance_cm < 20 || ir2_distance_cm > 30))
      return ROTATE;

    return NONE;
  }
}

void robotPosition()
{
  roboSumo = sonar();

  if (roboSumo == NONE)
  {
    roboSumo = IRSensor();
  }

  switch(roboSumo)
  {
    case STRAIGHT:
      posStraight();
      break;

    case LEFT:
      posRight();
      delay(tornTime);
      break;

    case RIGHT:
      posLeft();
      delay(tornTime);
      break;

    case ROTATE:
      posLeft();
      delay(rotateTime);
      break;

    case NONE:
      posStop();
      break;
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
      delay(300);
      break;
    
    case FIGHT_LEFT:
      posRight();
      delay(tornTime);
      posStraight();
      delay(300);
      break;

    case FIGHT_RIGHT:
      posLeft();
      delay(tornTime);
      posStraight();
      delay(300);
      break;

    case FIGHT_BACK:
      posLeft();
      delay(rotateTime);
      posStraight();
      delay(300);
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

void displayCount(void)
{
  for (int i = 5; i > 0; i--)
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
  delay(2000);
}








