#pragma once

#ifndef PWM_TEST_H
#define PWM_TEST_H

#include <Arduino.h>

// Pin definitions
extern const int DIR_PIN;
extern const int STEP_PIN;

// Stepper state
extern bool dirCW;
extern bool autoMode;
extern bool autoStepping;
extern int stepsPerMove;
extern int pulseWidthMicros;
extern int stepIntervalMs;
extern int focusIndex;

// Functions
void updateDemoLabels();
void updateDemoFocus();


#endif
