SoftwareSerial LTE(10, 11);
String sim808Echo = "";
String serialInput = "";

void Wait(int timeout = 10000)
{
  unsigned long initTime = millis();
  while (LTE.available() == 0) {
    if (millis() - initTime > timeout) return;
  }
}

String WaitAndReturn(int timeout = 10000)
{
  String rt = "";
  Wait(timeout);
  while (LTE.available() > 0)
  {
    rt += (char)LTE.read();
    delay(1);
  }
  return rt;
}

String WaitAndPrint(int timeout = 10000)
{
  String rt = WaitAndReturn(timeout);
  Serial.println("LTE >>> " + rt);
  return rt;
}

int WaitAndCompare(String content, int timeout = 10000)
{
  String rt = WaitAndPrint(timeout);
  return rt.indexOf(content);
}

void SendCommand(String cmd)
{
  Serial.println("LTE <<< " + cmd);
  LTE.flush();
  LTE.print(cmd);
}

String SendCommandAndWait(String cmd, int timeout = 10000)
{
  SendCommand(cmd);
  return WaitAndPrint(timeout);
}

String* Parse(String content)
{
  int count = 1;
  for (auto x : content)
  {
    if (x == ',') count++;
  }
  String* values = new String[count];
  int progress = 0;
  int index = content.indexOf(',');
  while (index != -1)
  {
    values[progress++] = content.substring(0, index);
    content = content.substring(index + 1, content.length());
    index = content.indexOf(',');
  }
  values[progress++] = content.substring(0, index);

  return values;
}

String* SendCommandAndParse(String command, int timeout = 10000)
{
  return Parse(SendCommandAndWait(command, timeout));
}

bool SendCommandAndExpect(String command, String expectation, int timeout = 10000)
{
  return SendCommandAndWait(command).indexOf(expectation) != -1;
}

bool WaitUntilExpect(String expectation, int timeout = 10000)
{
  Serial.println("Waitint for: " + expectation);
  unsigned long initTime = millis();
  while (WaitAndCompare(expectation, timeout) == -1)
  {
    if (millis() - initTime > timeout)
    {
      Serial.println(F("Expectation not satisfied"));
      return false;
    }
  }
  Serial.println(F("Expectation Satisfied"));
  return true;
}

bool SendCommandUntilExpect(String command, String expectation, int timeout = 10000)
{
  SendCommand(command);
  return WaitUntilExpect(expectation, timeout);
}

void readSerialPort()
{
  while (Serial.available() > 0)
  {
    char d = Serial.read(); //gets one byte from serial buffer
    serialInput += d;          //makes the string readString
    delay(10);
  }
}

// ------------------------------------------------------------------------------------------
//                                    Function End
// ------------------------------------------------------------------------------------------

String bearer = "358|u3GdjfxSIXVe0dsRg4ZtyXxWMKssylNdmyop7MYT";
String email = "hardware599@gmail.com";
String password = "123456789";
String userid = "36";
String post = "https://r-dap.com/api/position/userPosition";
String disaster = "https://r-dap.com/api/disasternet/happenaccident";

// COMMANDS

String OK = "OK";
String HTTPTERM = "AT+HTTPTERM\r";
String HTTPINIT = "AT+HTTPINIT\r";

void ResetHTTPSession()
{
  SendCommandUntilExpect(HTTPTERM, OK, 1000);
  SendCommandUntilExpect(HTTPINIT, OK, 12000);
}

//void Login()
//{
//  Serial.println(F("Proceeding login process"));
//  delay(2000);
//  ResetHTTPSession();
////  WaitUntilExpect("ATREADY", 12000);
//  SendCommandAndWait(F("AT+HTTPPARA=\"URL\", \"https://r-dap.com/api/auth/login\"\r"), 1000);
//  SendCommandAndWait(F("AT+HTTPPARA=\"CONTENT\",\"application/json\"\r"), 1000);
//  SendCommandAndWait(F("AT+HTTPDATA=80,10000\r"), 1000);
//  LTE.print("{\"email\":\"");
//  LTE.print(email);
//  LTE.print("\", \"password\":\"");
//  LTE.print(password);
//  LTE.print("\", \"fcm_token\":\"1234567\"}");
//  WaitAndPrint(1000);
//  SendCommandAndWait(F("AT+HTTPACTION=1\r"), 1000);
////  WaitUntilExpect("HTTPACTION", 60000);
//  String rt = WaitAndPrint();
//
//  SendCommandAndWait("AT+HTTPHEAD\r");
//  Serial.println("RT+++=> " + rt);
//  String returnLength = Parse(rt)[2];
//  Serial.println("Length: " + returnLength);
//  SendCommandAndWait("AT+HTTPREAD=" + returnLength + "\r");
//  WaitAndPrint(3000);
//  Serial.println(F("Finished login process"));
//}

