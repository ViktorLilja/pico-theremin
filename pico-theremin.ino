#include "lcd.h"
#include "encoder.h"
#include "antenna.h"
#include "dac.h"
#include "gui.h"
#include "effects.h"

// Second core is only used to run the DAC to avoid audio interruptions.
void setup1() { DAC_setup(); }
void loop1()  { DAC_write_next(); }

// First core handles all other peripherals
void setup() {
  LCD_setup();
  ENC_setup();
  ANT_setup();
  GUI_setup();
}

int gui_update_count = 0;
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

  double volume = ANT_freq_volume / 1000;
  double frequency = ANT_freq_pitch;

  EFX_apply_effects(&frequency, &volume);
  
  if (volume < 0) volume = 0;
  if (volume > 1) volume = 1;
  if (frequency < 20)    frequency = 20;
  if (frequency > 20000) frequency = 20000;

  DAC_set_volume(volume);
  DAC_set_frequency(frequency);

  // Do GUI updates once in a while
  // THIS PART IS PROBLEMATIC - CAUSES SLIGHT GLITCHES IN SOUND
  gui_update_count++;
  if (gui_update_count > 20000) {
    gui_update_count = 0;
    GUI_on_update(frequency, volume);
  }
}
