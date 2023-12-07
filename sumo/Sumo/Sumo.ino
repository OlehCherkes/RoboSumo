#include <AFMotor.h>
AF_DCMotor motor1(1);     
AF_DCMotor motor2(2);

#define IR A0
#define IR2 A3

int trigPin = 9;
int echoPin = 10;
int davomiylik, sm;

void plusUltra() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(5);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(15);
  digitalWrite(trigPin, LOW);
  davomiylik = pulseIn(echoPin, HIGH);
  sm = davomiylik / 58;
  //Serial.println(sm);
}

void setup() {
  Serial.begin(9600);

  pinMode(IR, INPUT);
  pinMode(IR2, INPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  delay(2000);             // 5 Sekund kutib turish
  motor1.setSpeed(255);    // Tezlik 0 dan 255 gacha o'zgartirish mumkin
  motor1.run(RELEASE);     
  motor2.setSpeed(255);    // Tezlik 2- Motor uchun
  motor2.run(RELEASE);
}

void loop() {
  plusUltra();
  //Serial.println(sm);

  if ((sm > 1 && sm < 25))
  {
    Serial.println("front");

    motor1.run(FORWARD);
    motor2.run(FORWARD);
    delay(300);
  }
  else
  {
    Serial.println("normal");
    motor1.run(BACKWARD);
    motor2.run(FORWARD);
    delay(10);
  }
  
  if(!digitalRead(IR))
  {
    Serial.println("left");
    motor1.run(FORWARD);
    motor2.run(FORWARD);
    delay(300);
  }
  
  if(!digitalRead(IR2))
  {
    Serial.println("right");
    motor1.run(BACKWARD);
    motor2.run(BACKWARD);
    delay(300);
  }
}
