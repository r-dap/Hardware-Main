#include <Arduino.h>
class IntervalAction
{
  public:
  unsigned long lastFired = 0;
  float interval = 100;

  IntervalAction(int interval)
  {
    this->interval = interval;
  }

  bool TryTrigger()
  {
    if(millis() - lastFired > interval)
    {
      lastFired = millis();
      return true;
    }
    return false;
  }
};
