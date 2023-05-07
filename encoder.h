
#define ENC_PIN_A  19
#define ENC_PIN_B  20
#define ENC_PIN_SW 21

// Number of steps rotated and number of clicks, will change dynamically.
int ENC_steps  = 0;
int ENC_clicks = 0;

// ISR debounce wrappers
int    ENC_millis_last;
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
  if (millis() - ENC_millis_last > 200) ENC_steps++;
  ENC_millis_last = millis();
}

void ENC_ISR_B() {
  if (millis() - ENC_millis_last > 200) ENC_steps--;
  ENC_millis_last = millis();
}

void ENC_ISR_SW() {
  if (millis() - ENC_millis_last > 200) ENC_clicks++;
  ENC_millis_last = millis();
}
