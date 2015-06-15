
#include "I2Cdev.h"
#include "MPU6050.h"

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

// class default I2C address is 0x68
// specific I2C addresses may be passed as a parameter here
// AD0 low = 0x68 (default for InvenSense evaluation board)
// AD0 high = 0x69
MPU6050 accelgyro;
//MPU6050 accelgyro(0x69); // <-- use for AD0 high

int16_t ax, ay, az;
int16_t gx, gy, gz;

float torque=0, angle, ang_velocity, ang_acceleration;
const float m_body = 0.25, m_wheel = 0.065, MoI_body = 0.0006, MoI_wheel = 0.0006, l_body = 0.0075, l_wheel = 0.0075, g=9.8;
const float fric_wheel = 0, fric_pivot = 0;
float cp, ci, cd, kp, ki, kd;
float current_time, previous_time, dt, ang_past;
float error=0;

void setup() {
    // join I2C bus (I2Cdev library doesn't do this automatically)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif

    // (38400 chosen because it works as well at 8MHz as it does at 16MHz...something else will also be fine
    Serial.begin(38400);
    
    accelgyro.initialize();

    // verify connection
    Serial.println("Testing device connections...");
    Serial.println(accelgyro.testConnection() ? "MPU6050 connection successful" : "MPU6050 connection failed");

    // use the code below to change accel/gyro offset values
    
    Serial.println("Updating internal sensor offsets...");
    // -76	-2359	1688	0	0	0
    Serial.print(accelgyro.getXAccelOffset()); Serial.print("\t"); // -76
    Serial.print(accelgyro.getYAccelOffset()); Serial.print("\t"); // -2359
    Serial.print(accelgyro.getZAccelOffset()); Serial.print("\t"); // 1688
    Serial.print(accelgyro.getXGyroOffset()); Serial.print("\t"); // 0
    Serial.print(accelgyro.getYGyroOffset()); Serial.print("\t"); // 0
    Serial.print(accelgyro.getZGyroOffset()); Serial.print("\t"); // 0
    Serial.print("\n");
    /*accelgyro.setXGyroOffset(220);
    accelgyro.setYGyroOffset(76);
    accelgyro.setZGyroOffset(-85);*/
    
    accelgyro.setXGyroOffset(20);
    accelgyro.setYGyroOffset(-28);
    accelgyro.setZGyroOffset(7);
    accelgyro.setXAccelOffset(-84);
    accelgyro.setYAccelOffset(2704);
    accelgyro.setZAccelOffset(1528);

    Serial.print(accelgyro.getXAccelOffset()); Serial.print("\t"); // -76
    Serial.print(accelgyro.getYAccelOffset()); Serial.print("\t"); // -2359
    Serial.print(accelgyro.getZAccelOffset()); Serial.print("\t"); // 1688
    Serial.print(accelgyro.getXGyroOffset()); Serial.print("\t"); // 0
    Serial.print(accelgyro.getYGyroOffset()); Serial.print("\t"); // 0
    Serial.print(accelgyro.getZGyroOffset()); Serial.print("\t"); // 0
    Serial.print("\n");

}

void loop() {
    
    current_time = millis();  
  
    // read raw accel/gyro measurements from device
    accelgyro.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

    // these methods (and a few others) are also available
    //accelgyro.getAcceleration(&ax, &ay, &az);
    //accelgyro.getRotation(&gx, &gy, &gz);
    
    double arx = (180/3.141592) * atan(ax / sqrt(square(ay) + square(az))); 
    double ary = (180/3.141592) * atan(ay / sqrt(square(ax) + square(az)));
    double arz = (180/3.141592) * atan(sqrt(square(ay) + square(ax)) / az);

    Serial.print("Angles: "); Serial.print("\t");
    Serial.print(arx); Serial.print("\t");
    Serial.print(ary); Serial.print("\t");
    Serial.println(arz); 
    //Serial.print("Ang. velocities: "); Serial.print("\t");
    //Serial.print((float)gx/131); Serial.print("\t");
    //Serial.print((float)gy/131); Serial.print("\t");
    //Serial.println((float)gz/131);
    //Serial.print("Acceleration: "); Serial.print("\t");
    //Serial.print((float)(ax*9.8)/16384); Serial.print("\t");
    //Serial.print((float)(ay*9.8)/16384); Serial.print("\t");
    //Serial.println((float)(az*9.8)/16384);
    
    Serial.print("a/g:\t");
    Serial.print(ax); Serial.print("\t");
    Serial.print(ay); Serial.print("\t");
    Serial.print(az); Serial.print("\t");
    Serial.print(gx); Serial.print("\t");
    Serial.print(gy); Serial.print("\t");
    Serial.print(gz); Serial.print("\n");
    
    //check if correct...
    angle = arx;
    ang_velocity = (float)gx/131;         //why the literal 131?
    
    //The PID part...we can implement it in 2 ways...one in which we see the angle and the other where we compare with expected acc and obtained acc
    //the second method will allow us to exactly control the manner in which return occurs while first one will be simpler to implement.
    
    //for 1st way uncomment this...
    /*
    torque = (m_body*l_body + m_wheel*l_wheel)*g*sin(angle) - fric_pivot*ang_velocity + cp*angle;
    */
    
  //for 2nd way uncomment this...
  {
    //get ang_acceleration and check with the expected one to make corrections in the next loop..
    dt = current_time - previous_time;
    ang_acceleration = (ang_velocity - ang_past)/dt;
    ang_past = ang_velocity;
    
    error = (torque - ((m_body*l_body + m_wheel*l_wheel)*g*sin(angle) - fric_pivot*ang_velocity))/(MoI_body + MoI_wheel) - ang_acceleration;
    
    //the required torque to lift the cubli back
    torque = (m_body*l_body + m_wheel*l_wheel)*g*angle - fric_pivot*ang_velocity;
    
    torque += kp*error;   
  }
    
    delay(50);
    previous_time = current_time;
}
