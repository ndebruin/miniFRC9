#include <Arduino.h>
#include <BluetoothSerial.h>
#include <AlfredoConnect.h>
#include <Alfredo_NoU2.h>
#include <ESP32Encoder.h>

#include "IMU.h"
#include "mecanumDrivetrain.h"
#include "Arm.h"

/////////////////////////////////// Function Declarations ///////////////////////////////////

// function def
double deadzone(double rawJoy);
double deadzoneVal = 0.05;
void updateYaw();
void autonCharge();
void autonFar();
void autonClose();
void twoPieceAuton();
void turnAuton();
void placeHighAuton();
void driveInches(double inches, double linearX, double linearY, double angularZ);
void taxiAuton();
void chargeAuton();

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
NoU_Motor intakeMotor(5);
NoU_Servo fourBarServo(3);
NoU_Servo secondJointServo(4);

// define arm
Arm arm = Arm(&fourBarServo, &secondJointServo, &intakeMotor);

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

bool fieldOriented = false;
unsigned long lastOrientedRead = 0;

double yawOffset = 0.0;
double robotHeading = 0.0;

unsigned long lastRSLFlash = 0;
bool RSLState = false;

bool autonStarted = true;

String selectedAuton = "charge";

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
  pinMode(LED_BUILTIN, OUTPUT);

  // set direction of motors
  frontLeftMotor.setInverted(false);
  frontRightMotor.setInverted(false);
  backLeftMotor.setInverted(true);
  backRightMotor.setInverted(true);

  intakeMotor.setInverted(false);

  // init encoders
  frontLeftEncoder.attachHalfQuad(34, 35);
  frontRightEncoder.attachHalfQuad(36, 39);
  frontLeftEncoder.setCount(0);
  frontRightEncoder.setCount(0);
}
////////////////////////////////////////////////////////////////////// loop() //////////////////////////////////////////////////////////////////////
void loop() {

  // parse updates from IMU
  int8_t imuRead = imu.read();
  if(imuRead > 0){
    serialBT.println("ERROR. IMU FAILED TO READ.");
    serialBT.println("Error code: " + String(imuRead));
  }

  if(imuStarted > 0){
    serialBT.println("ERROR! IMU FAILED TO START");
    serialBT.println("Error code: " + String(imuStarted));
  }

  // manage yaw data
  updateYaw();

  // check for disconnect
  if(!AlfredoConnect.getGamepadCount() >= 1){
    enabled = false;
  }

  // parse updates from driver station
  AlfredoConnect.update();

  // enable logic
  if(AlfredoConnect.buttonHeld(0, 9) && millis() - lastEnableRead > 500){
    lastEnableRead = millis();
    enabled = !enabled;
  }

  // rsl logic
  if(enabled){
    if(millis() - lastRSLFlash > 500){
      lastRSLFlash = millis();
      digitalWrite(LED_BUILTIN, RSLState);
      RSLState = !RSLState;
    }
  }
  else{
    digitalWrite(LED_BUILTIN, HIGH);
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
  if(fabs(linearX) > 0.95 && fabs(linearY) < 0.4){
    linearY = 0.0;
  }
  if(fabs(linearY) > 0.95 && fabs(linearX) < 0.4){
    linearX = 0.0;
  }

  ///////////////////////////////////// get arm preset positions from controller
  if(AlfredoConnect.buttonHeld(0, 1)){ // circle
    armPreset = 'M'; // mid node
    firstArm = true;
  }
  if(AlfredoConnect.buttonHeld(0, 2)){ // square
    armPreset = 'D'; // double substation
    firstArm = true;
  }
  if(AlfredoConnect.buttonHeld(0, 3)) { // triangle
    armPreset = 'H'; // high node
    firstArm = true;
  }
  if(AlfredoConnect.buttonHeld(0, 0)) { // x
    armPreset = 'F'; // floor
    firstArm = true;
  }
  if(AlfredoConnect.buttonHeld(0, 6)) { // left trigger
    armPreset = '0'; // stow
    firstArm = true;
  }
  if(AlfredoConnect.buttonHeld(0, 7)) { // right trigger
    armPreset = 'S'; // single substation
    firstArm = true;
  }

  ///////////////////////////////////// field oriented
  if(AlfredoConnect.buttonHeld(0, 12) && millis() - lastOrientedRead > 300){ // dpad up
    lastOrientedRead = millis();
    fieldOriented = !fieldOriented;
  }

  if (autonStarted) {
    if(AlfredoConnect.keyHeld(Key::W)){
      placeHighAuton();
      chargeAuton();
      autonStarted = false;
    }

    if(AlfredoConnect.keyHeld(Key::E)){
      autonClose();
      autonStarted = false;
    }
  }

  ///////////////////////////////////// field oriented reCal

  ///////////////////////////////////// only write to hardware if enabled
  if(enabled) {

    if(AlfredoConnect.buttonHeld(0, 8)){
      yawOffset = -imu.getYaw();
    }

    //teleop
    serialBT.println("leftRot: " + String(frontLeftEncoder.getCount()) + "rightRot: " + String(frontRightEncoder.getCount()));
    
    // drivetrain
    if(fieldOriented){
      drivetrain.set(linearX, linearY, angularZ, robotHeading);
    }
    else if (AlfredoConnect.buttonHeld(0, 14)) {
      drivetrain.set(linearX, linearY, -0.4);
    }
    else if (AlfredoConnect.buttonHeld(0, 15)) {
      drivetrain.set(linearX, linearY, 0.4);
    }
    else{
      drivetrain.set(linearX, linearY, angularZ);
    }

    // arm
    if (firstArm) {
      arm.set(armPreset);
      firstArm = false;
    }

    if (AlfredoConnect.buttonHeld(0, 5)) {
      arm.setIntake(-1);
    }
    else if (AlfredoConnect.buttonHeld(0, 4)) {
      arm.setIntake(1);
    }
    else {
      arm.setIntake(0);
    }
  }
  /////////////////////////////////// DISABLED
  else {
    // drivetrain e-stop if disabled
    frontLeftMotor.set(0.0);
    frontRightMotor.set(0.0);
    backLeftMotor.set(0.0);
    backRightMotor.set(0.0);
    arm.setIntake(0);
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


void resetEncoders() {
  frontLeftEncoder.setCount(0);
  frontRightEncoder.setCount(0);
}

void updateYaw(){
  // manage yaw data
  if(imu.getYaw() < 0){
    robotHeading = imu.getYaw() + 360;
  }
  else{
    robotHeading = imu.getYaw();
  }

  // add reCal offset
  robotHeading += yawOffset;

  // wrap rotation
  if (robotHeading > 360) {
    robotHeading -= 360;
  }
  else if (robotHeading < 0) {
    robotHeading += 360;
  }
}

// autons
void driveInches(double inches, double linearX, double linearY, double angularZ) {
  resetEncoders();
  double revsperin = 1.0/5.93689;
  double requiredrot = revsperin * inches;
  
  // 40 ticks per rev
  double leftRot = fabs(frontLeftEncoder.getCount() / 40.0);
  double rightRot = fabs(frontRightEncoder.getCount() / 40.0);

  rightRot = fabs(frontRightEncoder.getCount() / 40.0); 

  double leftTarget = leftRot + requiredrot;
  double rightTarget = rightRot + requiredrot;

  serialBT.println("leftRot: " + String(leftRot) + "rightRot: " + String(rightRot));
  serialBT.println("leftTarget: " + String(leftTarget) + "rightTarget: " + String(rightTarget));

  while (rightRot < rightTarget || leftRot < leftTarget) {
    leftRot = fabs(frontLeftEncoder.getCount() / 40.0);
    rightRot = fabs(frontRightEncoder.getCount() / 40.0); 
    serialBT.println("leftRot: " + String(leftRot) + "rightRot: " + String(rightRot));
    serialBT.println("leftTarget: " + String(leftTarget) + "rightTarget: " + String(rightTarget));
    drivetrain.set(linearX, linearY, angularZ);
  }
  drivetrain.set(0, 0, 0);
}

// actual autons
void autonCharge(){
  placeHighAuton();
  chargeAuton();
}

void autonClose(){
  placeHighAuton();
  taxiAuton();
}

void autonFar(){
  placeHighAuton();
  turnAuton();
  taxiAuton();
}

void floorPickAuton(){
  arm.set('F');
  delay(50);
  arm.setIntake(-1);
  drivetrain.set(0.0, 0.6, 0.0);
  delay(100);
  drivetrain.set(0.0, 0.0, 0.0);
  arm.set('0');
}

void turnAuton() {
  drivetrain.set(0.0, 0.0, 0.0);
  imu.read();
  double initialYaw = imu.getYaw();
  double heading = imu.getYaw();
  serialBT.println("Initial Yaw: " + String(initialYaw) + " RobotHeading: " + String(heading));
  while(fabs(heading-initialYaw) < 150){
    imu.read();
    heading = imu.getYaw();
    serialBT.println("Raw: " + String(imu.getYaw()) + " RobotHeading: " + String(heading) + " " + String(fabs(heading - initialYaw)));
    drivetrain.set(0.0, 0.0, -0.8);
    delay(1);
  }

  imu.read();
  // slow down close to it
  while(fabs(heading-initialYaw) < 180){
    imu.read();
    heading = imu.getYaw();
    serialBT.println("Raw: " + String(imu.getYaw()) + " RobotHeading: " + String(heading) + " " + String(fabs(heading - initialYaw)));
    drivetrain.set(0.0, 0.0, -0.7);
    delay(1);
  }
  drivetrain.set(0.0, 0.0, 0.0);
}

void placeHighAuton(){
  arm.set('H'); // set arm to high
  delay(1000); // wait for it to get there
  arm.setIntake(1); // output piece
  delay(1000); // wait for piece to evacuate
  arm.set('0'); // stow arm
  delay(200);
  arm.setIntake(0); // stop intake
}

void twoPieceAuton(){
  placeHighAuton();
  drivetrain.set(0.0, 0.8, 0.0);
  delay(250);
  drivetrain.set(0.0, 0.0, 0.0);
  driveInches(11.0, 0.0, 0.0, 0.8);
  delay(250);
  driveInches(20.0, 0.0, -1.0, 0.0); // drive to second piece
  arm.set('F'); // set arm to floor
  delay(1000); // wait for it to get there
  arm.setIntake(-1); // intake piece
  driveInches(3.0, 0.0, -0.8, 0.0); // drive forward to intake
  delay(1000); // wait for piece to intake
  arm.set('0'); // stow arm
  delay(200);
  arm.setIntake(0); // stop intake
}


void taxiAuton() {
  driveInches(32.0, 0.0, 0.8, 0.0);
}

void chargeAuton() {
  imu.read();
  // drive until we run into charge station
  while(fabs(imu.getPitch()) < 10){
    imu.read();
    serialBT.println("Pitch: " + String(imu.getPitch()));
    drivetrain.set(0.0, 1.0, 0.0);
  }

  imu.read();
  // drive until we are on top
  while(fabs(imu.getPitch()) > 10){
    imu.read();
    serialBT.println("Pitch: " + String(imu.getPitch()));
    drivetrain.set(0.0, 1.0, 0.0);
  }
  drivetrain.set(0.0, 0.0, 0.0);
}