#pragma once

#include <LiquidCrystal.h>

// Theremin v1
/*
#define LCD_PIN_RS 3
#define LCD_PIN_EN 4
#define LCD_PIN_D4 7
#define LCD_PIN_D5 6
#define LCD_PIN_D6 9
#define LCD_PIN_D7 8
*/


// Theremin v2

#define LCD_PIN_RS 4
#define LCD_PIN_EN 3
#define LCD_PIN_D4 9
#define LCD_PIN_D5 6
#define LCD_PIN_D6 8
#define LCD_PIN_D7 7


LiquidCrystal LCD(
  LCD_PIN_RS,
  LCD_PIN_EN,
  LCD_PIN_D4,
  LCD_PIN_D5,
  LCD_PIN_D6,
  LCD_PIN_D7
 );

void LCD_setup() {
  LCD.begin(20, 2);
}
