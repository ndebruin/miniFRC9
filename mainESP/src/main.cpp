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

// LED defs


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
NoU_Servo intakeServo(2);
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


////////////////////////////////////////////////////////////////////// setup() //////////////////////////////////////////////////////////////////////
void setup() {
  // begin DS comms
  serialBT.begin(robotName);
  AlfredoConnect.begin(serialBT);

  // start I2C
  Wire1.begin(5, 4);
  Wire1.setClock(400000);

  // start imu
  imu.begin(Wire1);

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
  frontLeftEncoder.attachHalfQuad(36, 39);
  frontRightEncoder.attachHalfQuad(34, 35);
  frontLeftEncoder.setCount(0);
  frontRightEncoder.setCount(0);

  // limit intake servo range
  intakeServo.setMaximumPulse(2000);
  intakeServo.setMinimumPulse(1000);

}
////////////////////////////////////////////////////////////////////// loop() //////////////////////////////////////////////////////////////////////
void loop() {

  // parse updates from IMU
  imu.read();

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
  double linearX = -AlfredoConnect.getAxis(0, 0); // angularZ
  double linearY = AlfredoConnect.getAxis(0, 1); // linearX
  double angularZ = AlfredoConnect.getAxis(0, 2); // linearY

  // apply deadzone
  linearX = deadzone(linearX);
  linearY = deadzone(linearY);
  angularZ = deadzone(angularZ);
  
  // apply straightening
  if(fabs(linearX) > 0.95 && fabs(linearY) < 0.3){
    linearY = 0.0;
  }
  if(fabs(linearY) > 0.95 && fabs(linearX) < 0.3){
    linearX = 0.0;
  }

  ///////////////////////////////////// get arm preset positions from controller
  if(AlfredoConnect.buttonHeld(0, 7)){ // right bumper
    armPreset = 'H'; // high node
    firstArm = true;
  }
  if(AlfredoConnect.buttonHeld(0, 6)){ // left bumper
    armPreset = 'D'; // double substation
    firstArm = true;
  }
  if(AlfredoConnect.buttonHeld(0, 12)) { // dpad up
    armPreset = 'M'; // mid node
    firstArm = true;
  }
  if(AlfredoConnect.buttonHeld(0, 15)) { // dpad right
    armPreset = 'L'; // low node
    firstArm = true;
  }
  if(AlfredoConnect.buttonHeld(0, 13)) { // dpad down
    armPreset = '0'; // stow
    firstArm = true;
  }
  if(AlfredoConnect.buttonHeld(0, 14)) { // dpad left
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
    //serialBT.println("FrontLeft: " + String(frontLeftEncoder.getCount() /2 ) + " FrontRight: " + String(frontRightEncoder.getCount() /2 ));
    //teleop
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
      serialBT.println(String(intakeClosed));
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
/*void driveInches(double inches, double linearX, double linearY, double angularZ) {
  double inperrev = 5.93689;
  double requiredrot = inches / inperrev;
  
  // div by 2 is bc of the counting method, dw abt it.
  double leftRot = frontLeftEncoder.getCount() /2;
  double rightRot = frontRightEncoder.getCount() /2;

  double leftTarget = leftRot + requiredrot;
  double rightTarget = rightRot + requiredrot;

  while (rightRot != rightTarget || leftRot != leftTarget) {
    drivetrain.set(linearX, linearY, angularZ);
  }
  drivetrain.set(0, 0, 0);
}

void taxiAuton() {
  driveInches(35.0, 0.0, 0.6, 0.0);
}*/