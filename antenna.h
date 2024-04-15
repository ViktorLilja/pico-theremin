#pragma once

#include "RP2040_PWM.h"

// Theremin v1
/*
#define ANT_PIN_PWM_PITCH     16
#define ANT_PIN_SIGNAL_PITCH  17
#define ANT_BASE_FREQ_PITCH   460000
#define ANT_PIN_PWM_VOLUME    15
#define ANT_PIN_SIGNAL_VOLUME 14
#define ANT_BASE_FREQ_VOLUME  500000
*/

// Theremin v2
#define ANT_PIN_PWM_PITCH     17
#define ANT_PIN_SIGNAL_PITCH  16
#define ANT_BASE_FREQ_PITCH   570000
#define ANT_PIN_PWM_VOLUME    14
#define ANT_PIN_SIGNAL_VOLUME 15
#define ANT_BASE_FREQ_VOLUME  520000



const double ANT_freq_offset = 10000; // Working frequency of antennas

// Initialize volume antenna
RP2040_PWM* ANT_pwm_volume;
void ANT_isr_volume();
double             ANT_pwm_freq_volume  = ANT_BASE_FREQ_VOLUME + ANT_freq_offset;
volatile uint32_t  ANT_micros_volume = 0;
volatile uint32_t  ANT_period_volume = 0;
double             ANT_zero_freq_volume = 0;

// Initialize pitch antenna
RP2040_PWM* ANT_pwm_pitch;
void  ANT_isr_pitch();
double             ANT_pwm_freq_pitch   = ANT_BASE_FREQ_PITCH + ANT_freq_offset;
volatile uint32_t  ANT_micros_pitch = 0;
volatile uint32_t  ANT_period_pitch = 0;
double             ANT_zero_freq_pitch = 0;


void ANT_setup() {

  // Set pin modes for antennas
  pinMode(ANT_PIN_PWM_PITCH,     OUTPUT);
  pinMode(ANT_PIN_SIGNAL_PITCH,  INPUT);
  pinMode(ANT_PIN_PWM_VOLUME,    OUTPUT);
  pinMode(ANT_PIN_SIGNAL_VOLUME, INPUT);

  // Start PWM
  ANT_pwm_volume = new RP2040_PWM(ANT_PIN_PWM_VOLUME, ANT_pwm_freq_volume, 50);
  ANT_pwm_pitch  = new RP2040_PWM(ANT_PIN_PWM_PITCH, ANT_pwm_freq_pitch, 50);
  if (ANT_pwm_volume) ANT_pwm_volume->setPWM();
  if (ANT_pwm_pitch)  ANT_pwm_pitch->setPWM();

  // Start listening to signal from antennas
  attachInterrupt(digitalPinToInterrupt(ANT_PIN_SIGNAL_VOLUME), ANT_isr_volume, RISING);
  attachInterrupt(digitalPinToInterrupt(ANT_PIN_SIGNAL_PITCH),  ANT_isr_pitch, RISING);
}

// Interrupt routine for signals from volume antenna
void ANT_isr_volume() {
  uint32_t current_micros = micros();
  ANT_period_volume += (current_micros-ANT_micros_volume<<16) - (ANT_period_volume>>8);
  ANT_micros_volume = current_micros;
}

// Interrupt routine for signals from pitch antenna
void ANT_isr_pitch() {
  uint32_t current_micros = micros();
  ANT_period_pitch += (current_micros-ANT_micros_pitch<<16) - (ANT_period_pitch>>8);
  ANT_micros_pitch = current_micros;
}

double ANT_get_freq_volume() {
  return (65536*256*1e6)/ANT_period_volume - ANT_zero_freq_volume;
}

double ANT_get_freq_pitch() {
  return (65536*256*1e6)/ANT_period_pitch - ANT_zero_freq_pitch;
}

void ANT_calibrate() {

    // Reset zero frequency
    ANT_zero_freq_volume = 0;
    ANT_zero_freq_pitch = 0;
 
    // Get new measurement
    delay(500);

    // Update pwm frequency to get desired base frequency
    ANT_pwm_freq_volume += -ANT_get_freq_volume() + ANT_freq_offset;
    ANT_pwm_freq_pitch  += -ANT_get_freq_pitch()  + ANT_freq_offset;
    ANT_pwm_volume->setPWM(ANT_PIN_PWM_VOLUME, ANT_pwm_freq_volume, 50);
    ANT_pwm_pitch->setPWM(ANT_PIN_PWM_PITCH,   ANT_pwm_freq_pitch,  50);

    // Get new measurement
    delay(500);
    
    // Update zero point frequency
    ANT_zero_freq_volume = ANT_get_freq_volume();
    ANT_zero_freq_pitch  = ANT_get_freq_pitch();
}
