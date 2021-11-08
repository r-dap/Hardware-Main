
#define BUZZER_PIN 8

// for enabling buzzer
IntervalAction buzzerAction(100);

// for setting buzzer pin HIGH and LOW
IntervalAction buzzerIntervalAction(100);

// SOS timeing sequence
IntervalAction buzzerLong(400);
IntervalAction buzzerShort(100);
IntervalAction buzzerSleep(1000);
int section = 0; // 0 = waiting, 1~3 = short, 4~6 = Long, 7~9 = short, 10~11 = sleep

//int buzzerEnabler = 0;

// The actual buzzer pin output state
bool buzzerState = false;

int buzzerTimer = 0;
unsigned long initTime = 0;
bool activating = false;

void BuzzerSetup()
{
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  Serial.println(F("Buzzer Enabled!"));
  digitalWrite(BUZZER_PIN, HIGH);
  delay(200);
}

void BuzzerMain()
{
#ifdef ENABLE_BUZZER

  if (activating && (initTime - millis() > 15000))
  {
    activating = false;
  }

//  SOSBuzzing();
  NormalBuzzing();


#endif
}

void SOSBuzzing()
{
  if(activating)
  {
    if((1 <= section && section <= 4) || (9 <= section && section <= 12))
    {
      if (buzzerShort.TryTrigger())
      {
        if (buzzerState)
        {
          buzzerState = false;
          digitalWrite(BUZZER_PIN, LOW);
        }
        else
        {
          buzzerState = true;
          digitalWrite(BUZZER_PIN, HIGH);
          section++;
        }
        Serial.println(section);
      }
    }
    if(5 <= section && section <= 8)
    {
      if (buzzerLong.TryTrigger())
      {
        if (buzzerState)
        {
          buzzerState = false;
          digitalWrite(BUZZER_PIN, LOW);
        }
        else
        {
          buzzerState = true;
          digitalWrite(BUZZER_PIN, HIGH);
          section++;
        }
        Serial.println(section);
      }
    }
    if(section == 13 && section == 14)
    {
      if (buzzerSleep.TryTrigger())
      {
        if(section == 14)
        {
          section = 1;
        }
      }
    }
  }
}

void NormalBuzzing()
{
  if (activating)
  {
    if (buzzerIntervalAction.TryTrigger())
    {
      if (buzzerState)
      {
        buzzerState = false;
        digitalWrite(BUZZER_PIN, LOW);
      }
      else
      {
        buzzerState = true;
        digitalWrite(BUZZER_PIN, HIGH);
      }
    }
  }
}


void EnableBuzzer()
{
  if (activating == false)
  {
    section = 1;
    activating = true;
    initTime = millis() + 3000;
  }
}

void DisableBuzzer()
{
  if (activating == false)
  {
    section = 0;
    activating = false;
    initTime = millis();
    digitalWrite(BUZZER_PIN, LOW);
  }
}
