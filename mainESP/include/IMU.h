#ifndef IMU_h
#define IMU_h

#include <Arduino.h>
#include <ICM_20948.h>
#include <Wire.h>

#define AD0_VAL 1

class IMU
{
    public:
        //IMU(uint8_t i2cBus);
        uint8_t begin(uint8_t SDA, uint8_t SCL);
        double getPitch();
        double getRoll();
        double getYaw();
        int8_t read();

    private:
        TwoWire wire = TwoWire(0);
        ICM_20948_I2C imuObj;
        double pitch;
        double roll;
        double yaw;
};

#endif