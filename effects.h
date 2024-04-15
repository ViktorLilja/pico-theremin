#pragma once

#include "tones.h"

bool EFX_autotune = false;
int EFX_vibrato  = 0;

// Apply effect to frequency and volue
void EFX_apply_effects(double *frequency, double *volume) {

  if (EFX_autotune) {
    int tone_number = closest_tone(*frequency);
    double closest_frequency = frequency_of_tone(tone_number);
    double delta = closest_frequency - *frequency;
    *frequency += delta;
  }

  if (EFX_vibrato) {
    double delta = 0.02 * *frequency * sin(EFX_vibrato * 6.28 * micros() / 1e6);
    *frequency += delta;
  }
}
