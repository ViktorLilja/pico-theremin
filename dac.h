#pragma once

#include "MCP_DAC.h"
#include "waveforms.h"

#define DAC_PIN_CLK   10
#define DAC_PIN_MOSI  11
#define DAC_PIN_MISO  12
#define DAC_PIN_CS    13

// Random 32-bit number for dithering
uint32_t random32 = 42;

// Waveforms are stored as 12 bit (4096) samples with a bit depth of 20.
// The first 12 bits are always zero.
// 20 bits are used for the waveform, even tough the DAC only has an accuracy of 12 bits.
// The extra information is used for noise reduction using dithering.

uint32_t DAC_count = 0;
// This value keeps track of where in the waveform we currently are.

// The first 12 bits determine which sample in the waveform to use.
// The remaining 20 bits are fractional steps.

uint32_t DAC_increment = 0;
// This variable determines the frequency.
// DAC_count will be incremented by this value each step.

// Values larger than 2^12 will mean that we are sometimes
// taking more than one step in the waveform.
// Values larger than 2^31 will mean 
// that we step more than we step more than half a period at a time,
// this is the Nyquist limit.

uint32_t DAC_volume = 0;
// The value of this variable determines the volume.
// The output will be multiplied by this value, and then downshifted 12 bits.

// The value should never exceed 2^12 (4096), as this
// would cause an output larger than the 12 bit resolution of the DAC.

// Example (assuming no dithering)
// If volume is 1 0000 0000 0000 (4096), then the unaltered waveform is played back.
// If volume is 0 0000 0000 0001 (   1), then only most significant bits is used to represet the waveform.
// If volume is 0 0000 0000 0000 (   0), no sound is played.

MCP4921 DAC_dac(255, 255, &SPI1);

void setup1() {
  DAC_dac.setGPIOpins(DAC_PIN_CLK, DAC_PIN_MISO, DAC_PIN_MOSI, DAC_PIN_CS);  // SELECT should match the param of begin()
  DAC_dac.begin(DAC_PIN_CS);
}

// Set the audio volume using a double between 0 and 1.0
void DAC_set_volume(double volume) {
  int val = (0x1000) * volume;
  if (val < 0)       val = 0;
  if (val > 0x1000)  val = 0x1000;
  DAC_volume = val;
}

// Set the audio frequency using a double between 20 and 20000
void DAC_set_frequency(double frequency) {

  // The factor multiplied by here is determined by how many times DAC_write_next 
  // is called per second. The value has been tuned experimentally.
  int inc = 19830 * frequency;
  if (inc < 1)     inc = 1;               // No stalling
  if (inc > 0x80000000) inc = 0x80000000; // No aliasing
  DAC_increment = inc;
}

// Write next value to DAC, should be called repeatedly as only function
// that runs on processor.
uint32_t val_integrated = 0;

const double  k = 1.5; // Valid range: -2.5 to 1.5
                       // Reasonable range: 0.5 to 1.5
                       // Most physical: 1.5 (dampening of high amplitudes)
                       // No distorsion: 1.0
const int32_t A = 4096 * (4 * (1-k));
const int32_t B = 4096 * (6 * (k-1));
const int32_t C = 4096 * (3 - 2 * k);

void loop1() {
  
  // Advance in waveform
  DAC_count += DAC_increment;

  // Read value from waveform table
  uint32_t val = WAVEFORM_selected[DAC_count>>20];

  // Scale by volume
  val *= DAC_volume;

  // Shift so that waveform is centered regardless of volume.
  // This is important to avoid non-linear regions of buffer for
  // low volumes.
  val += (0x00001000-DAC_volume)<<19;

  // Apply distorsion
  /*
  int32_t x = (int32_t) (val>>20);
  int32_t y = 0;
  y += ((((A*x)>>10)*x)>>10)*x;
  y +=   ((B*x)>>8)*x;
  y +=    (C*x)<<4;
  val = (uint32_t) (y<<4);
  */

  // Apply low pass filter to avoid aliasing
  val_integrated += (val>>3) - (val_integrated>>3);
  val = val_integrated;

  // Generate a random number with triangular pdf
  // Use Linear congruential RNG from wikipedia as RNG
  /*
  uint32_t dither = 0;
  random32 = 1664525 * random32 + 1013904223;
  dither += random32>>12;
  random32 = 1664525 * random32 + 1013904223;
  dither += random32>>12;
  
  // Add triangular dither
  val += dither;
  */

  // Reduce bit depth to DAC resolution
  val >>= 20;

  // Send value to DAC
  DAC_dac.fastWriteA(val);
  //DAC_dac.analogWrite(val);
}
