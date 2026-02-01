#include "display_manager.h"
#include "display_power.h"

static unsigned long lastActivityTime = 0;
static const unsigned long DISPLAY_TIMEOUT = 10000; 
static bool screenIsOn = true;

void initDisplayManager() {
    pinMode(40, OUTPUT); 
    digitalWrite(40, HIGH);
    lastActivityTime = millis();
}

void resetScreenTimer() {
    lastActivityTime = millis();
}

bool isDisplayOn() {
    return screenIsOn;
}

void wakeDisplay() {
    if (!screenIsOn) {
        displayWake();
        screenIsOn = true;
    }
    resetScreenTimer();
}

void sleepDisplay() {
    if (screenIsOn) {
        displaySleep();
        screenIsOn = false;
    }
}

void updateDisplayManager() {
    if (screenIsOn && (millis() - lastActivityTime >= DISPLAY_TIMEOUT)) {
        sleepDisplay();
    }
}