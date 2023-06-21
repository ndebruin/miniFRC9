#include <Arduino.h>
#include <BluetoothSerial.h>
#include <Alfredo_NoU2.h>
#include <ICM_20948.h>

#include "IMU.h"

// define bluetooth serial connection
BluetoothSerial bluetooth;
String robotName = "roboeaglets";

// define motors
NoU_Motor leftMotor(6);
NoU_Motor rightMotor(5);

// define drivetrain
NoU_Drivetrain drivetrain(&leftMotor, &rightMotor);

IMU imu;

// define values passed over bt serial
float linearX, angularZ;
unsigned long lastTimePacketReceived = 0;


void setup() {
  // put your setup code here, to run once:
  
  // begin bluetooth connection
  bluetooth.begin(robotName);

  // start RSL
  RSL::initialize();

  // start IMU
  imu.begin(5, 4);

  // set direction of motors
  leftMotor.setInverted(false);
  rightMotor.setInverted(true);
}

void loop() {
  // put your main code here, to run repeatedly:

  // if we have a bluetooth packet
  // pull data
  while (bluetooth.available() > 0) {
    lastTimePacketReceived = millis();
    linearX = -bluetooth.readStringUntil(',').toFloat();
    angularZ = bluetooth.readStringUntil('\n').toFloat();
  }

  // update IMU
  imu.read();

  // RSL logic
  if (millis() - lastTimePacketReceived > 100) {
    RSL::setState(RSL_DISABLED);
  }
  else {
    RSL::setState(RSL_ENABLED);
  }
  RSL::update();


  // write data to drivetrain
  // NOTE: is expecting -1 -> 1 values straight from joystick
  drivetrain.arcadeDrive(linearX, angularZ);
  
}