void LLSetup(String url)
{
  Serial.println(F("Proceeding LL Setup"));
  LTE.listen();
  LTE.flush();s
  ResetHTTPSession();
  SendCommand(F("AT+HTTPPARA=\"URL\", \""));
  SendCommand(url);
  SendCommandAndWait(F("\"\r"));
  
  SendCommandAndWait(F("AT+HTTPPARA=\"CONTENT\",\"application/json\"\r"));
  
  SendCommand(F("AT+HTTPPARA=\"USERDATA\",\"Authorization:Bearer "));
  SendCommand(bearer);
  SendCommandAndWait(F("\"\r"));
  
  Serial.println(F("Finished setting up"));
}

String latStr = "120.12";
String lonStr = "22.56";

void PostLL(float lat, float lon)
{
  PrintRamData();
  LLSetup(post);
  Serial.println(F("Proceeding Post LL process"));
  SendCommandAndWait(F("AT\r"), 3000);

  char latBuffer[] = "";
  String latStr = dtostrf(lat, 3, 4, latBuffer);

  char lonBuffer[] = "";
  String lonStr = dtostrf(lon, 3, 4, lonBuffer);

  Serial.println(lat);
  Serial.println(lon);

  int length = 31 + latStr.length() + lonStr.length();
  Serial.println(length);

  SendCommand(F("\rAT+HTTPDATA="));
  SendCommand(String(length));
  SendCommandAndWait(F(", 10000\r"));

  //  SendCommandAndWait(F("AT+HTTPDATA=42,10000\r"));

  SendCommand(F("{\"longitude\":\""));
  SendCommand(lonStr);
  SendCommand(F("\", \"latitude\":\""));
  SendCommand(latStr);
  SendCommandAndWait(F("\"}"));

  SendCommandAndWait(F("AT+HTTPACTION=1\r"));
//  WaitAndPrint(60000);

  //  String rt = WaitAndPrint(30000);
  //  Serial.println("RT+++=> " + rt);
  //  String returnLength = Parse(rt)[2];
  //  Serial.println("Length: " + returnLength);
  //  SendCommandAndWait("AT+HTTPREAD=" + returnLength + "\r");
  //  SendCommandAndWait("AT+HTTPREAD=182\r");
  Serial.println(F("Finished Post LL process"));
}

void PostLLRaw(String rawInfo)
{
  LLSetup(post);
  Serial.println(F("Proceeding Post LL process"));
  SendCommandAndWait(F("AT\r"), 3000);

  SendCommand(F("\rAT+HTTPDATA="));
  SendCommand(String(rawInfo.length() + 31));
  SendCommandAndWait(F(", 10000\r"));

  //  SendCommandAndWait(F("AT+HTTPDATA=42,10000\r"));

  SendCommand(F("{\"longitude\":\"\", \"latitude\":\""));
  SendCommand(rawInfo);
  SendCommandAndWait(F("\"}"));

  SendCommandAndWait(F("AT+HTTPACTION=1\r"));
//  WaitAndPrint(60000);

  //  String rt = WaitAndPrint(30000);
  //  Serial.println("RT+++=> " + rt);
  //  String returnLength = Parse(rt)[2];
  //  Serial.println("Length: " + returnLength);
  //  SendCommandAndWait("AT+HTTPREAD=" + returnLength + "\r");
  //  SendCommandAndWait("AT+HTTPREAD=182\r");
  Serial.println(F("Finished Post LL process"));
}

void PostDisaster(int type)
{
  LLSetup(disaster);
  Serial.println(F("Proceeding Disaster process"));
  
  SendCommand(F("\rAT+HTTPDATA="));
  SendCommand(String(userid.length() + 62));
  SendCommandAndWait(F(", 10000\r"));

  SendCommand(F("{\"longitude\":\"120\",\"latitude\":\"23.5\", \"userid\":\""));
  SendCommand(userid);
  SendCommand(F("\",\"event\":\""));
  SendCommand(String(type));
  SendCommandAndWait(F("\"}"));

  SendCommandAndWait(F("AT+HTTPACTION=1\r"));
//  WaitAndPrint(60000);
//  SendCommandAndWait(F("AT+HTTPREAD=50\r"));
  Serial.println(F("Finished posting disaster"));
}

void LTESetup()
{
  LTE.begin(115200);
  while (!LTE);

//  if (!SendCommandUntilExpect(F("\rAT\r"), F("OK")))
//  {
//    Serial.println(F("Waiting for LTE to wake up, usually take less than 10 seconds."));
//    while (millis() < 10000);
//
//    WaitAndPrint(1000);
//  }

  while (!SendCommandUntilExpect(F("\rAT\r"), F("OK"), 3000))
  {
    Serial.println(F("Waiting LTE"));
  }
  //  SendCommandAndWait("ATE0\r", 1000);// Turn off LTE AT command echo.
  //  SendCommandAndWait("AT+IPR=9600\r", 1000);
  //  LTE.begin(9600);
  //  while (!LTE);
  SendCommandAndWait(F("ATE0\r"), 1000);// Turn off LTE AT command echo.

  Serial.println();

  Serial.println(F("LTE Enabled!"));
  Serial.flush();

  LLSetup(post);
}

void LTEMain()
{
  //  WaitAndPrint(10);
}
