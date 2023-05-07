#include "RP2040_PWM.h"

#define ANT_PIN_PWM_PITCH     16
#define ANT_PIN_SIGNAL_PITCH  17
#define ANT_PIN_PWM_VOLUME    15
#define ANT_PIN_SIGNAL_VOLUME 14

RP2040_PWM* ANT_pwm_volume;
void ANT_isr_volume();
int     ANT_pwm_freq_volume     = 520000; // Should be roughly 20kHz above antenna oscillation frequency
int     ANT_last_micros_volume  = 0;
double  ANT_period_volume       = 0;
double  ANT_zero_freq_volume    = 20000;
double  ANT_freq_volume         = 0;

RP2040_PWM* ANT_pwm_pitch;
void ANT_isr_pitch();
int     ANT_pwm_freq_pitch      = 480000; // Should be roughly 20kHz above antenna oscillation frequency
int     ANT_last_micros_pitch   = 0;
double  ANT_period_pitch        = 0;
double  ANT_zero_freq_pitch     = 24000;
double  ANT_freq_pitch          = 0;

const int    ANT_freq_offset      = 5000; // Working frequency of antennas
const double ANT_filter_constant  = 0.01;  // Response time will be approx 1/(offset * constant) 




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
  int current_micros = micros();

  // Should never oscillate slower than 1kHz
  if (current_micros - ANT_last_micros_pitch > 1000) {
    ANT_last_micros_volume = current_micros;
    return;
  }

  // Simple low pass filter
  ANT_period_volume = (1-ANT_filter_constant) * ANT_period_volume + 
                      ANT_filter_constant * (current_micros - ANT_last_micros_volume);
                      
  ANT_freq_volume = 1e6 / ANT_period_volume;
  
  ANT_last_micros_volume = current_micros;
}

// Interrupt routine for signals from pitch antenna
void ANT_isr_pitch() {
  int current_micros = micros();

  // Should never oscillate slower than 1kHz
  if (current_micros - ANT_last_micros_pitch > 1000) {
    ANT_last_micros_pitch = current_micros;
    return;
  }

  // Simple low pass filter
  ANT_period_pitch = (1-ANT_filter_constant) * ANT_period_pitch + ANT_filter_constant * (current_micros - ANT_last_micros_pitch);

  ANT_freq_pitch = 1e6 / ANT_period_pitch;
  
  ANT_last_micros_pitch = current_micros;
}

// Call this to calibrate the antenna frequencies
void ANT_calibrate() {
  
    // Wait for new measurement
    delay(2500);

    // Update pwm frequency
    ANT_pwm_freq_volume = ANT_pwm_freq_volume - ANT_freq_volume + ANT_freq_offset;
    ANT_pwm_freq_pitch  = ANT_pwm_freq_pitch  - ANT_freq_pitch  + ANT_freq_offset;
    ANT_pwm_volume->setPWM(ANT_PIN_PWM_VOLUME, ANT_pwm_freq_volume, 50);
    ANT_pwm_pitch->setPWM(ANT_PIN_PWM_PITCH,   ANT_pwm_freq_pitch,  50);

    // Wait for new measurement
    delay(500);
    
    // Set Zero point frequency
    ANT_zero_freq_volume = ANT_freq_volume;
    ANT_zero_freq_pitch  = ANT_freq_pitch;
    
}
