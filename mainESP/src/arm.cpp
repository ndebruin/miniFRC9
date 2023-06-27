#include "Arm.h"

Arm::Arm(NoU_Servo* fourbarJoint, NoU_Servo* secondJoint, NoU_Servo* intakeServo) 
                                    : _fourBarJoint(fourbarJoint), _secondJoint(secondJoint), _intakeServo(intakeServo)
{ }

void Arm::begin(){
  _fourBarJoint->write(0);
  _secondJoint->write(0);
  _intakeServo->write(_intakeOpen);
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

void Arm::setIntake(bool closed) {
    if(closed){
        _intakeServo->write(_intakeClosed);
    } else {
        _intakeServo->write(_intakeOpen);
    }
}