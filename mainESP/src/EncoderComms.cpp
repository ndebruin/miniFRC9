#include "EncoderComms.h"

void EncoderComms::begin(Stream* streamPtr){
    SerialPort = streamPtr;
}

// 0 - read data successfully
// 1 - no data
uint8_t EncoderComms::read(){
    if(SerialPort->available() > 0){
        frontLeftCount = SerialPort->readStringUntil(',').toInt();
        frontRightCount = SerialPort->readStringUntil(',').toInt();
        backLeftCount = SerialPort->readStringUntil(',').toInt();
        backRightCount = SerialPort->readStringUntil('\n').toInt();
        return 0;
    }
    return 1;

}

long EncoderComms::getFrontLeft(){return frontLeftCount/ticksPerRev;}
long EncoderComms::getFrontRight(){return frontRightCount/ticksPerRev;}
long EncoderComms::getBackLeft(){return backLeftCount/ticksPerRev;}
long EncoderComms::getBackRight(){return backRightCount/ticksPerRev;}