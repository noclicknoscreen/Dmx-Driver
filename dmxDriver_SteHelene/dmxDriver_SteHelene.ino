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
vector<animation> multipleAnims;

int scenario = 0;

#define scenFadeIn  0
#define scenSin   1
#define scenWaveA   2
#define scenWaveRandom   3
#define scenMultiple  4
#define scenFadeOut 5
#define MAX_SCENARIO 5

#define minBright 0
#define maxBright 150

long period = 5000;

int waveRandomIndex = 0;
int waveRandomBrightness = maxBright;

// Serial read and test instructions ----
String inputString = "";         // a string to hold incoming data
boolean stringComplete = false;  // whether the string is complete

/* ------------------------------------------
    Specific jean mone stuff
    Contours, Volumes
*/
vector<int> contoursChannels = {1};
vector<int> volumesChannels = {2, 3, 4};

/*
   BUTTON SECTION
*/
const int buttonPin = 2;    // the number of the pushbutton pin
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

/* ---------------------------------------
    Iterates channels
  --------------------------------------- */
void initScenario() {

  // Animation section
  contourValue.start(period);
  volumeValue.start(period);

  // Specific inits procedure
  switch (scenario) {
    case scenFadeIn:
      break;

    case scenSin:
      break;

    case scenWaveA:
      break;

    case scenWaveRandom:
      waveRandomIndex = getNewIndexwaveRandom();
      break;

    case scenMultiple:
      restartMultipleAnims();
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

/*
   SCENARII SECTION ----------------------------------------------------------
*/
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
    float brightness = volumeValue.loopSin(wavePhase, minBright, maxBright);

    // each channel has its phase --
    lightChannel(_channels[idx], brightness);

  }
}

void waveRandom(vector<int> _channels) {
  // Iterates channels --
  for (int idx = 0; idx < _channels.size(); idx++) {

    if (idx == waveRandomIndex) {
      float brightness = volumeValue.loopSin(0, minBright, maxBright);
      lightChannel(_channels[idx], brightness);

      // Change the index if lightness is reach
      if (brightness >= waveRandomBrightness) {
        waveRandomIndex = getNewIndexwaveRandom();
      }

    } else
      lightChannel(_channels[idx], waveRandomBrightness);
  }

}

void multiple(vector<int> _channels) {
  // Iterates channels --
  for (int idx = 0; idx < _channels.size(); idx++) {
    if (idx < multipleAnims.size()) {

      // -- Each channel has a different animation
      float animValue =  multipleAnims[idx].loopSin(0, minBright, maxBright);
      lightChannel(_channels[idx], animValue);

    } else {
      // -- Default is on
      lightChannel(_channels[idx], maxBright);

    }
  }

}

int getNewIndexwaveRandom() {
  return random(0, volumesChannels.size());;
}

void restartMultipleAnims() {
  for (int idx = 0; idx < multipleAnims.size(); idx++) {
    long newPeriod = random(0.5 * period, 1.5 * period);
    multipleAnims[idx].start(newPeriod);

    // -- log it
    Serial.print("Restarting animations [");
    Serial.print(idx);
    Serial.print("] with a new period [");
    Serial.print(newPeriod);
    Serial.print("]");
    Serial.println();

  }
}
void updateMultipleAnims() {
  for (int idx = 0; idx < multipleAnims.size(); idx++) {
    multipleAnims[idx].update();
  }
}
/*
   End of scenarii section
   -------------------------------------------------------------------
*/

/* ---------------------------------------------
    Root function (Sending DMX)
  --------------------------------------------- */
void lightChannel(int channel, long brightness) {
  /* Update DMX channel 1 to new brightness */
  DmxSimple.write(channel, brightness);
}
/* ----------------------------------------- */



/* ---------------------------------------------------------
    MAIN SECTION : Setup + Loop
*/
void setup() {
  // Log setup
  Serial.begin(9600);

  // Button pin setup
  pinMode(buttonPin, INPUT_PULLUP);

  // DMX protocol setup
  dmxSetup();

  // Animation section
  // Init multiple animations array
  for (int idx = 0; idx < volumesChannels.size(); idx++) {
    animation newAnim;
    multipleAnims.push_back(newAnim);
  }
  // Starting them
  restartMultipleAnims();

  // Scenario section
  scenario = 0;
  initScenario();

}

void dmxSetup() {
  DmxSimple.usePin(pinDmx);
  DmxSimple.maxChannel(contoursChannels.size() + volumesChannels.size());
}


void loop() {

  testInstructions();

  contourValue.update();
  volumeValue.update();
  updateMultipleAnims();

  readingButton();

  /* Sending Contours (Static) */
  switch (scenario) {

    case scenFadeIn:
      // FADE IN and still
      simple(contoursChannels, contourValue.fadeIn(0, 0, maxBright));
      simple(volumesChannels, volumeValue.fadeIn(0, 0, maxBright));
      break;

    case scenSin:
      // Sinus Lights
      simple(contoursChannels, maxBright);
      simple(volumesChannels, volumeValue.loopSin(0, minBright, maxBright));
      break;

    case scenWaveA:
      // Waving A spatially
      simple(contoursChannels, maxBright);
      waveA(volumesChannels);
      break;

    case scenWaveRandom:
      // Waving B spatially
      simple(contoursChannels, maxBright);
      waveRandom(volumesChannels);
      break;

    case scenMultiple:
      // FADE OUT
      simple(contoursChannels, maxBright);
      multiple(volumesChannels);
      break;

    case scenFadeOut:
      // FADE OUT
      simple(contoursChannels, contourValue.fadeOut(0, 0, maxBright));
      simple(volumesChannels, volumeValue.fadeOut(0, 0, maxBright));
      break;

    default:
      simple(contoursChannels, maxBright);
      simple(volumesChannels, maxBright);

  }

}

void readingButton() {
  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == LOW) {
        plusScenario();
      }
    }
  }

  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:
  lastButtonState = reading;

}

/* Sending Contours (Static) */
String scenarioName(int _scenario) {
  switch (_scenario) {
    case 0: return "Fade in";
    case 1: return "Still";
    case 2: return "Wave A";
    case 3: return "Wave Random";
    case 4: return "Multiple";
    case 5: return "Fade Out";
    default: return "No name";
  }
}

void plusScenario() {
  scenario++;
  if (scenario > MAX_SCENARIO) {
    scenario = 0;
  }

  initScenario();
  
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

