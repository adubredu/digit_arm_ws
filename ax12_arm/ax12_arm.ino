#include <ax12.h>
#include <Wire.h>
#include <Servo.h>

#define SLAVE_ADDRESS 0X04
#define SPEED_REG 32

volatile boolean receiveFlag = false;
char temp[32];
String command;
Servo grabber;

const int yawID = 1;
const int pitchID = 2;
const float unit_degree = 0.29;
int grabber_pos = 1000;



 
 
 
 //yaw-min=200 (0 deg) max=818(180 deg)
 void setup(){
   ax12SetRegister2(yawID, SPEED_REG, 100);
   ax12SetRegister2(pitchID, SPEED_REG,100);
  //SetPosition(yawID, 512);
  ///delay(1000);
  //SetPosition(yawID, 100);
  
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveEvent);
  grabber.attach(13);
  //grabber.writeMicroseconds(1500);
  //delay(3000);
  grabber.writeMicroseconds(1000);
    
  delay(2500);
   
 
 }
   
 void loop()
 {
   
 if (receiveFlag == true) {
    if (temp[0] == '1'){
      String ang = temp;
      char buffer[10];
      ang.substring(1).toCharArray(buffer,10);
      float angle = atof(buffer);
      if (angle!=999)
        {
          if (angle==0)
          {
            while (grabber_pos > 1000)
            {
              grabber_pos -= 10;
              grabber.writeMicroseconds(grabber_pos);
              delay(50); //take 2.5 seconds to fully open
            }
          }
           // grabber.writeMicroseconds(1000);
          else if (angle ==1)
          {
            while (grabber_pos < 1500)
            {
              grabber_pos += 10;
              grabber.writeMicroseconds(grabber_pos);
              delay(50); //take 2.5 seconds to fully close
            }
          }
           // grabber.writeMicroseconds(1500);
        }
    }
    else if (temp[0] == '2'){
      String ang = temp;
      char buffer[10];
      ang.substring(1).toCharArray(buffer,10);
      float angle = atof(buffer);
      if (angle!=999)
        set_servo_position_angle(yawID, angle);
    }
    else if (temp[0] == '3'){
      String ang = temp;
      char buffer[10];
      ang.substring(1).toCharArray(buffer,10);
      float angle = atof(buffer);
      if (angle!=999)
        set_servo_position_angle(pitchID, angle);
    }
    
    receiveFlag = false;
  }
 }
 
 
 int degree_to_pwm(float degree)
 {
   return (int)round(degree/unit_degree);
 }
 
 
 float pwm_to_degree(int pwm)
 {
   return pwm*unit_degree;
 }
 
 
 void set_servo_position_angle(int servoID, float angle)
 {
   int pwm = degree_to_pwm(angle);
   SetPosition(servoID, pwm);
 }
 
 
 float get_servo_position_angle(int servoID)
 {
   int pwm = GetPosition(servoID);
   return pwm_to_degree(pwm);
 }
 
 void receiveEvent(int howMany) {

  for (int i = 0; i < howMany; i++) {
    temp[i] = Wire.read();
    temp[i + 1] = '\0'; //add null after ea. char
  }

  //RPi first byte is cmd byte so shift everything to the left 1 pos so temp contains our string
  for (int i = 0; i < howMany; ++i)
    temp[i] = temp[i + 1];

  receiveFlag = true;
}
