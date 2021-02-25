#include <Arduino.h>

/*
    The sample code for driving one way motor encoder.
    link:
       A pin -- digital 2
       B pin -- digital 9
       
    Beacuse of the interrupt port with different board, the following code is only used in UNO and mega2560.
    If you want to use Leonardo or Romeo,you should change digital pin 3 instead of digital pin 2.
    See the link for detail http://arduino.cc/en/Reference/AttachInterrupt
*/

void EncoderInit();
void wheelSpeed();

const byte encoder0pinA = 2;                  //A pin -> the interrupt pin 0
const byte encoder0pinB = 9;                  //B pin -> the digital pin 4
byte encoder0PinALast;
int duration;                                 //the number of the pulses
boolean Direction;                            //the rotation direction 
 
 
void setup(){  
  Serial.begin(9600);                       //Initialize the serial port
  EncoderInit();                              //Initialize the module
}
 
void loop(){
  Serial.print("Pulse:");
  Serial.println(duration);
  //duration = 0;
  delay(100);
}
 
void EncoderInit(){
  Direction = true;                            //default -> Forward  
  pinMode(encoder0pinB,INPUT);  
  attachInterrupt(0, wheelSpeed, CHANGE);
}
 
void wheelSpeed(){
  int Lstate = digitalRead(encoder0pinA);
  if((encoder0PinALast == LOW) && Lstate==HIGH){
    int val = digitalRead(encoder0pinB);
    if(val == LOW && Direction){
      Direction = false;                       //Reverse
    }
    else if(val == HIGH && !Direction){
      Direction = true;                        //Forward
    }
  }
  encoder0PinALast = Lstate;
  if(!Direction)  duration++;
  else  duration--;
}
