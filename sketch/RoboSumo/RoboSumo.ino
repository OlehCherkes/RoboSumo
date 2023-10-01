
#include <SharpIR.h>
#include <Servo.h>

SharpIR sensor1(SharpIR::GP2Y0A21YK0F, A0);
SharpIR sensor2(SharpIR::GP2Y0A21YK0F, A1);
Servo myservo;

void servoScan();

//----------- SERVO ----------//
uint32_t tmr;   // переменная таймера
int val = 0;    // переменная яркости
int dir = 2;    // скорость и направление яркости

void setup()
{
  myservo.attach(9);
  Serial.begin(9600);
  myservo.write(0);
  delay(1000);
}

void loop()
{
  //servoScan();
    // for (int angle = 1, direction = 1; ; angle += direction) {
    //     // Ваш код
    //     myservo.write(angle);
    //     Serial.print(angle);
    //     Serial.print(",");
    //     Serial.print(sensor.getDistanceFilter(40));
    //     Serial.print(".");
        
    //     delay(10);

    //     // Если достигнуто 180 или 0, меняем направление
    //     if (angle == 180 || angle == 0) {
    //         direction *= -1;
    //     }
    // }

    servoScan();


}

void servoScan()
{
  if (millis() - tmr >= 100) {
    tmr = millis();
    val += dir;                             // прибавляем скорость
    if (val >= 180 || val <= 0) dir = -dir;  // разворачиваем
    myservo.write(val);                     // применяем

    static uint32_t lastDistance = 0;
    static uint32_t lastDistance2 = 0;

    uint32_t currentDistance = sensor1.getDistance();
    uint32_t currentDistance2 = sensor2.getDistance();

    if (currentDistance != lastDistance )
    {
        lastDistance = currentDistance;

    }

    if (currentDistance2 != lastDistance2)
    {
        lastDistance2 = currentDistance2;
    }

    Serial.print(val); 
    Serial.print(",");
    Serial.print(currentDistance);
    Serial.print(",");
    Serial.print(currentDistance2); 
    Serial.print(".");
    //delay(100);
  }
}



























