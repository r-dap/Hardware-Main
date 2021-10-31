//#define USE_TINY_GPS

#ifdef ENABLE_GPS

#include <TinyGPS.h>

//洞洞板
int gpsRx = 5;
int gpsTx = 6;

SoftwareSerial GPSSerial(gpsRx, gpsTx);

#ifdef USE_TINY_GPS
TinyGPS gps; // create gps object
#endif

String gps_buffer = "";

float lat = -1, lon = -1;

String lastValidBuffer = "";

char* GPGLL = "GPGLL";
int GPGLLprogress = 0;
String GPGLLbuffer = "";
bool GPGLLsensing = false;

void GPSSetup()
{
  GPSSerial.begin(9600);
  GPSSerial.println(F("AT"));
  Serial.println(F("GPS Enabled!"));
}

void GPSMain()
{
  GPSSerial.listen();
  //  Serial.println("GPS DATA: ");
  if (GPSSerial.isListening())
  {
    while (GPSSerial.available() > 0)
    {
      delay(1);
      char c = GPSSerial.read();
//      gps_buffer += c;
//      if (gps_buffer.length() > 80)
//      {
//        gps_buffer = "";
//      }
      EncodeGPS(c);
    }

    if (gps_buffer.length() > 0)
    {
      //      BTPrint(gps_buffer, false, true);


      for (auto x : gps_buffer)
      {
        //        Serial.write(x);
#ifdef USE_TINY_GPS
        if (gps.encode(x))
        {
          gps.f_get_position(&lat, &lon); // get latitude and longitude
          // display position
          GPSLocationRCP(lat, lon);

          if (lat <= 90)
          {
            GPSLocationRCP(lat, lon);
            Serial.print(F("Position: "));
            Serial.print(F("Latitude:"));
            Serial.print(lat, 6);
            Serial.print(F(", Longitude:"));
            Serial.println(lon, 6);
          }
          break;
        }
#else
        
#endif
      }
      while (GPSSerial.available() > 0)
      {
        GPSSerial.read();
      }
      gps_buffer = "";


#ifdef TRANSFER_SERIAL_TO_GPS

      while (Serial.available() > 0)
      {
        char c = Serial.read();
        Serial.print(c);
        GPSSerial.print(c);
      }

#endif
    }
  }
}

void EncodeGPS(char x)
{
  if (GPGLLsensing)
  {
    GPGLLbuffer += x;
  }
  else if (GPGLL[GPGLLprogress] == x)
  {
    GPGLLprogress++;
    if (GPGLLprogress == 5)
    {
      GPGLLsensing = true;
    }
  }
  else
  {
    GPGLLbuffer = "";
    GPGLLsensing = false;
    GPGLLprogress = 0;
  }

  if (x == '\r' || x == '\n')
  {
    if (GPGLLbuffer.length() != 0)
    {
      if (GPGLLbuffer.length() >= 30)
      {
        Serial.print(F("\r\nGPGLL: "));
        Serial.println(GPGLLbuffer);
        GPSLocationRawRCP(GPGLLbuffer);
        lastValidBuffer = GPGLLbuffer;
      }
      else
      {
        Serial.println(F("\r\nGPS content not valid."));
      }
      GPGLLbuffer = "";
      GPGLLsensing = false;
      GPGLLprogress = 0;
    }
  }
}

#endif

void EnableGPSListen()
{
#ifdef ENABLE_GPS
  GPSSerial.listen();
#endif
}
