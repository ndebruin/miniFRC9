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

        void set(double linearX, double linearY, double angularZ, double yaw);

        void setFlipped(bool flipped);

    private:
        NoU_Motor* _frontLeftMotor;
        NoU_Motor* _frontRightMotor;
        NoU_Motor* _backLeftMotor;
        NoU_Motor* _backRightMotor;
        
        float frontLeftCurrentPower;
        float backLeftCurrentPower;
        float frontRightCurrentPower;
        float backRightCurrentPower;

        float frontLeftDesiredPower;
        float backLeftDesiredPower;
        float frontRightDesiredPower;
        float backRightDesiredPower;

        // jerk limit!! in terms of change in -1 -> 1 domain

        double jerkLimit = 0.045;


        double toRadians(double degrees);

        double applyJerkLimit(double current, double desired);

        bool flipped = false;
};

#endif