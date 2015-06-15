/*#include <Servo.h>

Servo motor;

float x;
int setupDone=0;

void setup()
{
  motor.attach(9);
  Serial.begin(9600);
  Serial.println("initializing...i.e. tp");
}


void loop()
{
  if(Serial.available()>0 && setupDone==0)
  {
    motor.write(180);
    setupDone=1;
    Serial.println("written 180");
    delay(2000);
  }
  if(Serial.available()>0 && setupDone==1)
  {
    motor.write(0);
    setupDone=2;
    Serial.println("written 0");
    delay(2000);
  }
  if(Serial.available()>0 && setupDone==2)
  {
    motor.write(90);
    setupDone=3;
    Serial.println("written 90");
    delay(2000);
  }
  if(Serial.available()>0 && setupDone==3)
  {
    x=Serial.read();
    if(x>0 && x<100)
    {
      motor.write(x);
      Serial.println("written x");
      //delay(2000);
    }
  }
}*/

int motorForward = 9;
int motorReverse =10;

//Gradual changes
//Back emf
void setup()
{
  pinMode(motorForward, OUTPUT);
  pinMode(motorReverse, OUTPUT);
}

void loop()
{
  
  //analogWrite(motorForward,0);
  //analogWrite(motorReverse,0);
  //delay(5000);
  
  delay(5000);
  
  analogWrite(motorForward,64);
  delay(2000);
  analogWrite(motorForward,128);
  delay(2000);
  analogWrite(motorForward,200);
  delay(3000);
  //analogWrite(motorReverse,0);
  //delay(5000);
  //analogWrite(motorForward,128);
  //delay(1000);
  
  
  //analogWrite(motorForward,0);
  //delay(2000);
  
  //analogWrite(motorReverse,128);
  //delay(1000);
  analogWrite(motorForward,0);
  //analogWrite(motorReverse,0);
  delay(5000);
  //analogWrite(motorReverse,128);
  //delay(1000);
  
  
}
