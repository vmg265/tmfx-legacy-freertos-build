#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <Arduino.h>

void initDisplayManager();
void updateDisplayManager(); // 
void wakeDisplay();
void sleepDisplay();
bool isDisplayOn();
void resetScreenTimer(); 
#endif