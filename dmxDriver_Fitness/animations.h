/*
   Class designed to animate a little bits and withour delay

*/

#pragma once

#include "arduino.h"

class animation {

  public:

    // Start function ----
    void start() {
      // Set the start time to current time
      m_startTime = millis();
      // Set flag to ok !!
      m_isOn = true;
    }

    void stop() {
      // Set flag to ok !!
      m_isOn = false;
    }

    // Warning : PERIOD is milliseconds --

    /*
       sinusoid function between min and max
    */
    float loopSin(long _period, long _min, long _max) {

      float percent = getPercent(_period);
      float sinValue = sin(TWO_PI * percent);
      float sinMapped = floatMap(sinValue, -1, 1, _min, _max);
      /*
      Serial.print("Sinus value = ");
      Serial.print(sinValue);
      Serial.print(" : mapped value = ");
      Serial.print(sinMapped);
      Serial.println();
      */
      return sinMapped;
      
    }

    /*
       linear ramp looping between min and max
    */
    float loopRamp(long _period, long _min, long _max) {
      return floatMap(getPercent(_period), 0, 1, _min, _max);
    }

    /*
       simple ramp function going from min to max
       It stopped after one occur
    */
    float fadeIn(long _period, long _min, long _max);

    /*
       simple ramp function going from max to min
       It stopped after one occur
    */
    float fadeOut(long _period, long _min, long _max);

  private:
    unsigned long m_startTime;
    bool          m_isOn = false;

    // Returns relative ratio calculted on period (as milliseconds)
    float getPercent(long _period) {

      // due to overclock every 50 days, get the absolute value --
      long timeDiff = abs(millis() - m_startTime);

      return (float)(timeDiff % _period) / (float)_period;

    }

  public:
    float floatMap(float x, float in_min, float in_max, float out_min, float out_max)
    {
      return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }

};
