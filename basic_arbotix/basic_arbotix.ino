#include <ax12.h>  

#define SPEED_REG 32
const float unit_degree = 0.29;

const int servo1 = 1;
const int servo2 = 2;


 void setup(){
   ax12SetRegister2(servo1, SPEED_REG, 100);
   ax12SetRegister2(servo2, SPEED_REG,100);
 }
 
 void loop(){
   set_servo_position_angle(servo1, 90);
   set_servo_position_angle(servo2, 180);
   delay(2000);
   
   set_servo_position_angle(servo1, 180);
   set_servo_position_angle(servo2, 90);
   delay(2000);
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
