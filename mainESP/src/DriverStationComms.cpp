#include <Arduino.h>
#include <BluetoothSerial.h>

#include "DriverStationComms.h"

bool DriverStation::begin(String robotName){
  return serialBt.begin(robotName);
}

uint8_t DriverStation::read(){
  while (serialBt.available() > 0) {
    lastRx = millis();
    frontLeft = serialBt.readStringUntil(',').toInt();
    frontRight = serialBt.readStringUntil(',').toInt();
    backLeft = serialBt.readStringUntil(',').toInt();
    backRight = serialBt.readStringUntil(',').toInt();

    turret = serialBt.readStringUntil(',').toInt();

    intake = serialBt.readStringUntil(',').toInt();
    armPreset = serialBt.readStringUntil(',').toInt();
    
    enabled = serialBt.readStringUntil(',').toInt();
    ledState = serialBt.readStringUntil(',').toInt();
    flash = serialBt.readStringUntil(',').toInt();
    return 0;
  }
  return 0;
}

uint8_t DriverStation::write(){
  if(millis() - lastTx > 100){
    lastTx = millis();

    txBuffer[0] = frontLeftRot;
    txBuffer[1] = frontRightRot;
    txBuffer[2] = backLeftRot;
    txBuffer[3] = backRightRot;

    txBuffer[4] = turretAngle;

    txBuffer[5] = pitch;
    txBuffer[6] = roll;
    txBuffer[7] = yaw;
    
    txBuffer[8] = cameraPixelOffset;

    String txString;
    for(int i=0;i<9;i++){
      txString += (txBuffer[i]);
      if(i != 8){
        txString += ",";
      }
    }

    serialBt.println(txString);
    return 0;
  }
  return 0;
}

unsigned long DriverStation::lastPacketTime(){
  return lastRx;
}