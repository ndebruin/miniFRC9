#include <Arduino.h>
#include <BluetoothSerial.h>
#include <AlfredoConnect.h>
#include <Alfredo_NoU2.h>
#include <ESP32Encoder.h>

#include "IMU.h"
#include "mecanumDrivetrain.h"
#include "Arm.h"
// #include "EncoderComms.h"
// #include "CameraComms.h"

/////////////////////////////////// Function Declarations ///////////////////////////////////

// function def
double deadzone(double rawJoy);
double deadzoneVal = 0.05;
void taxiAuton();
void driveInches(double inches, double linearX, double linearY, double angularZ);

/////////////////////////////////// Hardware Declarations ///////////////////////////////////

// define bluetooth serial connection
BluetoothSerial serialBT;
String robotName = "roboeaglets";

// define motors
NoU_Motor frontLeftMotor(3);
NoU_Motor frontRightMotor(2);
NoU_Motor backLeftMotor(4);
NoU_Motor backRightMotor(1);

// define drivetrain
mecanumDrivetrain drivetrain = mecanumDrivetrain(&frontLeftMotor, &frontRightMotor, &backLeftMotor, &backRightMotor);

// define encoders
ESP32Encoder frontLeftEncoder; // pins 34, 35
ESP32Encoder frontRightEncoder; // pins 36, 39

// define servos
NoU_Servo intakeServo(1);
NoU_Servo fourBarServo(3);
NoU_Servo secondJointServo(4);

// define arm
Arm arm = Arm(&fourBarServo, &secondJointServo, &intakeServo);

// define imu object
IMU imu;

/////////////////////////////////// Logic Declarations ///////////////////////////////////

// enable logic and debounce
bool enabled = false;
unsigned long lastEnableRead = 0;

bool firstArm = false;
bool runAuton = false;

// intake logic and debounce
bool intakeClosed = false;
unsigned long lastIntakeRead = 0;
bool firstIntake = false;

// current arm preset
char armPreset = '0';

uint8_t imuStarted;
////////////////////////////////////////////////////////////////////// setup() //////////////////////////////////////////////////////////////////////
void setup() {
  // begin DS comms
  serialBT.begin(robotName);
  AlfredoConnect.begin(serialBT, true); // providing true means we won't get annoying errors regarding lack of joystick data

  // start imu
  imuStarted = imu.begin(5, 4);

  

  // start arm
  arm.begin();

  // start RSL
  RSL::initialize();

  // set direction of motors
  frontLeftMotor.setInverted(false);
  frontRightMotor.setInverted(false);
  backLeftMotor.setInverted(true);
  backRightMotor.setInverted(true);

  // init encoders
  frontLeftEncoder.attachHalfQuad(34, 35);
  frontRightEncoder.attachHalfQuad(36, 39);
  frontLeftEncoder.setCount(0);
  frontRightEncoder.setCount(0);
}
////////////////////////////////////////////////////////////////////// loop() //////////////////////////////////////////////////////////////////////
void loop() {

  // parse updates from IMU
  uint8_t imuRead = imu.read();
  if(imuRead > 0){
    serialBT.println("ERROR. IMU FAILED TO READ.");
    serialBT.println("Error code: " + String(imuRead));
  }

  if(imuStarted > 0){
    serialBT.println("ERROR! IMU FAILED TO START");
    serialBT.println("Error code: " + String(imuStarted));
  }

  // parse updates from driver station
  AlfredoConnect.update();

  // enable logic
  if(AlfredoConnect.buttonHeld(0, 9) && millis() - lastEnableRead > 300){
    lastEnableRead = millis();
    enabled = !enabled;
  }

  // rsl logic
  if(enabled){
    RSL::setState(RSL_ENABLED);
  }
  else{
    RSL::setState(RSL_DISABLED);
  }

  ///////////////////////////////////// get values from controller for drivetrain
  double linearX = -AlfredoConnect.getAxis(0, 2); // angularZ
  double linearY = AlfredoConnect.getAxis(0, 3); // linearX
  double angularZ = AlfredoConnect.getAxis(0, 0); // linearY

  // apply deadzone
  linearX = deadzone(linearX);
  linearY = deadzone(linearY);
  angularZ = deadzone(angularZ);
  
  // apply straightening
  if(fabs(linearX) > 0.95 && fabs(linearY) < 0.4){
    linearY = 0.0;
  }
  if(fabs(linearY) > 0.95 && fabs(linearX) < 0.4){
    linearX = 0.0;
  }

  ///////////////////////////////////// get arm preset positions from controller
  if(AlfredoConnect.buttonHeld(0, 15)){ // dpad right
    armPreset = 'H'; // high node
    firstArm = true;
  }
  if(AlfredoConnect.buttonHeld(0, 13)){ // dpad down
    armPreset = 'D'; // double substation
    firstArm = true;
  }
  if(AlfredoConnect.buttonHeld(0, 12)) { // dpad up
    armPreset = 'M'; // mid node
    firstArm = true;
  }
  if(AlfredoConnect.buttonHeld(0, 14)) { // dpad left
    armPreset = 'L'; // low node
    firstArm = true;
  }
  if(AlfredoConnect.buttonHeld(0, 7)) { // right trigger
    armPreset = '0'; // stow
    firstArm = true;
  }
  if(AlfredoConnect.buttonHeld(0, 6)) { // left trigger
    armPreset = 'F'; // floor
    firstArm = true;
  }

  ///////////////////////////////////// intake
  if(AlfredoConnect.buttonHeld(0, 0) && millis() - lastIntakeRead > 250){ // x
    lastIntakeRead = millis();
    intakeClosed = !intakeClosed;
    firstIntake = true;
  }

  ///////////////////////////////////// only write to hardware if enabled
  if(enabled) {
    //teleop
    serialBT.println("Pitch: " + String(imu.getPitch()) + " Roll: " + String(imu.getRoll()) + " Yaw: " + String(imu.getYaw()));
    drivetrain.set(linearX, linearY, angularZ);

    // arm
    if (firstArm) {
      arm.set(armPreset);
      firstArm = false;
    }

    // intake
    if(firstIntake){
      firstIntake = false;
      arm.setIntake(intakeClosed);
    }
  }
  /////////////////////////////////// DISABLED
  else {
    // drivetrain e-stop if disabled
    frontLeftMotor.set(0.0);
    frontRightMotor.set(0.0);
    backLeftMotor.set(0.0);
    backRightMotor.set(0.0);
  }
}
////////////////////////////////////////////////////////////////////// Function Code //////////////////////////////////////////////////////////////////////

// joystick deadzone
double deadzone(double rawJoy){
  if(fabs(rawJoy) < deadzoneVal){
    return 0.0;
  }
  return rawJoy;
}

// autons
void driveInches(double inches, double linearX, double linearY, double angularZ) {
  double inperrev = 5.93689;
  double requiredrot = inches / inperrev;
  
  // 40 ticks per rev
  double leftRot = frontLeftEncoder.getCount();
  double rightRot = frontRightEncoder.getCount();

  double leftTarget = leftRot + requiredrot;
  double rightTarget = rightRot + requiredrot;

  while (rightRot != rightTarget || leftRot != leftTarget) {
    drivetrain.set(linearX, linearY, angularZ);
  }
  drivetrain.set(0, 0, 0);
}

void taxiAuton() {
  driveInches(35.0, 0.0, 0.6, 0.0);
}