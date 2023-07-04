#include "Arm.h"

Arm::Arm(NoU_Servo* fourbarJoint, NoU_Servo* secondJoint, NoU_Motor* intakeMotor) 
                                    : _fourBarJoint(fourbarJoint), _secondJoint(secondJoint), _intakeMotor(intakeMotor)
{ }

void Arm::begin(){
  set('0');
  _intakeMotor->set(0);
}

void Arm::setDelay(int fourBarAngle, int secondAngle){
    _fourBarJoint->write(180 - fourBarAngle);
    delay(250);
    _secondJoint->write(180 - secondAngle);
}

void Arm::set(int fourBarAngle, int secondAngle) {
    _fourBarJoint->write(180 - fourBarAngle);
    _secondJoint->write(180 - secondAngle);
}

void Arm::set(char preset) {
    switch (preset){
        case 'L':
            set(_angleFourBarLow, _angleSecondLow);
            break;
        case 'M':
            setDelay(_angleFourBarMid, _angleSecondMid);
            break;
        case 'H':
            setDelay(_angleFourBarHigh, _angleSecondHigh);
            break;
        case 'F':
            set(_angleFourBarFloor, _angleSecondFloor);
            break;
        case 'D':
            setDelay(_angleFourBarDouble, _angleSecondDouble);
            break;
        case 'S':
            set(_angleFourBarSingle, _angleSecondSingle);
            break;
        default:
            set(_angleFourBarStow, _angleSecondStow);
            break;
    }
}

void Arm::setIntake(double on) {
    _intakeMotor->set(on);
}

void Arm::setHighAuton() {
    set(_angleFourBarHigh, 0);
    delay(300);
    set(_angleFourBarHigh, _angleSecondHigh);
}