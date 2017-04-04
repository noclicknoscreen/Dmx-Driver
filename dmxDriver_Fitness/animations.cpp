/*
   Class designed to animate a little bits and withour delay

*/

#pragma once

#include "animations.h"

// Start function ----
void animation::start(long _period) {
  // Set the start time to current time
  m_startTime = millis();
  m_period = _period;
  // Set flag to ok !!
  m_isOn = true;
}

void animation::update() {
  m_ratio = getRatio();
  if (m_loop == false && m_ratio >= 1) {
    m_isOn = false;
  }
}

void animation::stop() {
  // Set flag to ok !!
  m_isOn = false;
}

// Warning : PERIOD is milliseconds --

/*
   sinusoid function between min and max
*/
float animation::loopSin(float _phase, long _min, long _max) {

  m_loop = true;

  float sinValue = sin(0.5 * TWO_PI * getRatioWithPhase(_phase));
  float sinMapped = floatMap(sinValue, 0, 1, _min, _max);

  return sinMapped;

}

/*
   linear ramp looping between min and max
*/
float animation::loopRamp(float _phase, long _min, long _max) {
  m_loop = true;

  return floatMap(getRatioWithPhase(_phase), 0, 1, _min, _max);

}

/*
   simple ramp function going from min to max
   It stopped after one occur
*/
float animation::fadeIn(float _phase, long _min, long _max) {
  m_loop = false;
  return m_ratio;
}

/*
   simple ramp function going from max to min
   It stopped after one occur
*/
float animation::fadeOut(float _phase, long _min, long _max) {
  m_loop = false;
  return 1 - m_ratio;
}

// Returns relative ratio calculted on period (as milliseconds)
float animation::getRatio() {

  // due to overclock every 50 days, get the absolute value --
  long timeDiff = abs(millis() - m_startTime);

  float ratio;

  if (m_loop) {
    ratio =  (float)(timeDiff % m_period);
  }

  ratio = ratio / (float)m_period;

  return ratio;

}

float animation::getRatioWithPhase(float _phase) {
  return fmod(m_ratio + _phase, 1);
}

static float animation::floatMap(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}
