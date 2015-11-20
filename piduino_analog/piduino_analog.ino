/*
  NOTES:
  For fast calculation, use int instead of long whenever possible.
  Think carefully about multiplication and division - sometimes they can be combined
  into a single bitshift.
  Choose powers of 2 as factors, e.g. triangle wave period.
  Sine takes forever to calculate (probably because of float). Use look-up table instead.
  Using an int array longer than about 1000 clogs up the device. Limit to 512 length or so.
  
  TO DO:
  - clean up parameter memory
  - implement (and solder) diode for high or low input voltages
  
  VERSION HISTORY:
  version 1.2, 2012-03-19
    - upgraded to Arduino 1.0: WProgram.h -> Arduino.h,
                               Wire.send() -> Wire.write()
  version 1.1, 2011-12-01
    - added direct locking mode
    - added scaling of output with resolution difference between AD and DA
      for 1:1 correspondence between input and output for P=1,T=0
  version 1.0
    - identical to PIduino_dither_beta2
    - in use in teleportation experiment at NICT
*/

#include <Wire.h>
#include <EEPROM.h>
#include "piduino_analog.h"

// Dither locking (true) or direct fringe locking (false)?
boolean ditherLocking = true;

// PI parameters
char outputMode;
int gainP;
int gainI;
int gainT; // total gain
char polarity;
int setpoint;
int aoBias;  // analog output bias
int modDepth;
int demodPhase;

byte regist[17];

int aiVal;      // analog input; process variable
long errorInt = 0;
long demodHist = 0;

volatile boolean triggered = false;
int buttonPrev = HIGH;
int clickTime = 0;

int sineLUT[SINE_LENGTH];

  
// initial setup
void setup() {
  Wire.begin();                          // initiate I2C communication with DAC
  setRegisters();
  Serial.begin(115200);                 // initiate serial communication with PC
  
  pinMode(PIN_DEBUG, OUTPUT);             // LED for general debugging usage
  pinMode(PIN_BUTTON, INPUT);

  readEEPROM();                          // recall saved settings
  
  attachInterrupt(INTERRUPT, readwrite, RISING); // interrupt trigger on pin 2 (interrupt 0)

  buildSine(SINE_LENGTH, modDepth);       // build sine function lookup table for modulation
}


// main loop
void loop() {

  parameterUpdate();            // check for incoming serial commands
  modeCheck(false);             // check for mode button press, use true for simple (2 state) switch, false for 4 state
  
  if (triggered) {
    aiVal = analogRead(PIN_AI);  // sample and hold 1.5 ADC clock cycles after initiation
    updateAo();                 // calculate and output analog signal
    triggered = false;
  }
}





