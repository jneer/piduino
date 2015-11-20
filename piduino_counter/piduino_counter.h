/*
  CounterPI > counterPI.h
  -----------------------
  Constant definitions.
*/

#define pinGate 2      // gating input signal
#define pinLockMode 7  // switch up: locking on
#define pinAltMode 8   // switch down: triangle wave on
#define pinLed 13      // debugging LED
                       // counter input is on pin 5
           
           
#define RES_DA 12              // DAC resolution
#define MIN_DA 0               // DAC min value
#define MAX_DA 4095            // DAC max value
#define SINE_LENGTH 256        // max length of sine LUT
#define SINELUT_AMPLITUDE 256  // resolution of sine LUT
#define DEMOD_LPA_MAX 1000     // scale of demodulation low-pass alpha value

#define SERIAL_TIMEOUT 10      // serial communication timeout in ms


// two-wire interface control bytes - these apply to MCP4725 - for details, read its datasheet
#define DAC_WRITE (0xC << 3 | 0x0)  // device code 1100 + address 000 + write bit
#define DAC_READ (0xC << 3 | 0x1)   // device code 1100 + address 000 + read bit
#define DAC_MODE (0x0 << 4)         // fast mode 00 + no power down 00


