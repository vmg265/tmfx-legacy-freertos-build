#include "display_power.h"
#include <TFT_eSPI.h>

#define LCD_BACKLIGHT_PIN 40
extern TFT_eSPI tft;

void displaySleep() {
    // 1. Send Sleep Mode command to the GC9A01 controller
    tft.writecommand(0x10); 
    
    // 2. Turn off the backlight to save power
    digitalWrite(LCD_BACKLIGHT_PIN, LOW);
    
    Serial.println("Display Power: GC9A01 Sleep + Backlight OFF");
}
void displayWake() {
    // 1. Turn on the backlight
    digitalWrite(LCD_BACKLIGHT_PIN, HIGH);
    
    // 2. Exit Sleep Mode
    tft.writecommand(0x11); 
    
    // 3. Required delay for GC9A01 stabilization
    delay(120); 

    Serial.println("Display Power: GC9A01 Wake + Backlight ON");
}