#ifndef BATTERY_STATUS_H
#define BATTERY_STATUS_H

#include <Arduino.h>
#include <lvgl.h>

void batteryStatusInit();

float getBatteryVoltage();

int getBatteryPercentage(float voltage);

void updateBatteryLabel(lv_obj_t *label);

#endif // BATTERY_STATUS_H