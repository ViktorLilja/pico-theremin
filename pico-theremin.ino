#include "lcd.h"
#include "encoder.h"
#include "antenna.h"
#include "dac.h"

// Global parameters (editable settings)
float     frequency = 440.0;
float     volume    = 90;

enum      Waveform           {SINE,   SQUARE,   TRIANGLE,   Waveform_COUNT};
String    waveform_names[] = {"Sine", "Square", "Triangle"};
Waveform  waveform         = SINE;

// For keeping track of system state
enum State {
  MENU_DISPLAY,
  MENU_FREQUENCY, SET_FREQUENCY,
  MENU_VOLUME,    SET_VOLUME,
  MENU_WAVEFORM,  SET_WAVEFORM,
};
State state;
void goto_state(enum State new_state);

// Event handlers
void on_step(int steps);
void on_click(int clicks);
void on_update();

// Initialization
void setup() {
  LCD_setup();
  ENC_setup();
  ANT_setup();

  goto_state(MENU_DISPLAY);
}

void setup1() {
  DAC_setup();
}

void loop1() {
  DAC_write_next();
}

// Continous loop
int gui_count = 0;
void loop() {
  
  // Handle stepping
  if (ENC_steps != 0) {
    int steps = ENC_steps;
    ENC_steps = 0;
    on_step(steps);
  }

  // Handle clicking
  if (ENC_clicks != 0) {
    int clicks = ENC_clicks;
    ENC_clicks = 0;
    on_click(clicks);
  }

  // Update sound frequency
  int inc = (ANT_freq_pitch - ANT_zero_freq_pitch)/3.4;
  if (inc < 0)      inc = 0;
  if (inc > 65536)  inc = 65536;
  DAC_increment = inc;

  // Update sound volume
  int vol = ANT_freq_volume - ANT_zero_freq_volume;
  if (vol < 0)      vol = 0;
  if (vol > 2048)   vol = 2048;
  DAC_volume = vol;
  
  // Update GUI
  // THIS PART IS PROBLEMATIC - CAUSES SLIGHT GLITCHES IN SOUND
  gui_count++;
  if (gui_count > 100000) {
    gui_count = 0;
    on_update();
  }
}

// Respond to encoder rotation
void on_step(int steps) {
  switch(state) {

    // Scrolling through menu
    case MENU_DISPLAY:    goto_state(steps < 0 ? MENU_WAVEFORM  : MENU_FREQUENCY);  break;
    case MENU_FREQUENCY:  goto_state(steps < 0 ? MENU_DISPLAY   : MENU_VOLUME   );  break;
    case MENU_VOLUME:     goto_state(steps < 0 ? MENU_FREQUENCY : MENU_WAVEFORM );  break;
    case MENU_WAVEFORM:   goto_state(steps < 0 ? MENU_VOLUME    : MENU_DISPLAY);    break;

    // Changing settings
    case SET_FREQUENCY:   
      frequency += steps;
      LCD.setCursor(0, 1);
      LCD.print("                    ");
      LCD.setCursor(0, 1);
      LCD.print(">");
      LCD.print(frequency);
      LCD.print(" Hz");
      break;
      
    case SET_VOLUME:
      volume += steps;
      if (volume < 0)   volume = 0;
      if (volume > 100) volume = 100;
      LCD.setCursor(0, 1);
      LCD.print("                    ");
      LCD.setCursor(0, 1);
      LCD.print(">");
      LCD.print(volume);
      LCD.print(" %");
      break;

    case SET_WAVEFORM:
      waveform = (Waveform)((((waveform + steps) % Waveform_COUNT) + Waveform_COUNT) % Waveform_COUNT);
      LCD.setCursor(0, 1);
      LCD.print("                    ");
      LCD.setCursor(0, 1);
      LCD.print(">");
      LCD.print(waveform_names[waveform]);
      break;
  }
}

// Respond to encoder pressing
void on_click(int clicks) {
  switch(state) {
    case MENU_DISPLAY:    ANT_calibrate();            break;
    case MENU_FREQUENCY:  goto_state(SET_FREQUENCY);  break;
    case MENU_VOLUME:     goto_state(SET_VOLUME);     break;
    case MENU_WAVEFORM:   goto_state(SET_WAVEFORM);   break;
    
    case SET_FREQUENCY:   goto_state(MENU_FREQUENCY); break;
    case SET_VOLUME:      goto_state(MENU_VOLUME);    break;
    case SET_WAVEFORM:    goto_state(MENU_WAVEFORM);  break;
  }
}

// Respond to periodic updates
void on_update() {
  switch(state) {
    case MENU_DISPLAY:
    LCD.setCursor(11, 0);
    LCD.print("          ");
    LCD.setCursor(11, 0);
    LCD.print(ANT_freq_pitch - ANT_zero_freq_pitch);
    LCD.setCursor(11, 1);
    LCD.print("          ");
    LCD.setCursor(11, 1);
    LCD.print(DAC_volume);

  }
}

// Move to a new state
void goto_state(enum State new_state) {
  state = new_state;

  switch(state) {

    case MENU_DISPLAY:
      LCD.clear();
      LCD.print("Frequency:");
      LCD.setCursor(0, 1);
      LCD.print("Volume:   ");
      break;
    
    case MENU_FREQUENCY:
      LCD.clear();
      LCD.print("Frequency        1/3");
      LCD.setCursor(0, 1);
      LCD.print(frequency);
      LCD.print(" Hz");
      break;

    case MENU_VOLUME:
      LCD.clear();
      LCD.print("Volume           2/3");
      LCD.setCursor(0, 1);
      LCD.print(volume);
      LCD.print(" %");
      break;

    case MENU_WAVEFORM:
      LCD.clear();
      LCD.print("Waveform         3/3");
      LCD.setCursor(0, 1);
      LCD.print(waveform_names[waveform]);
      break;

    case SET_FREQUENCY:
      LCD.clear();
      LCD.print("Frequency        1/3");
      LCD.setCursor(0, 1);
      LCD.print(">");
      LCD.print(frequency);
      LCD.print(" Hz");
      break;

    case SET_VOLUME:
      LCD.clear();
      LCD.print("Volume           2/3");
      LCD.setCursor(0, 1);
      LCD.print(">");
      LCD.print(volume);
      LCD.print(" %");
      break;

    case SET_WAVEFORM:
      LCD.clear();
      LCD.print("Waveform         3/3");
      LCD.setCursor(0, 1);
      LCD.print(">");
      LCD.print(waveform_names[waveform]);
      break;
  }
}
