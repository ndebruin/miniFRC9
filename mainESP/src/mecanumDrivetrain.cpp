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

void mecanumDrivetrain::set(double linearX, double linearY, double angularZ, bool fieldOriented, double yaw) {
  double botHeading = yaw;
  double rotX = linearX * cos(-botHeading) - linearY * sin(-botHeading)
  double rotY = linearX * sin(-botHeading) + linearY * cos(-botHeading)

  denominator = max(abs(rotY) + abs(rotX) + abs(angularZ), 1)
  frontLeftPower = ((rotY + rotX + angularZ) / denominator) * 255
  backLeftPower = ((rotY - rotX + angularZ) / denominator) * 255
  frontRightPower = ((rotY - rotX - angularZ) / denominator) * 255
  backRightPower = ((rotY + rotX - angularZ) / denominator) * 255

  _frontLeftMotor->set(frontLeftPower);
  _frontRightMotor->set(frontRightPower);
  _backLeftMotor->set(backLeftPower);
  _backRightMotor->set(backRightPower);
}

void mecanumDrivetrain:set(double linearX, double linearY, double angularZ) {
  denominator = max(abs(linearY) + abs(linearX) + abs(angularZ), 1)
  frontLeftPower = ((linearY + linearX + angularZ) / denominator) * 255
  backLeftPower = ((linearY - linearX + angularZ) / denominator) * 255
  frontRightPower = ((linearY - linearX - angularZ) / denominator) * 255
  backRightPower = ((linearY + linearX - angularZ) / denominator) * 255

  _frontLeftMotor->set(frontLeftPower);
  _frontRightMotor->set(frontRightPower);
  _backLeftMotor->set(backLeftPower);
  _backRightMotor->set(backRightPower);
}

double mecanumDrivetrain::toRadians(double degrees) {
  return degrees * (PI / 180);
}