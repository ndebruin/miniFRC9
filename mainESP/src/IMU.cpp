#include "IMU.h"

/*
  Initialize the IMU and DMP

  Return Codes:
  0 - All is good
  1 - Cannot communicate with IMU
  2 - DMP setup failed
*/
uint8_t IMU::begin(uint8_t SDA, uint8_t SCL)
{
  // start i2c interface
  wire.begin(SDA, SCL);
  wire.setClock(400000);

  // start imu object comms
  imuObj.begin(wire, AD0_VAL);

  // check if we havent started talking
  // retry three times
  uint8_t count = 0;
  if(imuObj.status != ICM_20948_Stat_Ok && count < 3){
    delay(100);
    count++;
  }
  // if we can't comm
  if(imuObj.status != ICM_20948_Stat_Ok){
    return 1;
  }

  // assuming all is good, startup the DMP
  bool success = true;

  // init dmp
  success &= (imuObj.initializeDMP() == ICM_20948_Stat_Ok);

  // set output type
  success &= (imuObj.enableDMPSensor(INV_ICM20948_SENSOR_GAME_ROTATION_VECTOR) == ICM_20948_Stat_Ok);

  // set update rate
  // see example 7 of the library for more info
  // current value is ~10hz update rate
  success &= (imuObj.setDMPODRrate(DMP_ODR_Reg_Quat6, 4) == ICM_20948_Stat_Ok);

  // Enable the FIFO
  success &= (imuObj.enableFIFO() == ICM_20948_Stat_Ok);

  // Enable the DMP
  success &= (imuObj.enableDMP() == ICM_20948_Stat_Ok);

  // Reset DMP
  success &= (imuObj.resetDMP() == ICM_20948_Stat_Ok);

  // Reset FIFO
  success &= (imuObj.resetFIFO() == ICM_20948_Stat_Ok);

  if(success){
    return 0;
  }
  else{
    return 2;
  }
}

/*
  update the current accessible data from the imu
  CALL ONCE PER loop()
  copied verbatium from example 7
  
  Return Codes:
  0 - Read correctly
  1 - Got data, wrong type. IMU misconfigured.
  2 - Invalid Frame
  -1 - No data
*/
int8_t IMU::read()
{
  // Read any DMP data waiting in the FIFO
  icm_20948_DMP_data_t data;
  imuObj.readDMPdataFromFIFO(&data);

  if ((imuObj.status == ICM_20948_Stat_Ok) || (imuObj.status == ICM_20948_Stat_FIFOMoreDataAvail)) // Was valid data available?
  {

    if ((data.header & DMP_header_bitmap_Quat6) > 0) // We have asked for GRV data so we should receive Quat6
    {
      // Q0 value is computed from this equation: Q0^2 + Q1^2 + Q2^2 + Q3^2 = 1.
      // In case of drift, the sum will not add to 1, therefore, quaternion data need to be corrected with right bias values.
      // The quaternion data is scaled by 2^30.

      // Scale to +/- 1
      double q1 = ((double)data.Quat6.Data.Q1) / 1073741824.0; // Convert to double. Divide by 2^30
      double q2 = ((double)data.Quat6.Data.Q2) / 1073741824.0; // Convert to double. Divide by 2^30
      double q3 = ((double)data.Quat6.Data.Q3) / 1073741824.0; // Convert to double. Divide by 2^30

      // Convert the quaternions to Euler angles (roll, pitch, yaw)
      // https://en.wikipedia.org/w/index.php?title=Conversion_between_quaternions_and_Euler_angles&section=8#Source_code_2

      double q0 = sqrt(1.0 - ((q1 * q1) + (q2 * q2) + (q3 * q3)));

      double q2sqr = q2 * q2;

      // roll (x-axis rotation)
      double t0 = +2.0 * (q0 * q1 + q2 * q3);
      double t1 = +1.0 - 2.0 * (q1 * q1 + q2sqr);
      roll = atan2(t0, t1) * 180.0 / PI;

      // pitch (y-axis rotation)
      double t2 = +2.0 * (q0 * q2 - q3 * q1);
      t2 = t2 > 1.0 ? 1.0 : t2;
      t2 = t2 < -1.0 ? -1.0 : t2;
      pitch = asin(t2) * 180.0 / PI;

      // yaw (z-axis rotation)
      double t3 = +2.0 * (q0 * q3 + q1 * q2);
      double t4 = +1.0 - 2.0 * (q2sqr + q3 * q3);
      yaw = atan2(t3, t4) * 180.0 / PI;

      // we have read correctly, return 0
      return 0;
    }
    // if we get to here, that means that we received valid data, but the wrong type of data
    return 1;
  }

  // no data
  else if(imuObj.status == ICM_20948_Stat_FIFONoDataAvail){
    return -1;
  }

  // invalid data
  else if(imuObj.status == ICM_20948_Stat_FIFOIncompleteData){
    return 2;
  }
  return 2;
}


double IMU::getPitch(){
  return pitch;
}
double IMU::getRoll(){
  return roll;
}
double IMU::getYaw(){
  return yaw;
}