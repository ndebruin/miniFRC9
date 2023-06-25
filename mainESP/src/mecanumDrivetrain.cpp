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

/*void mecanumDrivetrain::set(double linearX, double linearY, double angularZ, bool fieldOriented, double yaw) {
  double botHeading = yaw;
  double rotX = linearX * cos(-botHeading) - linearY * sin(-botHeading);
  double rotY = linearX * sin(-botHeading) + linearY * cos(-botHeading);

  double denominator = max(abs(rotY) + abs(rotX) + abs(angularZ), 1.0);
  frontLeftPower = ((rotY + rotX + angularZ) / denominator) * 255.0;
  backLeftPower = ((rotY - rotX + angularZ) / denominator) * 255.0;
  frontRightPower = ((rotY - rotX - angularZ) / denominator) * 255.0;
  backRightPower = ((rotY + rotX - angularZ) / denominator) * 255.0;

  _frontLeftMotor->set(frontLeftPower);
  _frontRightMotor->set(frontRightPower);
  _backLeftMotor->set(backLeftPower);
  _backRightMotor->set(backRightPower);
}*/

double mecanumDrivetrain::set(double linearX, double linearY, double angularZ) {

  
  frontLeftDesiredPower = ((linearY + linearX + angularZ));
  backLeftDesiredPower = ((linearY - linearX + angularZ));
  frontRightDesiredPower = ((linearY - linearX - angularZ));
  backRightDesiredPower = ((linearY + linearX - angularZ));

  // ensure motor output is never greater than -1/1
  double maxMagnitude = max(fabs(frontLeftDesiredPower), max(fabs(frontRightDesiredPower), max(fabs(backLeftDesiredPower), fabs(backRightDesiredPower))));
  if(maxMagnitude > 1.0){
    frontLeftDesiredPower /= maxMagnitude;
    frontRightDesiredPower /= maxMagnitude;
    backLeftDesiredPower /= maxMagnitude;
    backRightDesiredPower /= maxMagnitude;    
  }

  // apply jerk limits
  /*frontLeftCurrentPower = applyJerkLimit(frontLeftCurrentPower, frontLeftDesiredPower);
  frontRightCurrentPower = applyJerkLimit(frontRightCurrentPower, frontRightDesiredPower);
  backLeftCurrentPower = applyJerkLimit(backLeftCurrentPower, backLeftDesiredPower);
  backRightCurrentPower = applyJerkLimit(backRightCurrentPower, backRightDesiredPower);*/

  frontLeftCurrentPower = frontLeftDesiredPower;
  frontRightCurrentPower = frontRightDesiredPower;
  backLeftCurrentPower = backLeftDesiredPower;
  backRightCurrentPower = backRightDesiredPower;


  _frontLeftMotor->set(frontLeftCurrentPower);
  _frontRightMotor->set(frontRightCurrentPower);
  _backLeftMotor->set(backLeftCurrentPower);
  _backRightMotor->set(backRightCurrentPower);

  //return denominator;
  // debug
  return frontLeftCurrentPower * 255.0;
}

double mecanumDrivetrain::toRadians(double degrees) {
  return degrees * (PI / 180.0);
}

double mecanumDrivetrain::applyJerkLimit(double current, double desired){
  if(jerkLimit < abs(current - desired)){
    limiting = true;
    if(current - desired < 0){
      return current + jerkLimit;
    }
    else{
      return current - jerkLimit;
    }
  }
  else{
    limiting = false;
    return desired;
  }
}