

// store gain, polarity, set point and bias settings in non-volatile memory
void writeEEPROM() {
  regist[0] = byte(gainP / 256);
  regist[1] = byte(gainP % 256);
  regist[2] = byte(gainI / 256);
  regist[3] = byte(gainI % 256);
  regist[4] = byte(gainT / 256);
  regist[5] = byte(gainT % 256);
//  regist[6] = char(polarity / 256); // char is a signed byte
  regist[7] = char(polarity);
  regist[8] = byte(setpoint / 256);
  regist[9] = byte(setpoint % 256);
  regist[10] = byte(aoBias / 256);
  regist[11] = byte(aoBias % 256);
  regist[12] = char(outputMode);
  regist[13] = byte(modDepth / 256);
  regist[14] = byte(modDepth % 256);
  regist[15] = byte(demodPhase / 256);
  regist[16] = byte(demodPhase % 256);
  for (int i = 0; i <= 16; i++) {
    EEPROM.write(i, regist[i]);
  }
}


// read gain, polarity, set point and bias settings from non-volatile memory
void readEEPROM() {
  for (int i = 0; i <= 16; i++) {
    regist[i] = EEPROM.read(i);
  }
  gainP = 256 * regist[0] + regist[1];
  gainI = 256 * regist[2] + regist[3];
  gainT = 256 * regist[4] + regist[5];
//  polarity = 256 * regist[6] + regist[7];
  polarity = regist[7];
  setpoint = 256 * regist[8] + regist[9];
  aoBias = 256 * regist[10] + regist[11];
  outputMode = regist[12];
  modDepth = 256 * regist[13] + regist[14];
  demodPhase = 256 * regist[15] + regist[16];
}
