#ifndef Arm_h
#define Arm_h

#include <Arduino.h>
#include <Alfredo_NoU2.h>

class arm
{
    public:
        arm(NoU_Servo* fourbar_joint, NoU_Servo* second_joint);
        void begin();

        void set(int fourbar_angle, int second_angle);

    private:
        NoU_Servo* _fourbar_joint;
        NoU_Servo* _second_joint;

        int _fourbar_angle_low = 0;
        int _fourbar_angle_mid = 0;
        int _fourbar_angle_high = 0;
        int _fourbar_angle_load = 0;
        int _fourbar_angle_floor = 0;

        int _second_angle_low = 0;
        int _second_angle_mid = 0;
        int _second_angle_high = 0;
        int _second_angle_load = 0; 
        int _second_angle_floor = 0;
};