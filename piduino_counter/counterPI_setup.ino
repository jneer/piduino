/*
  CounterPI > counterPI_setup.ino
  -------------------------------
  Initial device setup.
*/


void _setup() {
  // set two-wire (I2C) bit rate register; minimum is 10. 
  //   72 for normal 100kbps operation, 12 for 400kbps
  //   according to formula: SCL Frequency = CPU Clock Frequency / (16 + (2 * TWBR))
  // for reference:
  //   http://arduino.cc/playground/Main/WireLibraryDetailedReference#registers
  //   http://electronics.stackexchange.com/questions/29457/how-to-make-arduino-do-high-speed-i2c
  //   http://arduino.cc/playground/Code/ATMELTWI#registers
  // - there may be ways to further enhance the speed
  TWBR = 12;

  // Set ADC clock divisor (ADCSRA register)
  // - we're changing this from the default 128 (125 kHz) to 16 (1 MHz)
  bitWrite(ADCSRA, ADPS2, 1);
  bitWrite(ADCSRA, ADPS1, 0);
  bitWrite(ADCSRA, ADPS0, 0);
  /*
  From ATmega328 data sheet, section 23.4:
  ADPS 2-1-0   clock divisor (16 MHz/#)
       0 0 0       2
       0 0 1       2
       0 1 0       4
       0 1 1       8
       1 0 0      16 (1 MHz - conversion time (13 cycles) 13 us)
       1 0 1      32
       1 1 0      64
       1 1 1     128 (default)
  */
  

  // set pull-up resistors on mode switch digital input pins
  pinMode(pinLockMode, INPUT);
  digitalWrite(pinLockMode, HIGH);
  pinMode(pinAltMode, INPUT);
  digitalWrite(pinAltMode, HIGH);

  Wire.begin();                      // initiate two-wire interface
  
  readEEPROM();                      // recall saved settings

  Serial.begin(115200);              // connect to the serial port
}
