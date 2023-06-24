#include <Arduino.h>
#include <BluetoothSerial.h>
#include <Alfredo_NoU2.h>
#include <ICM_20948.h>

#include "IMU.h"
#include "DriverStationComms.h"

// define bluetooth serial connection
DriverStation DS;
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

// define servos
NoU_Servo intakeServo(1);
NoU_Servo fourBarServo(2);
NoU_Servo secondJointServo(3);

// define imu and magnetic encoder objects
IMU imu;

// read from sensors
bool sensorRead = false;

unsigned long lastSent = millis();

// BluetoothSerial btSerial;

void setup() {

  // begin DS comms
  DS.begin(robotName);

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

  // check for updates from DS
  DS.read();

  // send updates to DS
  //DS.write();

  // RSL logic
  if (DS.enabled) {
    RSL::setState(RSL_ENABLED);
  }
  else {
    RSL::setState(RSL_DISABLED);
  }
  RSL::update();
  

  // only write to hardware if enabled
  if(DS.enabled)
  {
    frontLeftMotor.set(DS.frontLeft);
    frontRightMotor.set(DS.frontRight);
    backLeftMotor.set(DS.backLeft);
    backRightMotor.set(DS.backRight);
    turretMotor.set(DS.turret);

    
  }

  if(DS.armPreset == 0){
    fourBarServo.write(0);
    secondJointServo.write(0);
  }
          
  /*if(sensorRead){
    DS.pitch = imu.getPitch();
    DS.roll = imu.getRoll();
    DS.yaw = imu.getYaw();

    DS.turretAngle = 0;
  }*/

  // drivetrain kill if disabled
  if(!DS.enabled){
    frontLeftMotor.set(0);
    frontRightMotor.set(0);
    backLeftMotor.set(0);
    backRightMotor.set(0);
  }

}

