#include "CameraComms.h"

void CamComms::begin(Stream* streamPtr){
    SerialPort = streamPtr;
}

// 0 - read data successfully
// 1 - no data
uint8_t CamComms::read(){
    if(SerialPort->available() > 0){
        pixelOffset = SerialPort->readStringUntil('\n').toInt();
        return 0;
    }
    return 1;
}

int CamComms::getPixelOffset(){return pixelOffset;}
