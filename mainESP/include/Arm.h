#ifndef ArmH
#define ArmH

#include <Arduino.h>
#include <Alfredo_NoU2.h>

class Arm
{
    public:
        Arm(NoU_Servo* fourBarJoint, NoU_Servo* secondJoint);
        void begin();

        void set(int fourBarAngle, int secondAngle);
        void set(char preset);

    private:
        NoU_Servo* _fourBarJoint;
        NoU_Servo* _secondJoint;

        int _angleFourBarLow = 0;
        int _angleSecondLow = 0;

        int _angleFourBarMid = 0;
        int _angleSecondMid = 0;

        int _angleFourBarHigh = 0;
        int _angleSecondHigh = 0;

        int _angleFourBarDouble = 0;
        int _angleSecondDouble = 0; 

        int _angleFourBarSingle = 0;
        int _angleSecondSingle = 0; 

        int _angleFourBarFloor = 0;     
        int _angleSecondFloor = 0;
};

#endif