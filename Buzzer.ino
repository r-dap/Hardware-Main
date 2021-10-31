
#define BUZZER_PIN 8

IntervalAction buzzerAction(100);
//int buzzerEnabler = 0;
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

//  if (buzzerAction.TryTrigger())
//  {
//    if (buzzerEnabler <= 0)
//    {
//      DisableBuzzer();
//    }
//    else
//    {
//      buzzerState = !buzzerState;
//      digitalWrite(BUZZER_PIN, buzzerState);
//    }
//    buzzerEnabler = 0;
//  }

  if(millis() < buzzerTimer)
  {
    buzzerState = true;
  }
  else
  {
    buzzerState = false;
  }

  if(buzzerState)
  {
    digitalWrite(BUZZER_PIN, HIGH);
  }
  else
  {
    digitalWrite(BUZZER_PIN, LOW);
  }
  
#endif
}


void EnableBuzzer()
{
  if (activating == false)
  {
    activating = true;
    initTime = millis();
  }

  if (activating && millis() - initTime > 3000)
  {
    if (buzzerAction.TryTrigger())
    {
      digitalWrite(BUZZER_PIN, HIGH);
      delay(1);
      digitalWrite(BUZZER_PIN, LOW);
    }

  }
}

void DisableBuzzer()
{

  activating = false;
  initTime = millis();
  digitalWrite(BUZZER_PIN, LOW);
}
