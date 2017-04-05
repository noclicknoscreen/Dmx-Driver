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

#define scenFadeIn      0
#define scenStill       1
#define scenSin         2
#define scenWaveA       3
#define scenWaveRandom  4
#define scenMultiple    5
#define scenFadeOut     6

#define MAX_SCENARIO    5

#define minBright 0
#define maxBright 150

long animPeriod = 5000;
long waveAPeriod = 2000;
long fadePeriod = 1000;

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
unsigned long buttonDebounceTime = 0;  // the last time the output pin was toggled
unsigned long buttondebounceDelay = 50;    // the debounce time; increase if the output flickers

unsigned long buttonMaintainTime = 0;  // the last time the output pin was toggled
bool maintainInitDone = false;
#define BUTTON_MAINTAIN_TIME_MAX 1000

/* ---------------------------------------
    Iterates channels
  --------------------------------------- */
void initScenario() {


  // Specific inits procedure
  switch (scenario) {

    case scenFadeIn:
    case scenFadeOut:
      contourValue.start(fadePeriod);
      volumeValue.start(fadePeriod);
      break;

    case scenStill:
    case scenSin:
      contourValue.start(animPeriod);
      volumeValue.start(animPeriod);
      break;

    case scenWaveA:
      contourValue.start(waveAPeriod);
      volumeValue.start(waveAPeriod);
      break;

    case scenWaveRandom:
      contourValue.start(animPeriod);
      volumeValue.start(animPeriod);
      // Animation section
      waveRandomIndex = getNewIndexwaveRandom();
      break;

    case scenMultiple:
      contourValue.start(animPeriod);
      volumeValue.start(animPeriod);
      // Animation section
      restartMultipleAnims();
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

    float wavePhase = animation::floatMap(idx, 0.0f, _channels.size(), 0.0f, 1.0f);
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
    long newPeriod = random(0.1 * animPeriod, animPeriod);
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

  contoursUpdate();
  volumesUpdate();

}

void contoursUpdate() {

  /* Sending Contours (Static) */
  switch (scenario) {

    case scenFadeIn:
      simple(contoursChannels, contourValue.fadeIn(0, 0, maxBright));
      break;
    case scenStill:
    case scenSin:
    case scenWaveA:
    case scenWaveRandom:
    case scenMultiple:
      // FADE IN and Sinus Lights
      simple(contoursChannels, maxBright);
      break;

    case scenFadeOut:
      // FADE OUT
      simple(contoursChannels, contourValue.fadeOut(0, 0, maxBright));
      break;

    default:
      simple(contoursChannels, maxBright);

  }
}

void volumesUpdate() {

  /* Sending Contours (Static) */
  switch (scenario) {

    case scenFadeIn:
      // FADE IN and still
      simple(volumesChannels, volumeValue.fadeIn(0, 0, maxBright));
      break;

    case scenStill:
      // FADE IN and still
      simple(volumesChannels, maxBright);
      break;

    case scenSin:
      // FADE IN and still
      simple(volumesChannels, volumeValue.loopSin(0, minBright, maxBright));
      break;

    case scenWaveA:
      // Waving A spatially
      waveA(volumesChannels);
      break;

    case scenWaveRandom:
      // Waving B spatially
      waveRandom(volumesChannels);
      break;

    case scenMultiple:
      // FADE OUT
      multiple(volumesChannels);
      break;

    case scenFadeOut:
      // FADE OUT
      simple(volumesChannels, volumeValue.fadeOut(0, 0, maxBright));
      break;

    default:
      simple(volumesChannels, maxBright);

  }
}

void readingButton() {
  // read the state of the switch into a local variable:
  int reading = digitalRead(buttonPin);

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    buttonDebounceTime = millis();
  }

  if ((millis() - buttonDebounceTime) > buttondebounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading != buttonState) {
      buttonState = reading;

      // only toggle the LED if the new button state is HIGH
      if (buttonState == LOW) {
          plusScenario();
      }else{
        maintainInitDone = false;
        buttonMaintainTime = 0;
      }
    }

    // If maintained
    if (buttonState == LOW) {
      if (buttonMaintainTime >= BUTTON_MAINTAIN_TIME_MAX) {
        scenario = scenFadeOut;
        
        if(maintainInitDone == false){
          initScenario();
          maintainInitDone = true;
        }
        /*
        buttonMaintainTime = 0;
        */
      } else {
        buttonMaintainTime = millis() - buttonDebounceTime;
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
    case scenFadeIn: return "Fade in";
    case scenStill: return "Still";
    case scenSin: return "Sin";
    case scenWaveA: return "Wave A";
    case scenWaveRandom: return "Wave Random";
    case scenMultiple: return "Multiple";
    case scenFadeOut: return "Fade Out";
    default: return "No name";
  }
}

void plusScenario() {

  if (scenario == scenFadeOut) {
    scenario = scenFadeIn;

  } else {
    scenario++;
    if (scenario > MAX_SCENARIO) {
      scenario = 1;
    }
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

    int myInputNumber = inputString.toInt();
    if (myInputNumber >= 0 || myInputNumber <= 6) {
      scenario = myInputNumber;
      initScenario();
    }

    // --
    stringComplete = false;
    inputString = "";

  }
}

