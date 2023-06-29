#include "Arm.h"

Arm::Arm(NoU_Servo* fourbarJoint, NoU_Servo* secondJoint, NoU_Motor* intakeMotor) 
                                    : _fourBarJoint(fourbarJoint), _secondJoint(secondJoint), _intakeMotor(intakeMotor)
{ }

void Arm::begin(){
  _fourBarJoint->write(0);
  _secondJoint->write(0);
  _intakeMotor->set(0);
}

void Arm::set(int fourBarAngle, int secondAngle){
    _fourBarJoint->write(fourBarAngle);
    _secondJoint->write(secondAngle);
}

void Arm::set(char preset) {
    switch (preset){
        case 'L':
            set(_angleFourBarLow, _angleSecondLow);
            break;
        case 'M':
            set(_angleFourBarMid, _angleSecondMid);
            break;
        case 'H':
            set(_angleFourBarHigh, _angleSecondHigh);
            break;
        case 'F':
            set(_angleFourBarFloor, _angleSecondFloor);
            break;
        case 'D':
            set(_angleFourBarDouble, _angleSecondDouble);
            break;
        case 'S':
            set(_angleFourBarSingle, _angleSecondSingle);
            break;
        default:
            set(_angleFourBarStow, _angleSecondStow);
            break;
    }
}

void Arm::setIntake(bool on) {
    if(on){
        _intakeMotor->set(1);
    } else {
        _intakeMotor->set(0);
    }
}