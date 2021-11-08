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
  if(rt.length() > 0)
  {
    Serial.println("LTE >>> " + rt);
  }
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
// --------------------------------------`----------------------------------------------------

String bearer = "358|u3GdjfxSIXVe0dsRg4ZtyXxWMKssylNdmyop7MYT";
//String email = "hardware599@gmail.com";
//String password = "123456789";
String userid = "36";
String url = "https://r-dap.com/api/hw";

// COMMANDS

String OK = "OK";
String HTTPTERM = "AT+HTTPTERM\r";
String HTTPINIT = "AT+HTTPINIT\r";

void ResetHTTPSession()
{
  SendCommandUntilExpect(HTTPTERM, OK, 500);
  SendCommandUntilExpect(HTTPINIT, OK, 500);
}

void APISetup()
{
  Serial.println(F("Proceeding LL Setup"));
  LTE.listen();
  LTE.flush();
  ResetHTTPSession();
  SendCommand(F("AT+HTTPPARA=\"URL\", \""));
  SendCommand(url);
  SendCommandAndWait(F("\"\r"), 500);
  
  SendCommandAndWait(F("AT+HTTPPARA=\"CONTENT\",\"application/json\"\r"), 500);
  
  SendCommand(F("AT+HTTPPARA=\"USERDATA\",\"Authorization:Bearer "));
  SendCommand(bearer);
  SendCommandAndWait(F("\"\r"), 500);
  
  Serial.println(F("Finished setting up"));
}


void PostMessage(String rawInfo, String eventType, String value)
{
  #ifdef ENABLE_LTE
  APISetup();
  Serial.println(F("Posting message"));
  SendCommandAndWait(F("AT\r"), 500);

  Serial.println("Posting following message: ");
  Serial.println(rawInfo);

  Serial.println("Event Type: ");
  Serial.println(eventType);

  Serial.println("value: ");
  Serial.println(value);

  SendCommand(F("\rAT+HTTPDATA="));
  SendCommand(String(rawInfo.length() + eventType.length() + value.length() + 23));
  SendCommandAndWait(F(", 1000\r"), 500);

  SendCommand(F("{\"RD\":\""));
  SendCommand(rawInfo);
  SendCommand(F("\",\"E\":\""));
  SendCommand(eventType);
  SendCommand(F("\",\"V\":\""));
  SendCommand(value);
  SendCommandAndWait(F("\"}"), 500);
  
  SendCommandAndWait(F("AT+HTTPACTION=1\r"), 500);
  Serial.println(F("Finished postiing message"));
  #endif
}

void LTESetup()
{
  LTE.begin(115200);
  while (!LTE);
  
  while (!SendCommandUntilExpect(F("\rAT\r"), F("OK"), 3000))
  {
    Serial.println(F("Waiting LTE"));
  }

  
  
  SendCommandAndWait(F("ATE0\r"), 1000);// Turn off LTE AT command echo.

  Serial.println();

  Serial.println(F("LTE Enabled!"));
  Serial.flush();

//  APISetup();
  
}

void LTEMain()
{
    WaitAndPrint(1);
}
