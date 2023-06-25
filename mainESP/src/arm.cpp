#include <Arduino.h>
#include <Alfredo_NoU2.h>
#include "arm.h"

arm::arm(NoU_Servo* fourbar_joint, NoU_Servo* second_joint) 
                                    : _fourbar_joint(fourbar_joint), _second_joint(second_joint)
{ }

void arm::begin(){
  _fourbar_joint->write(0);
  _second_joint->write(0);
}

void arm::set(String preset) {
    if (preset.equals("LOW")) {
        _fourbar_joint->write(_fourbar_angle_low);
        _second_joint->write(_second_angle_low);
    }
    else if (preset.equals("MID")) {
        _fourbar_joint->write(_fourbar_angle_mid);
        _second_joint->write(_second_angle_mid);
    }
    else if (preset.equals("HIGH")) {
        _fourbar_joint->write(_fourbar_angle_high);
        _second_joint->write(_second_angle_high);
    }
    else if (preset.equals("LOAD")) {
        _fourbar_joint->write(_fourbar_angle_load);
        _second_joint->write(_second_angle_load);
    }
    else if (preset.equals("FLOOR")) {
        _fourbar_joint->write(_fourbar_angle_floor);
        _second_joint->write(_second_angle_floor);
    }
    else if (preset.equals("STOW")) {
        _fourbar_joint->write(0);
        _second_joint->write(0);
    }
}