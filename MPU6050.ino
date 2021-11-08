#ifdef ENABLE_MPU6050

#include <MPU6050.h>
#include <math.h>


#define ARRAY_SIZE 100

MPU6050 mpu;
bool mpuReady;

unsigned char tiltArr[ARRAY_SIZE];
int current = 0;


bool* MPUSetup()
{
  // initialize device
  Serial.println(F("Initializing I2C devices..."));
  mpu.initialize();

  // verify connection
  Serial.println(F("Testing device connections..."));
  mpuReady = mpu.testConnection();
  Serial.println(mpuReady ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

  if (mpuReady)
  {
    //    calibrateWithDebugInv();
    Serial.print(F("tile Angle: "));
    Serial.println(getTiltAngle());

    calibrateWithDebugInv();

//    MPUCalibrator calibrator(mpu);
//    calibrator.startFullCalibration();
  }
  else
  {
    
  }
  return &mpuReady;
}

float getTemperature()
{
  //divide the temperature reading by 340 and then add +36.53.  The result is Celsius.
  return mpu.getTemperature() / 340.0 + 36.53;
}

float getTiltAngle()
{
  float z = mpu.getAccelerationZ();
  float ratio = z / 16384;
  ratio = ratio > 0.999 ? 0.999 : ratio;
  float angle = acos(ratio) / 3.1415 * 180;
  return angle;
}

// return value in Gs.
float getHorizontalMovement()
{
  float ax = mpu.getAccelerationX() / 16384.0;
  float ay = mpu.getAccelerationY() / 16384.0;
  float value = sqrt(ax * ax + ay * ay);
  return value < 10 ? value : 10;
}

void calibrateWithDebugInv()
{
  mpu.setXAccelOffset(-2364);
  mpu.setYAccelOffset(1396);
  mpu.setZAccelOffset(1514);

  mpu.setXGyroOffset(118);
  mpu.setYGyroOffset(-22);
  mpu.setZGyroOffset(53);
}

void printTemperature()
{
  float temp = getTemperature();
  Serial.print(F(" Temp = "));
  Serial.print(temp);
  Serial.println(F(" *C"));
}

void TiltDataSetup()
{
  for(int i=0; i<ARRAY_SIZE; i++)
  {
    tiltArr[i] = 0;
  }
}

float GetTiltData()
{
  float output = 0;
  for(int i=0; i<ARRAY_SIZE; i++)
  {
    output += tiltArr[i];
  }
  output /= ARRAY_SIZE;
  return output;
}

void SetTiltData()
{
  current = current % ARRAY_SIZE;
  tiltArr[current++] = getTiltAngle();
}

bool tiltTest()
{
  return GetTiltData() > 50;
}
#endif
