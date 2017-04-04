/* Welcome to DmxSimple. This library allows you to control DMX stage and
** architectural lighting and visual effects easily from Arduino. DmxSimple
** is compatible with the Tinker.it! DMX shield and all known DIY Arduino
** DMX control circuits.
**
** DmxSimple is available from: http://code.google.com/p/tinkerit/
** Help and support: http://groups.google.com/group/dmxsimple       */

/* To use DmxSimple, you will need the following line. Arduino will
** auto-insert it if you select Sketch > Import Library > DmxSimple. */

#include <DmxSimple.h>
#include "animations.h"

#include <StandardCplusplus.h>
#include <vector>
using namespace std;

/* DMX Declaration section
   Basics DMX settings
*/
#define pinDmx      3

/* ANIMATION SECTION */
animation animatedValue;

int scenario = 0;
long period = 5000;

/* ------------------------------------------
    Specific jean mone stuff
    Contours, Volumes
*/
vector<int> contoursChannels = {5, 6};
vector<int> volumesChannels = {1, 2, 3, 4};

/* ---------------------------------------
    Iterates channels
  --------------------------------------- */
void simple(vector<int> _channels, int _brightness) {
  // Iterates channels --
  for (int idx = 0; idx < _channels.size(); idx++) {
    lightChannel(_channels[idx], _brightness);
  }
}

void waveA(vector<int> _channels) {

  // Iterates channels --
  for (int idx = 0; idx < _channels.size(); idx++) {

    float wavePhase = animation::floatMap(idx, 0.0f, _channels.size(), 0.0f, 0.25f);
    float brightness = animatedValue.loopSin(wavePhase, 0, 255);

    Serial.print("idx : ");
    Serial.print(idx);
    Serial.print(" / ");
    Serial.print(_channels.size());
    Serial.print(" Phase : ");
    Serial.print(wavePhase);
    Serial.print(" Brightness : ");
    Serial.print(brightness);
    Serial.println();
    

   // each channel has its phase --
    lightChannel(_channels[idx], brightness);
    
  }
}

void waveB(vector<int> _channels) {

  // Iterates channels --
  for (int idx = 0; idx < _channels.size(); idx++) {

    long wavePhase = animation::floatMap(idx, _channels.size(), 0, 0, PI * period / (float)_channels.size());
    float brightness = animatedValue.loopSin(wavePhase, 0, 255);

    // each channel has its phase --
    lightChannel(_channels[idx], brightness);
    
  }
}

/* ---------------------------------------------
    Root function (Sending DMX)
  --------------------------------------------- */
void lightChannel(int channel, long brightness) {
  /* Update DMX channel 1 to new brightness */
  DmxSimple.write(channel, brightness);

  /* Log brightness written ---- */
  /*
  Serial.print("Brightness [");
  Serial.print(brightness);
  Serial.print("] send to channel [");
  Serial.print(channel);
  Serial.print("]");
  Serial.println();
  */
}
/* ----------------------------------------- */



/* ---------------------------------------------------------
    MAIN SECTION : Setup + Loop
*/
void setup() {
  // Log setup
  Serial.begin(9600);

  // DMX protocol setup
  dmxSetup();

  // Animation section
  animatedValue.start(period);

  scenario = 2;

}

void dmxSetup() {
  DmxSimple.usePin(pinDmx);
  DmxSimple.maxChannel(contoursChannels.size() + volumesChannels.size());
}


void loop() {

  animatedValue.update();

  /* Sending Contours (Static) */
  simple(contoursChannels, 255);

  switch (scenario) {
    case 0:
      // FADE IN and still
      simple(volumesChannels, 255);
      break;

    case 1:
      // Sinus Lights
      /* Sending Volumes (Moving) */
      simple(volumesChannels, animatedValue.loopSin(0, 0, 255));
      break;

    case 2:
      // Waving A spatially
      waveA(volumesChannels);
      break;

    case 3:
      // Waving B spatially
      waveB(volumesChannels);
      break;

    case 4:
      // FADE OUT
      break;

    default:
      simple(contoursChannels, 255);
      simple(volumesChannels, 255);

  }


}
