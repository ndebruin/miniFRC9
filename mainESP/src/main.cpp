#include <Arduino.h>
#include <BluetoothSerial.h>
#include <AlfredoConnect.h>
#include <Alfredo_NoU2.h>
#include <FastLED.h>
#include <Encoder.h>

#include "IMU.h"
#include "mecanumDrivetrain.h"
#include "Arm.h"
// #include "EncoderComms.h"
// #include "CameraComms.h"

// function def
double deadzone(double rawJoy);
double deadzoneVal = 0.05;

// LED defs
#define NUM_LEDS 16
#define DATA_PIN 2

CRGB leds[NUM_LEDS];

// define bluetooth serial connection
BluetoothSerial serialBT;
String robotName = "roboeaglets";

// define motors
NoU_Motor frontLeftMotor(1);
NoU_Motor frontRightMotor(2);
NoU_Motor backLeftMotor(3);
NoU_Motor backRightMotor(4);

// define drivetrain
mecanumDrivetrain drivetrain = mecanumDrivetrain(&frontLeftMotor, &frontRightMotor, &backLeftMotor, &backRightMotor);

// define encoders
Encoder frontLeftEncoder(1, 3);
Encoder frontRightEncoder(21, 22);

// define servos
NoU_Servo intakeServo(1);
NoU_Servo fourBarServo(3);
NoU_Servo secondJointServo(4);

// define arm
Arm arm = Arm(&fourBarServo, &secondJointServo);

// define imu object
IMU imu;

// enable logic and debounce
bool enabled = false;
unsigned long lastEnableRead = millis();

// current arm preset
char armPreset = '0';

void setup() {
  // begin DS comms
  serialBT.begin(robotName);
  AlfredoConnect.begin(serialBT);

  // start RSL
  RSL::initialize();

  // start I2C
  Wire1.begin(5, 4);
  Wire1.setClock(400000);

  // start imu
  imu.begin(Wire1);

  // start arm
  arm.begin();

  // set direction of motors
  frontLeftMotor.setInverted(false);
  frontRightMotor.setInverted(true);
  backLeftMotor.setInverted(true);
  backRightMotor.setInverted(false);

  // start LEDS
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  // reset leds to black
  FastLED.clear(true);
  // limit brightness
  FastLED.setBrightness(100);

}

void loop() {
  fill_solid(leds, NUM_LEDS, CRGB::Green);

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
  RSL::update();


  // get values from controller for drivetrain
  double angularZ = -AlfredoConnect.getAxis(0, 0);
  double linearX = AlfredoConnect.getAxis(0, 1);
  double linearY = -AlfredoConnect.getAxis(0, 2);

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

  // get arm preset positions from controller
  if(AlfredoConnect.buttonHeld(0, 7)){ // right bumper
    armPreset = 'H'; // high node
  }
  if(AlfredoConnect.buttonHeld(0, 6)){ // left bumper
    armPreset = 'D'; // double substation
  }
  if(AlfredoConnect.buttonHeld(0, 12)) { // dpad up
    armPreset = 'M'; // mid node
  }
  if(AlfredoConnect.buttonHeld(0, 15)) { // dpad right
    armPreset = 'L'; // low node
  }
  if(AlfredoConnect.buttonHeld(0, 13)) { // dpad down
    armPreset = '0'; // stow
  }
  if(AlfredoConnect.buttonHeld(0, 14)) { // dpad left
    armPreset = 'F'; // floor
  }

  // only write to hardware if enabled
  if(enabled){    
    drivetrain.set(linearX, linearY, angularZ);
    arm.set(armPreset);
  }
  else {
    // drivetrain e-stop if disabled
    frontLeftMotor.set(0.0);
    frontRightMotor.set(0.0);
    backLeftMotor.set(0.0);
    backRightMotor.set(0.0);
  }
}


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

  double frontLeftRot = frontLeftEncoder.read();
  double frontRightRot = frontRightEncoder.read();

  double frontLeftTarget = frontLeftRot + requiredrot;
  double frontRightTarget = frontRightRot + requiredrot;

  while (frontLeftRot != frontLeftTarget || frontRightRot != frontRightTarget) {
    drivetrain.set(linearX, linearY, angularZ);
  }
  drivetrain.set(0, 0, 0);
}

void taxiAuton() {
  driveInches(35.0, 0.0, 0.6, 0.0);
}