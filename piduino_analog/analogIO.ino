// Interrupt call function
void readwrite() {
  triggered = true;
}


// Write 12-bit data value to MCP4725 DAC and update its output
void writeDAC(int data) {
  Wire.beginTransmission(DAC_WRITE);
    Wire.write(DAC_MODE | data >> 8); // higher 4 bits
    Wire.write(0xFF & data);          // lower 8 bits (& operator truncates to length of shortest input, here FF)
  Wire.endTransmission();
}


// Analog output wrapper
void updateAo() {
  unsigned int ao;
  if (outputMode == 0) {
    ao = aoBias;
  } else if (outputMode == 1) {
    ao = aoTriangle();
  } else if (outputMode == 2) {
    if (ditherLocking) {
      ao = aoFBDemod() + aoMod();   // dither locking
    } else {
      ao = aoFBDirect();            // direct locking
    }
  }
  writeDAC(ao);
}


// Modulation signal
long aoMod() {
  unsigned int ao;
  ao = sineLUT[micros() % MOD_PERIOD >> MOD_PERIOD_EXP - SINE_LENGTH_EXP];
  return ao;
}
  
  
// Demodulation and lock
long aoFBDemod() {
  unsigned int ao;
  long demod;
  int error;
  int fb;
  
  demod = aiVal * sineLUT[(micros() - demodPhase) % MOD_PERIOD >> MOD_PERIOD_EXP - SINE_LENGTH_EXP];
  demodHist = DEMOD_LOWPASS_ALPHA * demod + ((1<<DEMOD_LOWPASS_SIZE) - DEMOD_LOWPASS_ALPHA) * demodHist >> DEMOD_LOWPASS_SIZE;
  
  errorInt += demodHist;
  fb = polarity * (gainP * demodHist + gainI * errorInt) << (RES_DA - RES_AD) >> gainT;
  ao = aoBias + fb;
  
  if (ao <= MIN_DA || ao >= MAX_DA) {
    errorInt = 0;                  // relock
  }

  return constrain(ao, MIN_DA, MAX_DA);
}  

// Direct locking (use analog input minus setpoint as error)
long aoFBDirect() {
  unsigned int ao;
  long demod;
  int error;
  int fb;
  
  error = aiVal - setpoint;
  errorInt += error;
  fb = polarity * (gainP * error + gainI * errorInt) << (RES_DA - RES_AD) >> gainT;
  ao = aoBias + fb;
  
  if (ao <= MIN_DA || ao >= MAX_DA) {
    errorInt = 0;                  // relock
  }

  return constrain(ao, MIN_DA, MAX_DA);
}


long aoTriangle() {
  unsigned int ao;
  unsigned long twPhase;
  twPhase = micros() % TW_PERIOD;
  if (twPhase < (TW_PERIOD >> 1)) {
    ao = TW_BIAS - (TW_AMPLITUDE >> 1) + (twPhase >> (TW_PERIOD_EXP - TW_AMPLITUDE_EXP - 1)); // 2 * twAmplitude (2^12) / twPeriod (2^15) = 2
  } else {
    ao = TW_BIAS + (3 * TW_AMPLITUDE >> 1) - (twPhase >> (TW_PERIOD_EXP - TW_AMPLITUDE_EXP - 1));
  }
  return constrain(ao, MIN_DA, MAX_DA);;  
}


// Rebuild sine lookup table
void buildSine(int len, int depth) {
  int i;
  for (i = 0; i < len; i++) {
    sineLUT[i] = depth * sin(2*3.1416*i/(float)len);
  }
}

