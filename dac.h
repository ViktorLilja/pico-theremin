#pragma once

#include "MCP_DAC.h"
#include "waveforms.h"

#define DAC_PIN_CLK   10
#define DAC_PIN_MOSI  11
#define DAC_PIN_MISO  12
#define DAC_PIN_CS    13

uint16_t DAC_count = 0;
// This 16-bit unsigned integer keeps track of where in the
// waveform we currently are.

// XXXX XXXX XXYY YYYY
// X : These 10 bits tell where in the 1024 sample waveform we are.
// Y : These 6 bits keep track of how far we are to the way to the
//     next sample. They are required for low notes.

uint32_t DAC_increment = 0;
// The value of this variable determines the frequency.
// DAC_count will be incremented by this value each step.
// Values larger than 0100 0000 (64) will mean that we are sometimes
// taking more than one step in the waveform.
// Values larger than 1000 0000 0000 0000 (65536) will mean 
// that we step more than we step more than half a period at a time,
// this is the Nyquist limit.

uint32_t DAC_volume = 0;
// The value of this variable determines the volume.
// The output will be multiplied by this value, and then downshifted 11 bits.
// The value should never exceed 1000 0000 0000 (2048), as this
// would cause an output larger than the 12 bit resolution of the DAC.

// Example
// If volume is 1000 0000 0000 (2048), then the unaltered waveform is played back.
// If volume is 0000 0000 0001 (   1), then only most significant bits is used to represet the waveform.
// If volume is 0000 0000 0000 (   0), no sound is played.

MCP4921 DAC_dac(255, 255, &SPI1);

void DAC_setup() {
  DAC_dac.setGPIOpins(DAC_PIN_CLK, DAC_PIN_MISO, DAC_PIN_MOSI, DAC_PIN_CS);  // SELECT should match the param of begin()
  DAC_dac.begin(DAC_PIN_CS);
}

// Set the audio volume using a double between 0 and 1.0
void DAC_set_volume(double volume) {
  int val = (2048) * volume;
  if (val < 0)      val = 0;
  if (val > 2048)   val = 2048;
  DAC_volume = val;
}

// Set the audio frequency using a double between 20 and 20000
void DAC_set_frequency(double frequency) {

  // The factor multiplied by here is determined by how many times DAC_write_next 
  // is called per second. The value has been tuned experimentally.
  int inc = 0.3036 * frequency;
  if (inc < 1)     inc = 1;
  if (inc > 65536) inc = 65536;
  DAC_increment = inc;
}

// Write next value to DAC, should be called repeatedly as only function
// that runs on processor.
void DAC_write_next() {

  // Advance in waveform
  DAC_count += DAC_increment;

  // Read value from waveform table
  uint32_t val = (2048 + pgm_read_word_near(WAVEFORM_selected + (DAC_count>>6)));

  // Scale by volume
  val *= DAC_volume;
  val >>= 12;
  
  // Send value to DAC
  DAC_dac.analogWrite(val);
}
