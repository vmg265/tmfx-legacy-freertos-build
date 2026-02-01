#ifndef BUTTON_HANDLER_H
#define BUTTON_HANDLER_H

#include <Arduino.h>
// Inside button_handler.h
#define BTN_UP_PIN     0
#define BTN_DOWN_PIN   2
#define BTN_SELECT_PIN 46
#define BTN_BACK_PIN   14

// Optional: Enable debug output
#define BUTTON_DEBUG true
// Identify buttons
enum ButtonType { BTN_UP, BTN_DOWN, BTN_SELECT, BTN_BACK, BTN_UNKNOWN };

void buttonTask(void* pvParameters); 
void setupButtons();                
#endif
