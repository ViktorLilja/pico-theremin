#pragma once

// Theremin v1
/*
#define ENC_PIN_A  19
#define ENC_PIN_B  20
#define ENC_PIN_SW 21
*/

//Theremin v2
#define ENC_PIN_A  20
#define ENC_PIN_B  19
#define ENC_PIN_SW 21


// Number of steps rotated and number of clicks, will change dynamically.
int ENC_steps  = 0;
int ENC_clicks = 0;

// ISR debounce wrappers
int    ENC_millis_last;
int    ENC_debounce_A = 0;
int    ENC_debounce_B = 0;
void   ENC_ISR_A();
void   ENC_ISR_B();
void   ENC_ISR_SW();

void ENC_setup() {
  pinMode(ENC_PIN_A,  INPUT_PULLUP);
  pinMode(ENC_PIN_B,  INPUT_PULLUP);
  pinMode(ENC_PIN_SW, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(ENC_PIN_A),  ENC_ISR_A, FALLING);
  attachInterrupt(digitalPinToInterrupt(ENC_PIN_B),  ENC_ISR_B, FALLING);
  attachInterrupt(digitalPinToInterrupt(ENC_PIN_SW), ENC_ISR_SW, FALLING); 
}

void ENC_ISR_A() {
  ENC_debounce_B = 0;
  if (ENC_debounce_A) return;
  if (!digitalRead(ENC_PIN_B)) {
    ENC_steps--;
    ENC_debounce_A = 1;
  }
}

void ENC_ISR_B() {
  ENC_debounce_A = 0;
  if (ENC_debounce_B) return;
  if (!digitalRead(ENC_PIN_A)) {
    ENC_steps++;
    ENC_debounce_B = 1;
  }
}

void ENC_ISR_SW() {
  if (millis() - ENC_millis_last > 300) ENC_clicks++;
  ENC_millis_last = millis();
}
