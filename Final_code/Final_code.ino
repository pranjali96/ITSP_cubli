#include "I2Cdev.h"
#include "MPU6050.h"
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
#include "Wire.h"
#endif

MPU6050 accelgyro;

int16_t ax, ay, az;
int16_t gx, gy, gz;


float torque=0, voltage=0, rpm=0, angular_velocity=0, value=0;
unsigned long int current_time=0, previous_time=0, time=0.5, dt;
const float m_body = 0.25, m_wheel = 0.065, MoI_body = 0.0006, MoI_wheel = 0.0006, l_body = 0.0075, l_wheel = 0.0075, g=9.8;
int motorForward=10;
int motorReverse=9;
double arx = 0, grx = 0;
double speeed = 0;
float time_const = 1000;
double filter_angle;
double x;
double prev_ang[10];
float avg_ang;
float old_avg;
int to_change = 1;

void setup() 
{
 
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
  Wire.begin();
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
  Fastwire::setup(400, true);
  #endif
  
  Serial.begin(38400);
  accelgyro.initialize();

  accelgyro.setXGyroOffset(20);
  accelgyro.setYGyroOffset(-28);
  accelgyro.setZGyroOffset(7);
  accelgyro.setXAccelOffset(-84);
  accelgyro.setYAccelOffset(2704);
  accelgyro.setZAccelOffset(1528);

  pinMode(9,OUTPUT);
  pinMode(10,OUTPUT);
  delay(1000);
  speeed = 0;
  previous_time = millis();
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
  grx = (180/3.141592) * atan(ax / sqrt(square(ay) + square(az)));
  prev_ang[9] = prev_ang[8] = prev_ang[7] = prev_ang[6] = prev_ang[5]
    = prev_ang[4] = prev_ang[3] = prev_ang[2] = prev_ang[1] = prev_ang[0] = grx;

  avg_ang = grx;
}

void loop() 
{    
  accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    
  arx = (180/3.141592) * atan(ax / sqrt(square(ay) + square(az)));     
  grx += gy/131.0;
  
  Serial.print("Angle:");
  Serial.println(arx);
  //torque=1.05*(m_body*l_body + m_wheel*l_wheel)*g*sin(arx*180/3.14);
  
  //Serial.print("Speed ");
  //Serial.println(speeed);
  
  current_time = millis();
  dt = current_time - previous_time;
  previous_time = current_time;
  x = time_const/(time_const + dt);
  filter_angle = (1-x)*grx + (x)*arx;
  //filter_angle = (x)*(grx + filter_angle) + (1 - x)*arx;
  Serial.print("Filtered:    ");
  Serial.println(filter_angle);
  //speeed += filter_angle*dt;
  
  if(old_avg*avg_ang < 0)
    to_change = 0;
  else
    to_change = 1;
    
  if(to_change == 1)  
  {
  
  if(avg_ang>2 && avg_ang < 15)
  {
    //analogWrite(motorReverse, 10*filter_angle);
    analogWrite(motorReverse, avg_ang*17);
    //analogWrite(motorReverse, 255);
    analogWrite(motorForward,0);
  }
  else if(avg_ang<-2 && avg_ang>-10)
  {
    //analogWrite(motorForward, 10*filter_angle);
    analogWrite(motorForward, avg_ang*17);
    //analogWrite(motorForward, 255);
    analogWrite(motorReverse,0);
  }
  else if(avg_ang<=2 && avg_ang>=-2)
  {
    analogWrite(motorForward, 0);
    analogWrite(motorReverse,0);
  }
  /* 
  else if(avg_ang<-23)
  {
    grx = -7.5;
    analogWrite(motorReverse,250);
    analogWrite(motorForward,0);
    delay(700);
    analogWrite(motorForward,250);
    analogWrite(motorReverse,0);
    delay(500);
    for(int i=20; i>0; i--)
    {
      analogWrite(motorForward,i*10);
      delay(100);
    }
  }
   
  else if(avg_ang>12)
  {
    grx = 15;
    analogWrite(motorReverse,0);
    analogWrite(motorForward,200);
    delay(110);
    analogWrite(motorForward,0);
    analogWrite(motorReverse,200);
    delay(500);
    for(int i=20; i>0; i--)
    {
      analogWrite(motorReverse,i*10);
      delay(50);
    }
    
  }*/
  }
    
  avg_ang += (prev_ang[0]/10.0 - prev_ang[9]/10.0);
  Serial.print("Avg_ang:           "); Serial.println(avg_ang);
  
  old_avg = avg_ang;
  
  for(int i = 0; i<9; i++)
  {
    prev_ang[9-i] = prev_ang[8-i];
  }
  prev_ang[0] = filter_angle;
  
  if(abs(prev_ang[0] - prev_ang[9])<1)
  {
    for(int i = 0; i<10; i++)
    {
      prev_ang[i] = arx;
    }
    grx = arx;
    avg_ang = arx;
  }
  
}  
  
   

    
    


