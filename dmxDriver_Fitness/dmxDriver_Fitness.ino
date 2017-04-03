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

/* DMX Declaration section
   Basics DMX settings
*/
#define pinDmx      3


/* ------------------------------------------
    Specific jean mone stuff
    Contours, Volumes
*/
#define nbContours 2
#define nbVolumes  2

int contoursChannels[nbContours] = {1, 4};
int volumesChannels[nbVolumes] = {2, 3};

/* ---------------------------------------
    Iterates Contours
  --------------------------------------- */
void lightContours(int brightness) {
  // Iterates channels --
  for (int idx = 0; idx < nbContours; idx++) {
    lightChannel(contoursChannels[idx], brightness);
  }
}

/* ---------------------------------------
    Iterates Volumes
  --------------------------------------- */
void lightVolumes(int brightness) {
  // Iterates each channels of volumes setup --
  for (int idx = 0; idx < nbVolumes; idx++) {
    lightChannel(volumesChannels[idx], brightness);
  }
}

/* ---------------------------------------------
    Root function (Sending DMX)
  --------------------------------------------- */
void lightChannel(int channel, int brightness) {
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

/*
 * ANIMATION SECTION : 
 * 
 */
animation contourValue;
animation volumeValue;

/* ---------------------------------------------------------
 *  MAIN SECTION : Setup + Loop
 */
void setup() {
  // Log setup
  Serial.begin(9600);

  // DMX protocol setup
  dmxSetup();

  // Animation section
  contourValue.start();
  volumeValue.start();

}

void dmxSetup() {
  DmxSimple.usePin(pinDmx);
  DmxSimple.maxChannel(nbContours + nbVolumes);
}


void loop() {

    /* Sending Contours (Static) */
    //lightContours(255);

    /* Sending Volumes (Moving) */
    lightVolumes(volumeValue.loopSin(5000, 0, 255));

}
