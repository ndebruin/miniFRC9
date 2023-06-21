#ifndef Mecanum_h
#define Mecanum_h

#include <Arduino.h>
#include <Alfredo_NoU2.h>

class mecanumDrivetrain
{
    public:
        mecanumDrivetrain(NoU_Motor* frontLeftMotor, NoU_Motor* frontRightMotor, NoU_Motor* backLeftMotor, NoU_Motor* backRightMotor);
        void begin();
        
        void set(double linearX, double linearY, double angularZ);

    private:
        NoU_Motor* _frontLeftMotor;
        NoU_Motor* _frontRightMotor;
        NoU_Motor* _backLeftMotor;
        NoU_Motor* _backRightMotor;
};

#endif