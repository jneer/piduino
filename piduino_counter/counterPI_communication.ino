/*
  CounterPI > counterPI_communication.ino
  ---------------------------------------
  Serial communication and lock parameter update functions.
*/


// read a specified number of bytes from serial and convert to integer
int readVal(int bytes) {
  char commval[bytes];
  
  long time0 = millis();
  while (true) {
    if (Serial.available() >= bytes) {   // does the buffer contain the specified number of bytes?
      break;
    } else if (millis() - time0 > SERIAL_TIMEOUT) {
      return -1;
    }
  }
    
  for (int i = 0; i < bytes; i++) {
    commval[i] = Serial.read();   // read bytes one by one
  }
  Serial.flush();                 // may actually not be necessary since Arduino 1.0 - should be tested without!
  
  return atoi(commval);
}



// read commands on serial and update lock parameters accordingly
void parameterUpdate() {
  if (Serial.available() > 0) {        // any data in the serial buffer?
    char command = Serial.read();      // read the command type in the first byte of the buffer
    int val;
    
    switch (command) {                 // read value and update parameter depending on command type
      case 'P':  // change P-gain
        val = readVal(2);
        gainP = val;
        break;
        
      case 'I':  // change I-gain
        val = readVal(2);
        gainI = val;
        break;
        
      case 'T':  // change total gain reduction divisor
        val = readVal(4);
        reduction = val;
        break;
        
      case 'L':  // toggle polarity
        polarity *= -1;
        break;
                
      case 'B':  // change output bias
        val = readVal(4);
        aoBias = constrain(val, MIN_DA, MAX_DA);
        break;
        
      case 'A':  // change modulation depth/amplitude
        val = readVal(4);
        sineAmplitude = constrain(val, 0, MAX_DA / 2);
        break;
        
      case 'p':  // change modulation period (in ms)
        val = readVal(4);
        sinePeriodms = val;
        sinePeriod = 1000 * (long)sinePeriodms;
        break;

      case 'D':  // change demodulation phase shift (in LUT indices)
        val = readVal(4);
        demodPhase = constrain(val, 0, SINE_LENGTH);
        break;
        
      case 'a':  // change demodulation lowpass alpha
        val = readVal(4);
        demodLPA = val;
        break;
        
      case 'l':  // toggle locking
        locking = 1 - locking;
        break;
      
      case 'c':  // change counting time interval
        val = readVal(2);
        countingTime = val;
        break;
        
      case 'C':  // clear accumulated error, i.e. reset I
        errInt = 0;
        break;
        
      case 'u':  // change update period for debug info printing to serial
        val = readVal(4);
        updatePeriod = val;
        break;
        
      case 'd':  // toggle debugging
        debug = !debug;
        break;
      
      case 'E':  // write settings to EEPROM
        writeEEPROM();
        break;  
        
      case 'Q':  // query settings; calling program must read the following serial stream
        reportParameters();
        break;
    }
  }
}



// write lock settings string to serial
void reportParameters() {
  Serial.print("P");
  Serial.print(gainP);
  Serial.print(" I");
  Serial.print(gainI);
  Serial.print(" T");
  Serial.print(reduction);
  Serial.print(" pol");
  Serial.print((int)polarity);  // polarity is char, so must be converted for pretty printing
  Serial.print(" b");
  Serial.print(aoBias);
  Serial.print(" modP");
  Serial.print(sinePeriod);
  Serial.print(" modA");
  Serial.print(sineAmplitude);
  Serial.print(" ph");
  Serial.print(demodPhase);
  Serial.print(" LP");
  Serial.print(demodLPA);
  Serial.print(" ct");
  Serial.print(countingTime);
  Serial.print("\n");
}



// write status info string to serial (when lock is disengaged)
void reportIdle(int cnt, int out) {
  Serial.print(cycle);
  Serial.print(" - counts: ");
  Serial.print(cnt);
  Serial.print(" - feedback: ");
  Serial.println(out);
}



/*
byte* intToBytes(int val) {
  byte bytes[2];
  bytes[0] = (val >> 8);
  bytes[1] = val;
  return bytes;
}

byte* longToBytes(long val) {
  byte bytes[4];
  bytes[0] = val >> 24;
  bytes[1] = val >> 16;
  bytes[2] = val >> 8;
  bytes[3] = val;
  return bytes;
}
*/


// write integer and long to serial byte stream
void intWrite(int val) {
  Serial.write((byte)(val >> 8));
  Serial.write((byte)(val));
}

void longWrite(long val) {
  Serial.write((byte)(val >> 24));
  Serial.write((byte)(val >> 16));
  Serial.write((byte)(val >> 8));
  Serial.write((byte)(val));
}
  


// write status info as numbers on serial string (for debugging and status)
// - the receiving PC client must convert the byte stream to numbers using
//   the same format
void sendValues(int mod, long demod, int fb, int out) {
  intWrite(65535);  // notifier to PC receiver - a bit of a hack, should be improved
  longWrite(cycle);
  intWrite(cnt);
  longWrite(cntHist);
  intWrite(mod);
  longWrite(demod);
  longWrite(err);
  longWrite(errInt);
  intWrite(fb);
  intWrite(out);
}


