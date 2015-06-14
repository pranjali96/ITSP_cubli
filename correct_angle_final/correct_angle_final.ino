#include "I2Cdev.h"
#include "MPU6050.h"
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

MPU6050 accelgyro;

int16_t ax, ay, az;
int16_t gx, gy, gz;

float current_time, previous_time, dt;
double filter_angle;
float time_const = 1000;
double arx, ary, arz;
double grx, gry, grz;
double x;
double prev_ang[10];
float avg_ang;

void setup() {
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

    previous_time = millis();
    delay(2000);
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    grx = (180/3.141592) * atan(ax / sqrt(square(ay) + square(az)));
    
    prev_ang[9] = prev_ang[8] = prev_ang[7] = prev_ang[6] = prev_ang[5]
    = prev_ang[4] = prev_ang[3] = prev_ang[2] = prev_ang[1] = prev_ang[0] = grx;
    avg_ang = grx;
}

void loop() {
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    
    arx = (180/3.141592) * atan(ax / sqrt(square(ay) + square(az)));     
    grx += gy/131.0;         //why the literal 131?
    
    Serial.print("Angles: ");
    Serial.println(arx);
    Serial.print("G_Angle");
    Serial.println(grx);
    Serial.println(gy/130.0);
    
    Serial.print("Filtered angle");
    
    current_time = millis();
    dt = current_time - previous_time;
    previous_time = current_time;
    x = time_const/(time_const + dt);
    
    filter_angle = (1-x)*(grx) + (x)*arx;
    Serial.println(filter_angle);
    
    //changing the average...
    avg_ang += (prev_ang[0]/10.0 - prev_ang[9]/10.0);
    Serial.print("Avg_ang: "); Serial.println(avg_ang);
  
    for(int i = 0; i<9; i++)
    {
      prev_ang[9-i] = prev_ang[8-i];
    }
    
    prev_ang[0] = filter_angle;
    
    //now to take reading by accelerometer if the reading stays relativey stable...
    if(abs(prev_ang[0] - prev_ang[9])<1)
    {
      for(int i = 0; i<10; i++)
      {
        prev_ang[i] = arx;
      }
      grx = arx;
      avg_ang = arx;
    }
        
    //delay(5);
}
