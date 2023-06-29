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

void mecanumDrivetrain::set(double linearX, double linearY, double angularZ, double yaw) {

  linearY *= -1.0;

  double joystickAngle = 0.0;

  // convert linearX/linearY to polar
  if(linearX != 0){
    if(linearX >= 0 && linearY <= 0){ // Q1
      joystickAngle = atan(linearY / linearX) * -180.0 / PI;
    }
    else if (linearX <= 0 && linearY <= 0) { //Q2
      joystickAngle = atan(linearY / linearX) * -180 / PI + 180;
    }
    else if (linearX <= 0 && linearY >= 0) { //Q3
      joystickAngle = atan(linearY / linearX) * -180 / PI + 180;
    }
    else { //Q4
      joystickAngle = atan(linearY / linearX) * -180 / PI + 360;
    }
  }
  else {
    if (linearY >= 0) {
      joystickAngle = 270;
    }
    else {
      joystickAngle = 90;
    }
  }

  double skew = joystickAngle - yaw; //determine difference between robot angle and joystick angle
  double joystickMag = sqrt(pow(linearX, 2) + pow(linearY, 2)); //find magnitude of joystick values

  double linearXPrime = joystickMag * cos(skew * PI / 180.0);
  double linearYPrime = joystickMag * sin(skew * PI / 180.0);

  set(linearXPrime, linearYPrime, angularZ);
}

void mecanumDrivetrain::set(double linearX, double linearY, double angularZ) {
  double linX = linearX;
  double linY = linearY;
  double angZ = angularZ;

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