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
  double linX = linearX;
  double linY = linearY;
  double angZ = angularZ;
  if (this->flipped) {
    linX = angularZ;
    linY = linearX;
    angZ = linearY;
  }

  double botHeading = yaw;
  double rotX = linX * cos(-botHeading) - linY * sin(-botHeading);
  double rotY = linX * sin(-botHeading) + linY * cos(-botHeading);

  double maxMagnitude = max(fabs(frontLeftDesiredPower), max(fabs(frontRightDesiredPower), max(fabs(backLeftDesiredPower), fabs(backRightDesiredPower))));
  if(maxMagnitude > 1.0){
    frontLeftDesiredPower /= maxMagnitude;
    frontRightDesiredPower /= maxMagnitude;
    backLeftDesiredPower /= maxMagnitude;
    backRightDesiredPower /= maxMagnitude;    
  }

  frontLeftCurrentPower = applyJerkLimit(frontLeftCurrentPower, frontLeftDesiredPower);
  frontRightCurrentPower = applyJerkLimit(frontRightCurrentPower, frontRightDesiredPower);
  backLeftCurrentPower = applyJerkLimit(backLeftCurrentPower, backLeftDesiredPower);
  backRightCurrentPower = applyJerkLimit(backRightCurrentPower, backRightDesiredPower);

  _frontLeftMotor->set(frontLeftCurrentPower);
  _frontRightMotor->set(frontRightCurrentPower);
  _backLeftMotor->set(backLeftCurrentPower);
  _backRightMotor->set(backRightCurrentPower);
}

void mecanumDrivetrain::set(double linearX, double linearY, double angularZ) {
  double linX = linearX;
  double linY = linearY;
  double angZ = angularZ;
  if (this->flipped) {
    linX = angularZ;
    linY = linearX;
    angZ = linearY;
  }

  frontLeftDesiredPower = ((linX - linY - angZ));
  backLeftDesiredPower = ((linX + linY + angZ));
  frontRightDesiredPower = ((linX + linY - angZ));
  backRightDesiredPower = ((linX - linY + angZ));

  // ensure motor output is never greater than -1/1
  double maxMagnitude = max(fabs(frontLeftDesiredPower), max(fabs(frontRightDesiredPower), max(fabs(backLeftDesiredPower), fabs(backRightDesiredPower))));
  if(maxMagnitude > 1.0){
    frontLeftDesiredPower /= maxMagnitude;
    frontRightDesiredPower /= maxMagnitude;
    backLeftDesiredPower /= maxMagnitude;
    backRightDesiredPower /= maxMagnitude;    
  }

  // apply jerk limits
  frontLeftCurrentPower = applyJerkLimit(frontLeftCurrentPower, frontLeftDesiredPower);
  frontRightCurrentPower = applyJerkLimit(frontRightCurrentPower, frontRightDesiredPower);
  backLeftCurrentPower = applyJerkLimit(backLeftCurrentPower, backLeftDesiredPower);
  backRightCurrentPower = applyJerkLimit(backRightCurrentPower, backRightDesiredPower);

  _frontLeftMotor->set(frontLeftCurrentPower);
  _frontRightMotor->set(frontRightCurrentPower);
  _backLeftMotor->set(backLeftCurrentPower);
  _backRightMotor->set(backRightCurrentPower);
}

double mecanumDrivetrain::toRadians(double degrees) {
  return degrees * (PI / 180.0);
}

double mecanumDrivetrain::applyJerkLimit(double current, double desired){
  if(jerkLimit < abs(current - desired)){
    if(current - desired < 0){
      return current + jerkLimit;
    }
    else{
      return current - jerkLimit;
    }
  }
  else{
    return desired;
  }
}

void mecanumDrivetrain::setFlipped(bool _flipped){
  flipped = _flipped;
}