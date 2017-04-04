/*
   Class designed to animate a little bits and withour delay

*/

#pragma once

#include "arduino.h"

class animation {

  public:

    // Start function ----
    void start(long _period);
    void update();
    void stop();

    // Warning : PERIOD is milliseconds --
    float loopSin(float _phase, long _min, long _max);
    float loopRamp(float _phase, long _min, long _max);
    float fadeIn(float _phase, long _min, long _max);
    float fadeOut(float _phase, long _min, long _max);

    static float floatMap(float x, float in_min, float in_max, float out_min, float out_max);

  private:
    unsigned long m_startTime;
    float         m_ratio = 0;
    long          m_period;
    bool          m_isOn = false;
    bool          m_loop = false;

    float getRatio();
    float getRatioWithPhase(float _phase);

};
