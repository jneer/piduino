/*
  CounterPI > counterPI_parametermemory.ino
  -----------------------------------------
  Read/write parameters to/from EEPROM memory.
*/


// store gain, polarity, and all the other lock settings in non-volatile memory
void writeEEPROM() {
  regist[0] = byte(gainP / 256);
  regist[1] = byte(gainP % 256);
  regist[2] = byte(gainI / 256);
  regist[3] = byte(gainI % 256);
  regist[4] = byte(reduction / 256);
  regist[5] = byte(reduction % 256);
  regist[6] = char(polarity);
  regist[7] = byte(aoBias / 256);
  regist[8] = byte(aoBias % 256);
  regist[9] = byte(sinePeriodms / 256);
  regist[10] = byte(sinePeriodms % 256);
  regist[11] = byte(sineAmplitude / 256);
  regist[12] = byte(sineAmplitude % 256);
  regist[13] = byte(demodPhase / 256);
  regist[14] = byte(demodPhase % 256);
  regist[15] = byte(demodLPA / 256);
  regist[16] = byte(demodLPA % 256);
  regist[17] = byte(countingTime / 256);
  regist[18] = byte(countingTime % 256);
  regist[19] = byte(updatePeriod / 256);
  regist[20] = byte(updatePeriod % 256);
  regist[21] = byte(debug);
  for (int i = 0; i <= 21; i++) {
    EEPROM.write(i, regist[i]);
  }
}


// read gain, polarity, and all the other lock settings from non-volatile memory
void readEEPROM() {
  for (int i = 0; i <= 18; i++) {
    regist[i] = EEPROM.read(i);
  }
  gainP = 256 * regist[0] + regist[1];
  gainI = 256 * regist[2] + regist[3];
  reduction = 256 * regist[4] + regist[5];
  polarity = regist[6];
  aoBias = 256 * regist[7] + regist[8];
  sinePeriodms = 256 * regist[9] + regist[10];
  sinePeriod = 1000 * sinePeriodms;
  sineAmplitude = 256 * regist[11] + regist[12];
  demodPhase = 256 * regist[13] + regist[14];
  demodLPA = 256 * regist[15] + regist[16];
  countingTime = 256 * regist[17] + regist[18];
  updatePeriod = 256 * regist[19] + regist[20];
  debug = (boolean)regist[21];
}
