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
animation volumeValue;
animation contourValue;

int scenario = 0;

#define scenFadeIn  0
#define scenStill   1
#define scenWaveA   2
#define scenWaveRandom   3
#define scenRandom  4
#define scenFadeOut 5

long period = 5000;

int waveRandomIndex = 0;
int waveRandomBrightness = 255;

// Serial read and test instructions ----
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

/* ------------------------------------------
    Specific jean mone stuff
    Contours, Volumes
*/
vector<int> contoursChannels = {1};
vector<int> volumesChannels = {2, 3, 4};

/* ---------------------------------------
    Iterates channels
  --------------------------------------- */
void initScenario() {
  // Animation section
  contourValue.start(period);
  volumeValue.start(period);

  // Specific inits procedure
  /* Sending Contours (Static) */
  switch (scenario) {
    case scenFadeIn:
      break;

    case scenStill:
      break;

    case scenWaveA:
      break;

    case scenWaveRandom:
      waveRandomIndex = getNewIndexwaveRandom();
      break;

    case scenRandom:
      break;

    case scenFadeOut:
      break;

    default:
      break;

  }


  // -- log it
  Serial.print("Initializing scenario [");
  Serial.print(scenario);
  Serial.print("] : ");
  Serial.print(scenarioName(scenario));
  Serial.println();

}
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
    float brightness = volumeValue.loopSin(wavePhase, 0, 255);

    // each channel has its phase --
    lightChannel(_channels[idx], brightness);

  }
}

void waveRandom(vector<int> _channels) {

  // Iterates channels --
  for (int idx = 0; idx < _channels.size(); idx++) {

    if (idx == waveRandomIndex) {
      float brightness = volumeValue.loopSin(0, 0, 255);
      lightChannel(_channels[idx], brightness);

      // Change the index if lightness is reach
      if (brightness >= waveRandomBrightness) {
        waveRandomIndex = getNewIndexwaveRandom();
      }

    } else
      lightChannel(_channels[idx], waveRandomBrightness);
  }

}

int getNewIndexwaveRandom() {
  return random(0, volumesChannels.size());;
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
  initScenario();

  scenario = 0;

}

void dmxSetup() {
  DmxSimple.usePin(pinDmx);
  DmxSimple.maxChannel(contoursChannels.size() + volumesChannels.size());
}


void loop() {

  testInstructions();

  contourValue.update();
  volumeValue.update();

  /* Sending Contours (Static) */
  switch (scenario) {

    case scenFadeIn:
      // FADE IN and still
      simple(contoursChannels, contourValue.fadeIn(0, 0, 255));
      simple(volumesChannels, volumeValue.fadeIn(0, 0, 255));
      break;

    case scenStill:
      // Sinus Lights
      simple(contoursChannels, 255);
      simple(volumesChannels, volumeValue.loopSin(0, 0, 255));
      break;

    case scenWaveA:
      // Waving A spatially
      simple(contoursChannels, 255);
      waveA(volumesChannels);
      break;

    case scenWaveRandom:
      // Waving B spatially
      simple(contoursChannels, 255);
      waveRandom(volumesChannels);
      break;

    case scenRandom:
      // FADE OUT
      simple(contoursChannels, contourValue.fadeOut(0, 0, 255));
      simple(volumesChannels, volumeValue.fadeOut(0, 0, 255));
      break;

    case scenFadeOut:
      // FADE OUT
      simple(contoursChannels, contourValue.fadeOut(0, 0, 255));
      simple(volumesChannels, volumeValue.fadeOut(0, 0, 255));
      break;

    default:
      simple(contoursChannels, 255);
      simple(volumesChannels, 255);

  }

}

/* Sending Contours (Static) */
String scenarioName(int _scenario) {
  switch (_scenario) {
    case 0: return "Fade in";
    case 1: return "Still";
    case 2: return "Wave A";
    case 3: return "Wave B";
    case 4: return "Random";
    case 5: return "Fade Out";
    default: return "No name";
  }
}

/*
  SerialEvent occurs whenever a new data comes in the
  hardware serial RX.  This routine is run between each
  time loop() runs, so using delay inside loop can delay
  response.  Multiple bytes of data may be available.
*/
void testInstructions() {

  if (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }

  // then get into test instructions ---
  if (stringComplete == true) {

    Serial.print(inputString);

    // Choose the 1st scenario with key 1
    if (inputString.startsWith("0")) {
      scenario = 0;
      initScenario();
    }
    // Choose the 1st scenario with key 1
    if (inputString.startsWith("1")) {
      scenario = 1;
      initScenario();
    }
    // Choose the 1st scenario with key 1
    if (inputString.startsWith("2")) {
      scenario = 2;
      initScenario();
    }
    // Choose the 1st scenario with key 1
    if (inputString.startsWith("3")) {
      scenario = 3;
      initScenario();
    }
    // Choose the 1st scenario with key 1
    if (inputString.startsWith("4")) {
      scenario = 4;
      initScenario();
    }
    // Choose the 1st scenario with key 1
    if (inputString.startsWith("5")) {
      scenario = 5;
      initScenario();
    }

    // --
    stringComplete = false;
    inputString = "";

  }
}

