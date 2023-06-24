#include <Arduino.h>
#include <BluetoothSerial.h>
#include <AlfredoConnect.h>
#include <Alfredo_NoU2.h>

#include "IMU.h"

#include "mecanumDrivetrain.h"

// define bluetooth serial connection

BluetoothSerial serialBT;
String robotName = "roboeaglets";

// define serial to other micros
// espCam camera;
// auxIO io;

// define motors
NoU_Motor frontLeftMotor(2);
NoU_Motor frontRightMotor(1);
NoU_Motor backLeftMotor(4);
NoU_Motor backRightMotor(3);
NoU_Motor turretMotor(5);

// define drivetrain
mecanumDrivetrain drivetrain = mecanumDrivetrain(&frontLeftMotor, &frontRightMotor, &backLeftMotor, &backRightMotor);

// define servos
NoU_Servo intakeServo(1);
NoU_Servo fourBarServo(2);
NoU_Servo secondJointServo(3);

// define imu and magnetic encoder objects
IMU imu;

// read from sensors
bool sensorRead = false;

unsigned long lastSent = millis();

bool enabled = false;

void setup() {

  // begin DS comms
  serialBT.begin(robotName);
  AlfredoConnect.begin(serialBT);

  // start RSL
  RSL::initialize();

  // start IMU
  //imu.begin(5, 4);

  // set direction of motors
  frontLeftMotor.setInverted(false);
  frontRightMotor.setInverted(true);
  backLeftMotor.setInverted(false);
  backRightMotor.setInverted(true);
}

bool firstWrite = true;
void loop() {

  // update IMU
  //imu.read();

  // RSL logic
  if (enabled) {
    RSL::setState(RSL_ENABLED);
  }
  else {
    RSL::setState(RSL_DISABLED);
  }

  // get values from driver station
  enabled = AlfredoConnect.buttonHeld(0, 4);
  double linearX = AlfredoConnect.getRawAxis(0, 1);
  double linearY = AlfredoConnect.getRawAxis(0, 0);
  double angularZ = AlfredoConnect.getRawAxis(0, 3);

  // only write to hardware if enabled
  if(enabled){
    drivetrain.set(linearX, linearY, angularZ);
  }

  // drivetrain e-stop if disabled
  if(!enabled){
    drivetrain.set(0, 0, 0);
  }

}

