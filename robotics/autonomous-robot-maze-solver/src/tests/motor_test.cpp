#include <Arduino.h>
//Arduino PLL Speed Control：
int E1 = 4;
int M1 = 5;
int E2 = 7;
int M2 = 6;

void setup()
{
    pinMode(M1, OUTPUT);
    pinMode(M2, OUTPUT);
}

void loop()
{
  int value;
  for(value = 0 ; value <= 255; value+=5)
  {
    digitalWrite(M1,HIGH);
    //digitalWrite(M2, HIGH);
    analogWrite(E1, value);   //PLL Speed Control
    //analogWrite(E2, value);   //PLL Speed Control
    delay(30);
  }
}
