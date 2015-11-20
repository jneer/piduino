int readVal(int bytes) {
  char commval[bytes];
  
  long time0 = millis();
  while (true) {
    if (Serial.available() >= bytes) {
      break;
    } else if (millis() - time0 > SERIAL_TIMEOUT) {
      return -1;
    }
  }
    
  for (int i = 0; i < bytes; i++) {
    commval[i] = Serial.read();
  }
  Serial.flush();
  return atoi(commval);
}



void parameterUpdate() {
  if (Serial.available() > 0) {
    char command = Serial.read();
    int val;
    
    switch (command) {
      case 'P':  // change P-gain
        val = readVal(2);
        if (val != -1) {
          gainP = val;
        } else {
          Serial.println("Timeout!");
        }
        break;
      case 'I':  // change I-gain
        val = readVal(2);
        gainI = val;
        break;
      case 'T':  // change total gain divisor
        val = readVal(2);
        gainT = val;
        break;
      case 'L':  // toggle polarity
        polarity *= -1;
        break;
      case 'S':  // change set point
        val = readVal(4);
        setpoint = constrain(val, MIN_SETPOINT, MAX_SETPOINT);
        break;
      case 'D':  // change demodulation phase shift (in us)
        val = readVal(4);
        demodPhase = constrain(val, 0, MOD_PERIOD);
        break;
      case 'B':  // change output bias
        val = readVal(4);
        aoBias = constrain(val, MIN_BIAS, MAX_BIAS);
        break;
      case 'A':  // change modulation depth/amplitude and rebuild sine LUT
        val = readVal(4);
        modDepth = constrain(val, 0, MAX_DA >> 1);
        buildSine(SINE_LENGTH, modDepth);
        break;
      case 'C':  // clear accumulated error, i.e. reset I
        errorInt = 0;
        break;
      case 'E':  // write settings to EEPROM
        writeEEPROM();
        break;  
      case 'M':  // toggle output mode: {passive, triangle wave, lock}
        outputMode = (outputMode + 1) % 3;
        break; 
      case 'Q':  // query settings, calling program must read the following serial stream
        reportParameters();
        break;
    }
  }
}


void reportParameters() {
  Serial.print("P");
  Serial.print(gainP);
  Serial.print(" I");
  Serial.print(gainI);
  Serial.print(" T");
  Serial.print(gainT);
  Serial.print(" pol");
  Serial.print((int)polarity);  // polarity is char, so must be converted for pretty printing
  Serial.print(" sp");
  Serial.print(setpoint);
  Serial.print(" mod");
  Serial.print(modDepth);
  Serial.print(" ph");
  Serial.print(demodPhase);
  Serial.print(" b");
  Serial.print(aoBias);
  Serial.print(" M");
  Serial.print((int)outputMode);
  Serial.print("\n");
}


void modeCheck(boolean simple) {
  int buttonState = digitalRead(PIN_BUTTON);
  if (buttonState == LOW && buttonPrev == HIGH) {
    int now = millis();
    if (now - clickTime > BUTTON_WAIT) { 
      if (simple) {                              // switch between lock and triangle
        if (outputMode == 1) {
          outputMode = 2;
        } else {
          outputMode = 1;
        }
      } else {                                   // switch idle -> triangle -> lock+ -> lock- ->
        if (polarity == 1 && outputMode == 2) {
          polarity = -1;
        } else {
          outputMode = (outputMode + 1) % 3;
          polarity = 1;
        }
      }
      clickTime = now;
    }
  }
  buttonPrev = buttonState;
}


