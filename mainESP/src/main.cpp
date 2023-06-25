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

  // start IMU
  //imu.begin(5, 4);

  // set direction of motors
  frontLeftMotor.setInverted(false);
  frontRightMotor.setInverted(true);
  backLeftMotor.setInverted(false);
  backRightMotor.setInverted(true);

  // wait for DS comms
}

bool lastPacket = millis();

bool firstWrite = true;
void loop() {

  // update IMU
  //imu.read();

  AlfredoConnect.update();

  // get values from driver station
  enabled = AlfredoConnect.buttonHeld(0, 4);
  double linearX = -AlfredoConnect.getAxis(0, 0);
  double linearY = AlfredoConnect.getAxis(0, 1);
  double angularZ = AlfredoConnect.getAxis(0, 2);

  /*if(abs(linearX) < 0.01){
    linearX = 0;
  }
  if(abs(linearY) < 0.01){
    linearY = 0;
  }
  if(abs(angularZ) < 0.01){
    angularZ = 0;
  }*/


  // serialBT.println(String(linearX) + " " +String(linearY) + " " + String(angularZ));

  // only write to hardware if enabled
  if(enabled){
    serialBT.println(drivetrain.set(linearX, linearY, angularZ));
  }

  // drivetrain e-stop if disabled
  if(!enabled){
    drivetrain.set(0, 0, 0);
  }

}

