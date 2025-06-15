#ifndef ArmH
#define ArmH

#include <Arduino.h>
#include <Alfredo_NoU2.h>

class Arm
{
    public:
        Arm(NoU_Servo* fourBarJoint, NoU_Servo* secondJoint, NoU_Motor* intakeMotor);
        void begin();

        void set(int fourBarAngle, int secondAngle);
        void setDelay(int fourBarAngle, int secondAngle);
        void setHighAuton();
        void set(char preset);
        void setIntake(double on);

    private:
        NoU_Servo* _fourBarJoint;
        NoU_Servo* _secondJoint;
        NoU_Motor* _intakeMotor;

        int _angleFourBarStow = 0;
        int _angleSecondStow = 0;

        int _angleFourBarLow = 0;
        int _angleSecondLow = 60;

        int _angleFourBarMid = 30;
        int _angleSecondMid = 85;

        int _angleFourBarHigh = 30;
        int _angleSecondHigh = 75;

        int _angleFourBarDouble = 25;
        int _angleSecondDouble = 80; 

        int _angleFourBarSingle = 0;
        int _angleSecondSingle = 0; 

        int _angleFourBarFloor = 0;     
        int _angleSecondFloor = 93;
};

#endif