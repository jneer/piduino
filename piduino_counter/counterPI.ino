/*
  CounterPI
  =========
  - a dither lock for a Fabry-Perot resonator, using APD counts
    in transmission as the locking signal.
  - developed by Jonas S. Neergaard-Nielsen at DTU, Denmark in 
    summer 2012, based on an earlier parametric gain (analog 
    detector signal) phase lock developed at NICT, Tokyo.

  Using the Frequency Counter lib by Martin Nawrath 
  http://interface.khm.de/index.php/lab/experiments/arduino-frequency-counter-library/
  
  v0.3
  ----
   * comments improved
   * unused commands and constants removed
   
 */
 
// #include <Arduino.h>           // probably not needed anymore - I forgot why it's there!
#include <FreqCounter.h>          // read counter signals
#include <Wire.h>                 // communicate with the DAC via I²C (two-wire interface)
#include <EEPROM.h>               // write and read on the non-volatile memory
#include "counterPI.h"            // header file with constant definitions

long cycle = 0;                   // program cycle counter
boolean debug = true;             // send debugging info over serial - may be removed later
int updatePeriod = 1;             // how often to output debugging/lock state info

boolean locking = 0;              // locking toggled through remote control (additional to hardware switch)

unsigned int countingTime = 2;    // interval for counting [ms]
int cnt;                          // counter value
unsigned long cntHist;            // integrated counts

unsigned int sinePeriodms = 50;   // modulation period [ms]
long sinePeriod = 50000;          // modulation period [us]
int sineAmplitude = 1000;         // modulation amplitude [bits]
int sineLUT[SINE_LENGTH];         // look-up table for modulation sine waveform
long demodLPA = 25;               // low-pass alpha (on a scale from 0 to DEMOD_LPA_MAX) for demodulation
int demodPhase = 0;               // demodulation phase [LUT index]

long err = 0;                     // error signal
long errInt = 0;                  // integrated error signal

char polarity = 1;                // lock polarity
int aoBias;                       // output bias voltage
int gainP;                        // proportional (P) gain
int gainI;                        // integral (I) gain
int reduction;                    // scaling factor to limit the gain

unsigned long twPeriod = 20000;   // triangle wave period [us]
unsigned int twAmplitude = 2047;  // triangle wave amplitude [bits]
unsigned int twBias = 2047;       // triangle wave offset [bits]

int out;                          // output digital voltage

byte regist[25];                  // parameter memory array



void setup() { 
  buildSine();                    // construct sine waveform LUT
  _setup();                       // various setup commands moved to counterPI_setup.ino
}



void loop() {
  cycle++;
  parameterUpdate();              // check for incoming parameter update commands on serial
  
  if (!digitalRead(pinLockMode))  // LOW when mode switched to locking
  {
    if (!digitalRead(pinGate))    // check for gate signal
                                  // NB!!! should be inverted if gate signal is applied (remove !)
    {  // lock mode and gate on
      cnt = doCount();            // get count value
      out = getFB(cnt);           // calculate feedback and total output signal, send debug info
      writeDAC(out);              // send output signal to DA converter
    }
    else
    {  // lock mode but in measurement mode - print information
      //printExtra();               // not implemented yet
    }
  }
  
  else if (!digitalRead(pinAltMode)) 
  {  // triangle wave (alternative) mode
    out = getTW();                // generate triangle waveform
    writeDAC(out);                // send triangle signal to DA converter
  }
  else
  
  {  // idle mode - print information
    cnt = doCount();
    out = getFB(cnt);
    reportIdle(cnt, out);         // write status and feedback information on serial connection
    delay(500);
  }
  
}
