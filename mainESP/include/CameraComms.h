#ifndef CAM_h
#define CAM_h

#include <Arduino.h>
#include <HardwareSerial.h>

class espCam
{
    public:
        espCam(uint8_t txPin, uint8_t rxPin, uint8_t espPort);
        void begin();
        int getPixelOffset();
        void setFlash(bool on);

    private:
        uint8_t _txPin;
        uint8_t _rxPin;
        
        HardwareSerial SerialPort;
};

#endif