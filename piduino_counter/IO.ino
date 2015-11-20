/*
  CounterPI > counterPI_IO.ino
  ----------------------------
  Counter input and analout output functions.
*/


// get count value from counter - locks the device while counting
unsigned long doCount() {
  FreqCounter::f_comp=0;             // calibration value - not so important
  FreqCounter::start(countingTime);  // start counter

  while (FreqCounter::f_ready == 0)  
    cnt = FreqCounter::f_freq;       // read counter value
  
  return cnt;
}



// calculate analog output, including modulation and feedback
//  - optionally print debug/status information to serial
int getFB(int cnt) {
  int mod, fb, ao;
  long demod;
  unsigned long time = micros() % sinePeriod;               // microsecond timer modulo modulation sine period
  int LUTindex = (float)time / sinePeriod * SINE_LENGTH ;   // index the LUT by the current microsecond time
  
  // modulation signal from sine LUT
  mod = ((long)sineAmplitude * (long)sineLUT[LUTindex]) / (long)SINELUT_AMPLITUDE;
  
  // get error signal from lock-in (demodulation) signal
  //  - demod is count value times modulation signal
  //  - err is demod low-pass filtered
  //  - errInt is just the integrated err values
  demod = (long)cnt * (long)sineLUT[(LUTindex + demodPhase) % (long)SINE_LENGTH];
  err = (demodLPA * demod + (DEMOD_LPA_MAX - demodLPA) * err) / DEMOD_LPA_MAX;
  errInt += err;

  // get feedback value from P and I parts
  fb = locking * polarity * (gainP * err + gainI * errInt) / reduction;
  
  // analog output is modulation + feedback + bias voltage
  ao = aoBias + mod + fb;
  if (ao <= MIN_DA || ao >= MAX_DA) errInt = 0;   // relock
  
  // print debug information for every updatePeriod'th cycle
  if (debug && cycle % updatePeriod == 0)
  {
    sendValues(mod, demod, fb, ao);
    cntHist = 0;
  }
  else
  {
    cntHist += cnt;
  }
       
  return constrain(ao, MIN_DA, MAX_DA);
}



// calculate triangle wave analog output
int getTW() {
  int tw;
  unsigned long time = millis() % twPeriod;    // microsecond timer modulo the triangle wave period
  
  // these formulas work...
  if (time < twPeriod/2) {
    tw = twBias - twAmplitude + 4 * twAmplitude * time / twPeriod;
  } else {
    tw = twBias + 3 * twAmplitude - 4 * twAmplitude * time / twPeriod;
  }
  return tw;
}



// Write 12-bit data value to MCP4725 DAC and update its output
void writeDAC(int data) {
  Wire.beginTransmission(DAC_WRITE);
    Wire.write(DAC_MODE | data >> 8); // higher 4 bits
    Wire.write(0xFF & data);          // lower 8 bits (& operator truncates to length of shortest input, here FF)
  Wire.endTransmission();
}



// Rebuild sine lookup table
void buildSine() {
  int i;
  for (i = 0; i < SINE_LENGTH; i++) {
    sineLUT[i] = SINELUT_AMPLITUDE * sin(2*3.1416*i/SINE_LENGTH);
  }
}
