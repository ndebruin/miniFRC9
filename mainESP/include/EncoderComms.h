#ifndef Encoder_h
#define Encoder_h

#include <Arduino.h>
#include <Stream.h>

class EncoderComms
{
    public:
        void begin(Stream* streamPtr);
        
        uint8_t read();

        long getFrontLeft();
        long getFrontRight();
        long getBackLeft();
        long getBackRight();

    private:
        Stream* SerialPort;

        long frontLeftCount;
        long frontRightCount;
        long backLeftCount;
        long backRightCount;

        int ticksPerRev = 1024;
};

#endif