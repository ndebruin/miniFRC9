#ifndef ArmH
#define ArmH

#include <Arduino.h>
#include <Alfredo_NoU2.h>

class Arm
{
    public:
        Arm(NoU_Servo* fourBarJoint, NoU_Servo* secondJoint, NoU_Servo* intakeServo);
        void begin();

        void set(int fourBarAngle, int secondAngle);
        void set(char preset);
        void setIntake(bool closed);

    private:
        NoU_Servo* _fourBarJoint;
        NoU_Servo* _secondJoint;
        NoU_Servo* _intakeServo;

        int _angleFourBarStow = 2;
        int _angleSecondStow = 0;

        int _angleFourBarLow = 0;
        int _angleSecondLow = 20;

        int _angleFourBarMid = 0;
        int _angleSecondMid = 40;

        int _angleFourBarHigh = 30;
        int _angleSecondHigh = 40;

        int _angleFourBarDouble = 25;
        int _angleSecondDouble = 60; 

        int _angleFourBarSingle = 0;
        int _angleSecondSingle = 0; 

        int _angleFourBarFloor = 0;     
        int _angleSecondFloor = 75;

        int _intakeClosed = 135;
        int _intakeOpen = 45;
};

#endif