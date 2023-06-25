#include <Arduino.h>
#include <BluetoothSerial.h>
#include <AlfredoConnect.h>
#include <Alfredo_NoU2.h>
#include <HardwareSerial.h>
#include <AS5600.h>

#include "IMU.h"
#include "mecanumDrivetrain.h"
#include "Arm.h"
#include "EncoderComms.h"
#include "CameraComms.h"

// define bluetooth serial connection

BluetoothSerial serialBT;
String robotName = "roboeaglets";

// define serial to ESP32-Cam
HardwareSerial cameraPort(0);
CamComms camera;

// define serial to Encoder read
HardwareSerial encoderPort(1);
EncoderComms encoder;

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

// define arm
Arm arm = Arm(&fourBarServo, &secondJointServo);

// define imu and magnetic encoder objects
TwoWire wire;
IMU imu;
AS5600 turretEncoder(&wire);


// enable logic and debounce
bool enabled = false;
unsigned long lastEnableRead = millis();

void setup() {

  // begin DS comms
  serialBT.begin(robotName);
  AlfredoConnect.begin(serialBT);

  // start RSL
  RSL::initialize();

  // start I2C
  wire.begin(5, 4);
  wire.setClock(400000);

  // start imu
  imu.begin(wire);

  // start magnetic encoder
  turretEncoder.begin();

  // start comms to camera
  cameraPort.begin(9600,SERIAL_8N1,3,1);
  camera.begin(&cameraPort);


  // start comms to encoders
  encoderPort.begin(9600,SERIAL_8N1,35,-1);
  encoder.begin(&encoderPort);


  // set direction of motors
  frontLeftMotor.setInverted(false);
  frontRightMotor.setInverted(true);
  backLeftMotor.setInverted(false);
  backRightMotor.setInverted(true);  
}

void loop() {

  // parse updates from IMU
  //imu.read();

  // parse updates from camera
  camera.read();

  // parse updates from encoders
  encoder.read();

  AlfredoConnect.update();

  // get values from driver station
  if(AlfredoConnect.buttonHeld(0, 9) && millis() - lastEnableRead > 300){
    lastEnableRead = millis();
    enabled = !enabled;
  }

  if(enabled){
    RSL::setState(RSL_ENABLED);
  }
  else{
    RSL::setState(RSL_DISABLED);
  }
  RSL::update();


  //enabled = true;
  double linearX = -AlfredoConnect.getAxis(0, 0);
  double linearY = AlfredoConnect.getAxis(0, 1);
  double angularZ = -AlfredoConnect.getAxis(0, 2);

  //serialBT.println("LinearX: " + String(linearX) + " LinearY: " +String(linearY) + " AngularZ: " + String(angularZ));

  // only write to hardware if enabled
  if(enabled){    
    drivetrain.set(linearX, linearY, angularZ);
  }

  // drivetrain e-stop if disabled
  if(!enabled){
    frontLeftMotor.set(0.0);
    frontRightMotor.set(0.0);
    backLeftMotor.set(0.0);
    backRightMotor.set(0.0);
  }
}

