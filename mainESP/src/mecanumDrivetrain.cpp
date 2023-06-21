#include <Arduino.h>
#include <Alfredo_NoU2.h>
#include "mecanumDrivetrain.h"

mecanumDrivetrain::mecanumDrivetrain(NoU_Motor* frontLeftMotor, NoU_Motor* frontRightMotor, NoU_Motor* backLeftMotor, NoU_Motor* backRightMotor) 
                                    : _frontLeftMotor(frontLeftMotor), _frontRightMotor(frontRightMotor), _backLeftMotor(backLeftMotor), _backRightMotor(backRightMotor)
{ }

void mecanumDrivetrain::begin(){
  _frontLeftMotor->set(0);
  _frontRightMotor->set(0);
  _backLeftMotor->set(0);
  _backRightMotor->set(0);
}

void mecanumDrivetrain::set(double linearX, double linearY, double angularZ){
  // perform kinematic equations
  float frontLeft = (linearX - linearY - angularZ);
  float frontRight = (linearX + linearY + angularZ);
  float backLeft = (linearX + linearY - angularZ);
  float backRight = (linearX - linearY + angularZ);

  // write to each motor
  _frontLeftMotor->set(frontLeft);
  _frontRightMotor->set(frontRight);
  _backLeftMotor->set(backLeft);
  _backRightMotor->set(backRight);
}