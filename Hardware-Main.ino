#include <SoftwareSerial.h>
#include "IntervalAction.cpp"
#include <MemoryUsage.h>;

#define ENABLE_GPS
//#define ENABLE_BT
#define ENABLE_LTE
#define ENABLE_MPU6050
#define ENABLE_BUZZER

using namespace std;

bool* mpuReadyPtr;

IntervalAction tiltAction(15000);
//IntervalAction temperatureAction(3000);
IntervalAction collideAction(15000);
IntervalAction gpsAction(20000);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  PrintRamData();

#ifdef ENABLE_LTE
  LTESetup();
#endif

#ifdef ENABLE_GPS
  GPSSetup();
#endif

#ifdef ENABLE_MPU6050
  mpuReadyPtr = MPUSetup();
#endif

#ifdef ENABLE_BUZZER
  BuzzerSetup();
#endif

  Serial.println(F("All Setup Finished!"));
}

void loop() {
#ifdef ENABLE_LTE
  LTEMain();
#endif

#ifdef ENABLE_BT
  BTMain();
#endif

#ifdef ENABLE_GPS
  EnableGPSListen();
  GPSMain();
#endif

#ifdef ENABLE_MPU6050
  SetTiltData();

  if (*mpuReadyPtr)
  {
    float tiltAngle = GetTiltData();
    //        Serial.print("TileAngle: ");
    //        Serial.println(tiltAngle);
    if (tiltAngle > 45)
    {
      EnableBuzzer();
      if (tiltAction.TryTrigger())
      {
        Serial.print(F("Tilted!"));
        Serial.println(tiltAngle);
        PostDisaster(2);
        //        BTPrint("Tilted: " + (String)tiltAngle, true, true);
      }
    }
    else
    {
      DisableBuzzer();
    }
    //    int temperature = getTemperature();
    //    if (temperature > 50)
    //    {
    //      if (temperatureAction.TryTrigger())
    //      {
    //        Serial.print("HighTemerature: ");
    //        Serial.println(temperature);
    //        //        BTPrint(F("HighTemperature"), true, true);
    //      }
    //      EnableBuzzer();
    //    }
  }


  float movement = getHorizontalMovement();
  if (movement > 2)
  {
    if (collideAction.TryTrigger())
    {
      Serial.print(F("Collided: "));
      Serial.println(movement);
      PostDisaster(1);
      //      BTPrint("Collided: " + (String)movement, true, true);
    }
    EnableBuzzer();
  }
  else
  {
    DisableBuzzer();
  }
#endif

  BuzzerMain();
}

void GPSLocationRCP(float latitude, float longitude)
{
  if(gpsAction.TryTrigger())
  {
    PostLL(latitude, longitude);
  }
}

void GPSLocationRawRCP(String rawInfo)
{
  if(gpsAction.TryTrigger())
  {
    PostLLRaw(rawInfo);
  }
}

void PrintRamData()
{
  MEMORY_PRINT_START
  MEMORY_PRINT_HEAPSTART
  MEMORY_PRINT_HEAPEND
  MEMORY_PRINT_STACKSTART
  MEMORY_PRINT_END
  MEMORY_PRINT_HEAPSIZE
  FREERAM_PRINT
}
