#include <AFMotor.h>
#include <NewPing.h>
#include "SharpIR.h"

#define IR_SENSOR_1 A1
#define IR_SENSOR_2 A2
#define LINE_SENSOR A4
#define LED A5

#define IR A0
#define IR2 A3


#define FRONT_HC_TRIG 9
#define FRONT_HC_ECHO 10

AF_DCMotor motor1(1);     
AF_DCMotor motor2(2);

NewPing sonarFront(FRONT_HC_TRIG, FRONT_HC_ECHO, 150);

int sonarDistance = 70;
int IRDistanceFront = 70;
const unsigned long measurementInterval = 100;

void setup()
{
  Serial.begin(9600);

  pinMode(IR, INPUT);
  pinMode(IR2, INPUT);

  pinMode(IR_SENSOR_1, INPUT);
  pinMode(IR_SENSOR_2, INPUT);
  pinMode(LINE_SENSOR, INPUT);
  pinMode(LED, OUTPUT);

  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  delay(1000);
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  delay(1000);
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  delay(1000);
  digitalWrite(LED, HIGH);
  delay(100);
  digitalWrite(LED, LOW);
  delay(1000);


  // Stop
  motor1.setSpeed(255);
  motor1.run(RELEASE);     
  motor2.setSpeed(255);
  motor2.run(RELEASE);
}

void loop()
{
  if (sonar() != 0)
  {
    Serial.println("normal");
    motor1.run(BACKWARD);
    motor2.run(FORWARD);

    digitalWrite(LED, HIGH);
    delay(10);
  }

  // Left
  if(IRSensor_1() != 0)
  {
    Serial.println("left");
    motor1.run(BACKWARD);
    motor2.run(BACKWARD);

    digitalWrite(LED, HIGH);
    delay(300);
  }

  // Right
  if(IRSensor_2() != 0)
  {
    Serial.println("Right");
    motor1.run(FORWARD);
    motor2.run(FORWARD);

    digitalWrite(LED, HIGH);
    delay(300);
  }

  // Back
  if(!digitalRead(IR))
  {
    Serial.println("left");
    motor1.run(FORWARD);
    motor2.run(FORWARD);
    
    digitalWrite(LED, HIGH);
    delay(300);
  }

  if(!digitalRead(IR2))
  {
    Serial.println("right");
    motor1.run(BACKWARD);
    motor2.run(BACKWARD);

    digitalWrite(LED, HIGH);
    delay(300);
  }

  // Line sensor
  if(lineSensor())
  {
    Serial.println("line");
    motor1.run(FORWARD);
    motor2.run(BACKWARD);
    digitalWrite(LED, HIGH);
    delay(500);

    motor1.run(BACKWARD);
    motor2.run(BACKWARD);
    delay(300);
  }

  digitalWrite(LED, LOW);
  motor1.run(BACKWARD);
  motor2.run(FORWARD);
  delay(2);
}

int sonar()
{
  unsigned long currentTime = millis();
  static unsigned long lastMeasurementTime;

  if (currentTime - lastMeasurementTime >= measurementInterval)
  {
    lastMeasurementTime = currentTime;

    unsigned long frontDistance = sonarFront.ping_cm();

    if (frontDistance != 0 && frontDistance < sonarDistance)
      return frontDistance;

    return 0;
  }  
}

int IRSensor_1()
{
  unsigned long currentTime = millis();
  static unsigned long lastMeasurementTime;
  static int lastIr1Distance = -1;

  if (currentTime - lastMeasurementTime >=40)
  {
    lastMeasurementTime = currentTime;

    int ir1_distance_cm = 29.988 * pow(analogRead(IR_SENSOR_1) * (3.3 / 1024), -1.173);
    if (ir1_distance_cm <= IRDistanceFront && (ir1_distance_cm < 37 || ir1_distance_cm > 41))
    {
      if (ir1_distance_cm == lastIr1Distance) 
      {
        lastIr1Distance = ir1_distance_cm;
        return ir1_distance_cm;
      }
    }

    lastIr1Distance = ir1_distance_cm;

    return 0;
  }
}

int IRSensor_2() {
  unsigned long currentTime = millis();
  static unsigned long lastMeasurementTime;
  static int lastIr1Distance = -1;

  if (currentTime - lastMeasurementTime >= 40)
  {
    lastMeasurementTime = currentTime;

    int ir1_distance_cm = 29.988 * pow(analogRead(IR_SENSOR_2) * (3.3 / 1024), -1.173);
    if (ir1_distance_cm <= IRDistanceFront && (ir1_distance_cm < 37 || ir1_distance_cm > 41))
    {
      if (ir1_distance_cm == lastIr1Distance) 
      {
        lastIr1Distance = ir1_distance_cm;
        return ir1_distance_cm;
      }
    }

    lastIr1Distance = ir1_distance_cm;

    return 0;
  }
}

int lineSensor()
{
  if (digitalRead(LINE_SENSOR) == HIGH)
     return 1;

  return 0;
}
