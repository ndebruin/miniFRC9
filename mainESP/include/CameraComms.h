#ifndef CAM_h
#define CAM_h

#include <Arduino.h>
#include <Stream.h>

class CamComms
{
    public:
        void begin(Stream* streamPtr);

        uint8_t read();

        int getPixelOffset();
        void setFlash(bool on);

    private:
        int pixelOffset;
        
        Stream* SerialPort;
};

#endif