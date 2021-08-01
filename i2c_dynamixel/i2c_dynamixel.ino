#include <Wire.h>
#include <DynamixelShield.h>
#include <Servo.h>

Servo grabber;

#define SLAVE_ADDRESS 0x04

volatile boolean receiveFlag = false;
char temp[32];
String command;

const uint8_t DXL_1 = 1;
const uint8_t DXL_2 = 2;
const uint8_t DXL_3 = 3;

const float DXL_PROTOCOL_VERSION = 2.0;

DynamixelShield dxl;
using namespace ControlTableItem;

void setup() {

  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(receiveEvent);

  dxl.begin(57600);
  dxl.setPortProtocolVersion(DXL_PROTOCOL_VERSION);
  // Check if active
  dxl.ping(DXL_1);
  dxl.ping(DXL_2);
  dxl.ping(DXL_3);

  // Turn off torque when configuring items in EEPROM area
  dxl.torqueOff(DXL_1);
  dxl.torqueOff(DXL_2);
  dxl.torqueOff(DXL_3);

  dxl.setOperatingMode(DXL_1, OP_POSITION);
  dxl.setOperatingMode(DXL_2, OP_POSITION);
  dxl.setOperatingMode(DXL_3, OP_POSITION);

  dxl.torqueOn(DXL_1);
  dxl.torqueOn(DXL_2);
  dxl.torqueOn(DXL_3);

  dxl.writeControlTableItem(GOAL_TORQUE, DXL_1, 1023);
  dxl.writeControlTableItem(GOAL_CURRENT, DXL_1, 1023);
   dxl.writeControlTableItem(CURRENT_LIMIT, DXL_1, 1023);
  dxl.writeControlTableItem(TORQUE_LIMIT, DXL_1, 1023);
  dxl.writeControlTableItem(PROFILE_ACCELERATION, DXL_1, 1023); 
  dxl.writeControlTableItem(PROFILE_VELOCITY, DXL_1,1023);
  dxl.writeControlTableItem(PROFILE_ACCELERATION, DXL_2, 50); 
  dxl.writeControlTableItem(PROFILE_VELOCITY, DXL_2,130);
  dxl.writeControlTableItem(PROFILE_ACCELERATION, DXL_3, 50); 
  dxl.writeControlTableItem(PROFILE_VELOCITY, DXL_3, 130);
 
  dxl.setGoalPosition(DXL_3, 180.0, UNIT_DEGREE); //elbow    - angular
  dxl.setGoalPosition(DXL_2, 180.0, UNIT_DEGREE); //wrist    - angular
  dxl.setGoalPosition(DXL_1, 90.0, UNIT_DEGREE); //EEF      - angular

  grabber.attach(5);
  grabber.writeMicroseconds(1000);
  delay(2500);

}

void loop() {

  if (receiveFlag == true) {
    if (temp[0] == '1'){
      String ang = temp;
      float angle = ang.substring(1).toFloat();
      if (angle!=999)
      {
        if (angle == 0) //open
          grabber.writeMicroseconds(1000);
        else if (angle == 1) //close
          grabber.writeMicroseconds(1500);
      }
    }
    else if (temp[0] == '2'){
      String ang = temp;
      float angle = ang.substring(1).toFloat();
      if (angle!=999)
        dxl.setGoalPosition(DXL_2, angle, UNIT_DEGREE);
    }
    else if (temp[0] == '3'){
      String ang = temp;
      float angle = ang.substring(1).toFloat();
      if (angle!=999)
        dxl.setGoalPosition(DXL_3, angle, UNIT_DEGREE);
    }
    
    receiveFlag = false;
  }
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
