#include "lcd.h"
#include "encoder.h"
#include "antenna.h"
#include "dac.h"
#include "gui.h"
#include "effects.h"

// First core handles all other peripherals
void setup() {
  LCD_setup();
  ENC_setup();
  ANT_setup();
  GUI_setup();
}

int freq_update_count = 0;
int gui_update_count = 0;
double volume = 0;
double frequency = 0;
void loop() {
  
  // Handle stepping
  if (ENC_steps != 0) {
    int steps = ENC_steps;
    ENC_steps = 0;
    GUI_on_step(steps);
  }

  // Handle clicking
  if (ENC_clicks != 0) {
    int clicks = ENC_clicks;
    ENC_clicks = 0;
    GUI_on_click(clicks);
  }

  volume = 1.0 - ANT_get_freq_volume() / 1500.0;
  frequency = ANT_get_freq_pitch() * 0.8;

  EFX_apply_effects(&frequency, &volume);
  
  if (volume < 0) volume = 0;
  if (volume > 1) volume = 1;
  if (frequency < 1)    frequency = 1;
  if (frequency > 20000) frequency = 20000;

  DAC_set_volume(volume);
  DAC_set_frequency(frequency);

  // Do GUI updates about every 100 ms
  gui_update_count++;
  if (gui_update_count > 10000) {
    gui_update_count = 0;
    GUI_on_update(frequency, volume);
  }
}
