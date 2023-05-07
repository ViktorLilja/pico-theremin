#pragma once

#include "waveforms.h"
#include "antenna.h"
#include "dac.h"

// For keeping track of system state
enum GUI_State {
  MENU_DISPLAY,
  MENU_WAVEFORM,  SET_WAVEFORM,
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
    case MENU_DISPLAY:    GUI_goto_state(steps < 0 ? MENU_WAVEFORM  : MENU_WAVEFORM); break;
    case MENU_WAVEFORM:   GUI_goto_state(steps < 0 ? MENU_DISPLAY    : MENU_DISPLAY); break;

    case SET_WAVEFORM:
      WAVEFORM_step(steps);
      LCD.setCursor(0, 1);
      LCD.print("                    ");
      LCD.setCursor(0, 1);
      LCD.print(">");
      LCD.print(WAVEFORM_label);
      break;
  }
  
}

// Respond to encoder pressing
void GUI_on_click(int clicks) {
  
  switch(GUI_current_state) {
    case MENU_DISPLAY:    ANT_calibrate();                break;
    case MENU_WAVEFORM:   GUI_goto_state(SET_WAVEFORM);   break;
   
    case SET_WAVEFORM:    GUI_goto_state(MENU_WAVEFORM);  break;
  }
  
}

// Respond to periodic updates using current frequency and volume
void GUI_on_update(double frequency, double volume) {
  switch(GUI_current_state) {
    case MENU_DISPLAY:
    LCD.setCursor(11, 0);
    LCD.print("          ");
    LCD.setCursor(11, 0);
    LCD.print(int(frequency));
    LCD.print("Hz");
    LCD.setCursor(11, 1);
    LCD.print("          ");
    LCD.setCursor(11, 1);
    LCD.print(int(100 * volume));
    LCD.print("%");
  }
}

// Move to a new state
void GUI_goto_state(enum GUI_State new_state) {
  GUI_current_state = new_state;

  switch(GUI_current_state) {

    case MENU_DISPLAY:
      LCD.clear();
      LCD.print("Frequency:");
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
      LCD.print(">");
      LCD.print(WAVEFORM_label);
      break;
  }
}
