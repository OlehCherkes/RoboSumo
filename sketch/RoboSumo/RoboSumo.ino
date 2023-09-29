
#include <SharpIR.h>

SharpIR sensor(SharpIR::GP2Y0A21YK0F, 34);

void setup()
{
  Serial.begin(9600);
}

void loop()
{
//    for(int i = 180; i > 0; i--)
//   {  
//     Serial.print(i);
//     Serial.print(",");
//     Serial.print(sensor.getDistanceFilter(40));
//     Serial.print(".");

//     delay(30);
//   }



  Serial.println(analogRead(34));
  delay(300);
}





























