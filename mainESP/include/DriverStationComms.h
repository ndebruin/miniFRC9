#ifndef DS_h

#define DS_h


#include <Arduino.h>
#include <BluetoothSerial.h>

class DriverStation
{
    public:
        bool begin(String robotName);
        uint8_t read();
        uint8_t write();

        unsigned long lastPacketTime();

        // rx values
        uint8_t enabled, ledState, flash;
        uint8_t intake, fourBar, secondJoint;
        int16_t frontLeft, frontRight, backLeft, backRight, turret;

        // tx values
        long frontLeftRot, frontRightRot, backLeftRot, backRightRot;
        double turretAngle;

        double pitch;
        double roll;
        double yaw;

        int cameraPixelOffset;


    private:
        BluetoothSerial serialBt;

        unsigned long lastTx;
        unsigned long lastRx;

        String txBuffer[9];
};

#endif