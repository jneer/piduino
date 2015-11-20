//#include "WProgram.h"
#include "Arduino.h"


#define PIN_AI 0            // analog input
#define PIN_BUTTON 7           // momentary switch
#define INTERRUPT 0  // 0 = Arduino D2, 1 = Arduino D3
#define PIN_DEBUG 13

#define RES_AD 10     // ADC resolution
#define MIN_AD 0      // ADC (built-in) min value
#define MAX_AD 1023   // ADC (built-in) max value
#define RES_DA 12     // DAC resolution
#define MIN_DA 0      // DAC min value
#define MAX_DA 4095   // DAC max value

// triangle wave parameters
#define TW_PERIOD_EXP 19
#define TW_PERIOD ((long)1<<TW_PERIOD_EXP)         // period of full sawtooth scan in us
#define TW_AMPLITUDE_EXP 12
#define TW_AMPLITUDE (1<<TW_AMPLITUDE_EXP)   // full DAC output scale = 2^twAmplitudeExp
#define TW_BIAS (MAX_DA>>1)                  // offset of triangle waveform

// sine modulation parameters
#define SINE_LENGTH_EXP 8
#define MOD_PERIOD_EXP 11
#define SINE_LENGTH (1<<SINE_LENGTH_EXP)   // length of sine function LUT - takes up valuable space in RAM!
#define MOD_PERIOD (1<<MOD_PERIOD_EXP)   // modulation sine wave period in us, e.g. 1<<11 = 2048 us ~ 500 Hz

#define DEMOD_LOWPASS_SIZE 3   // exponent for lowpass filter
#define DEMOD_LOWPASS_ALPHA 2  // lowpass alpha:  out[k] = (alpha * in  +  (2^size - alpha) * out[k-1]) / 2^size

#define SERIAL_TIMEOUT 10  // serial communication timeout in ms
#define BUTTON_WAIT 200    // idle time for button press in ms

// allowed parameter ranges
#define MIN_GAIN 0
#define MAX_GAIN 99
#define MIN_GAINSHIFT 0
#define MAX_GAINSHIFT 99
#define MIN_SETPOINT 0
#define MAX_SETPOINT (1<<RES_AD)-1
#define MIN_BIAS 0
#define MAX_BIAS (1<<RES_DA)-1

// two-wire interface control bytes - these apply to MCP4725
#define DAC_WRITE (0xC << 3 | 0x0)  // device code 1100 + address 000 + write bit
#define DAC_READ (0xC << 3 | 0x1) // device code 1100 + address 000 + read bit
#define DAC_MODE (0x0 << 4) // fast mode 00 + no power down 00


void setRegisters() {
  // set two-wire (I2C) bit rate register; minimum is 10. 
  // 72 for normal 100kbps operation, 12 for 400kbps
  //#define I2C_RATE 12  
  TWBR = 12;                       // set two-wire (I2C) comm. speed
  
  // Set ADC clock divisor (ADCSRA register)
  bitWrite(ADCSRA, ADPS2, 1);
  bitWrite(ADCSRA, ADPS1, 0);
  bitWrite(ADCSRA, ADPS0, 0);
  /*
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
}
