#pragma once
#ifndef QMI8658_DRIVER_H
#define QMI8658_DRIVER_H
#include "Arduino.h"

#include <stdint.h>

struct IMUData {
    float acc[3];   // X, Y, Z in mg
    float gyro[3];  // X, Y, Z in dps
    uint32_t timestamp;
};

struct IMUSmoothedData {
    float acc[3] = {0};
    float gyro[3] = {0};
};

// Call once in setup()
bool initQMI8658Sensor();

// Read one sample
bool readIMUData(IMUData &data);

// Start background task
void startIMUTask();

// Global smoothed value (read-only use)
extern IMUSmoothedData imuSmooth;

#endif