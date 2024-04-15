#pragma once

#include "waveforms.h"
#include "antenna.h"
#include "dac.h"
#include "tones.h"
#include "effects.h"

// For keeping track of system state
enum GUI_State {
  CALIBRATING,
  MENU_DISPLAY,
  MENU_WAVEFORM,  SET_WAVEFORM,
  MENU_AUTOTUNE,
  MENU_VIBRATO,   SET_VIBRATO
} GUI_current_state;

void GUI_goto_state(enum GUI_State new_state);
void GUI_on_step(int steps);
void GUI_on_click(int clicks);

// Initialization
void GUI_setup() {
  GUI_goto_state(MENU_DISPLAY);
}

// Respond to encoder rotation
void GUI_on_step(int steps) {
  
  switch(GUI_current_state) {

    // Scrolling through menu
    case MENU_DISPLAY:    GUI_goto_state(steps < 0 ? MENU_VIBRATO : MENU_WAVEFORM); break;
    case MENU_WAVEFORM:   GUI_goto_state(steps < 0 ? MENU_DISPLAY  : MENU_AUTOTUNE); break;
    case MENU_AUTOTUNE:   GUI_goto_state(steps < 0 ? MENU_WAVEFORM : MENU_VIBRATO); break;
    case MENU_VIBRATO:    GUI_goto_state(steps < 0 ? MENU_AUTOTUNE : MENU_DISPLAY); break;

    case SET_WAVEFORM:
      WAVEFORM_step(steps);
      LCD.setCursor(0, 1);
      LCD.print("                    ");
      LCD.setCursor(0, 1);
      LCD.print("> ");
      LCD.print(WAVEFORM_label);
      break;

    case SET_VIBRATO:
      EFX_vibrato += steps;
      if (EFX_vibrato < 0) EFX_vibrato = 0;
      if (EFX_vibrato > 10) EFX_vibrato = 10;
      LCD.setCursor(0, 1);
      LCD.print("                    ");
      LCD.setCursor(0, 1);
      LCD.print("> ");
      if (EFX_vibrato) {
        LCD.print(EFX_vibrato);
        LCD.print("Hz");
      } else {
        LCD.print("OFF");
      }
      break;
  }
  
}

// Respond to encoder pressing
void GUI_on_click(int clicks) {
  
  switch(GUI_current_state) {
    case MENU_DISPLAY:
      GUI_goto_state(CALIBRATING);
      ANT_calibrate();    
      GUI_goto_state(MENU_DISPLAY);            
      break;
    
    case MENU_WAVEFORM:   GUI_goto_state(SET_WAVEFORM);   break;
    case SET_WAVEFORM:    GUI_goto_state(MENU_WAVEFORM);  break;
    
    case MENU_AUTOTUNE:
      EFX_autotune = !EFX_autotune;
      LCD.setCursor(0, 1);
      if (EFX_autotune) LCD.print("ON ");
      else LCD.print("OFF");
      break;

    case MENU_VIBRATO:   GUI_goto_state(SET_VIBRATO);   break;
    case SET_VIBRATO:    GUI_goto_state(MENU_VIBRATO);  break;
  }
  
}

// Respond to periodic updates using current frequency and volume
void GUI_on_update(double frequency, double volume) {
  switch(GUI_current_state) {
    case MENU_DISPLAY:

    // Write frequency
    char buf[21];
    sprintf(buf, "%5dHz       ", int(frequency));
    LCD.setCursor(8, 0);
    LCD.print(buf);
    
    // Write tone
    int tone_number = closest_tone(frequency);
    char name_buffer[8];
    name_of_tone(name_buffer, tone_number);
    LCD.setCursor(16, 0);
    LCD.print(name_buffer);

    // Write volume
    sprintf(buf, "%3d%%         ", int(100*volume));
    LCD.setCursor(10, 1);
    LCD.print(buf);
  }
}

// Move to a new state
void GUI_goto_state(enum GUI_State new_state) {
  GUI_current_state = new_state;

  switch(GUI_current_state) {

    case CALIBRATING:
      DAC_volume = 0;
      LCD.clear();
      LCD.print("Calibrating...");
      LCD.setCursor(0, 1);
      LCD.print("Please stand back.");
      break;

    case MENU_DISPLAY:
      LCD.clear();
      LCD.print("Freq:");
      LCD.setCursor(0, 1);
      LCD.print("Volume:   ");
      break;

    case MENU_WAVEFORM:
      LCD.clear();
      LCD.print("Waveform");
      LCD.setCursor(0, 1);
      LCD.print(WAVEFORM_label);
      break;

    case SET_WAVEFORM:
      LCD.clear();
      LCD.print("Waveform");
      LCD.setCursor(0, 1);
      LCD.print("> ");
      LCD.print(WAVEFORM_label);
      break;

    case MENU_AUTOTUNE:
      LCD.clear();
      LCD.print("Pitch correction");
      LCD.setCursor(0, 1);
      if (EFX_autotune) LCD.print("ON ");
      else LCD.print("OFF");
      break;

    case MENU_VIBRATO:
      LCD.clear();
      LCD.print("Vibrato");
      LCD.setCursor(0, 1);
      if (EFX_vibrato) {
        LCD.print(EFX_vibrato);
        LCD.print("Hz");
      } else {
        LCD.print("OFF");
      }
      break;

    case SET_VIBRATO:
      LCD.clear();
      LCD.print("Vibrato");
      LCD.setCursor(0, 1);
      LCD.print("> ");
      if (EFX_vibrato) {
        LCD.print(EFX_vibrato);
        LCD.print("Hz");
      } else {
        LCD.print("OFF");
      }
      break;
  }
}
