#include "qmi8658_driver.h"
#include <Wire.h>
#include <lvgl.h>
#include "ui.h"

// ========= CONFIG =========
static uint8_t QMI8658_ADDR = 0x6B;
static const float alpha = 0.1f; // smoothing factor
static const uint16_t acc_lsb_div = (1 << 12); // 8g
static const uint16_t gyro_lsb_div = 64;       // 512 dps

// ========= GLOBAL STATE =========
IMUSmoothedData imuSmooth;

// ========= I2C HELPERS =========
static void i2cWriteByte(uint8_t reg, uint8_t val) {
    Wire.beginTransmission(QMI8658_ADDR);
    Wire.write(reg);
    Wire.write(val);
    Wire.endTransmission();
}

static void i2cReadBytes(uint8_t reg, uint8_t *buf, uint8_t len) {
    Wire.beginTransmission(QMI8658_ADDR);
    Wire.write(reg);
    Wire.endTransmission(false);
    Wire.requestFrom((int)QMI8658_ADDR, (int)len);
    for (uint8_t i = 0; i < len && Wire.available(); i++) {
        buf[i] = Wire.read();
    }
}

// ========= SENSOR INIT =========
bool initQMI8658Sensor() {
    Wire.begin(6, 7);  // SDA = GPIO6, SCL = GPIO7 
    delay(10);

    uint8_t chipID = 0;
    const uint8_t addresses[] = {0x6A, 0x6B};
    for (int i = 0; i < 2; i++) {
        QMI8658_ADDR = addresses[i];
        i2cReadBytes(0x00, &chipID, 1);
        if (chipID == 0x05) {
            Serial.printf("QMI8658 found at address 0x%02X\n", QMI8658_ADDR);
            break;
        }
    }

    if (chipID != 0x05) {
        Serial.println("QMI8658 not detected.");
        return false;
    }

    i2cWriteByte(0x02, 0x60); // CTRL1: software reset
    delay(50);
    i2cWriteByte(0x03, 0x23); // CTRL2: ACC = 8g, 1kHz
    i2cWriteByte(0x04, 0x23); // CTRL3: GYRO = 512dps, 1kHz
    i2cWriteByte(0x06, 0x00); // CTRL5: no LPF
    i2cWriteByte(0x08, 0x03); // CTRL7: enable ACC + GYRO

    Serial.println("QMI8658 initialized successfully");
    return true;
}

// ========= SENSOR READ =========
bool readIMUData(IMUData &data) {
    uint8_t buf[12];
    int16_t accRaw[3], gyroRaw[3];

    uint8_t ts[3] = {0};
    i2cReadBytes(0x30, ts, 3); // TIMESTAMP_L
    data.timestamp = ((uint32_t)ts[2] << 16) | ((uint32_t)ts[1] << 8) | ts[0];

    i2cReadBytes(0x35, buf, 12); // acc + gyro data
    for (int i = 0; i < 3; i++) {
        accRaw[i]  = (int16_t)((buf[i*2+1] << 8) | buf[i*2]);
        gyroRaw[i] = (int16_t)((buf[i*2+7] << 8) | buf[i*2+6]);
    }

    for (int i = 0; i < 3; i++) {
        data.acc[i]  = (float)accRaw[i] * 1000.0f / acc_lsb_div;   // mg
        data.gyro[i] = (float)gyroRaw[i] / gyro_lsb_div;           // dps
    }

    return true;
}

// ========= TASK =========
void IMUTask(void *pvParams) {
    IMUData data;
    char buf[32];

    while (true) {
        if (readIMUData(data)) {
            for (int i = 0; i < 3; i++) {
                imuSmooth.acc[i]  = alpha * data.acc[i]  + (1 - alpha) * imuSmooth.acc[i];
                imuSmooth.gyro[i] = alpha * data.gyro[i] + (1 - alpha) * imuSmooth.gyro[i];
            }

            sprintf(buf, "X: %.2f", imuSmooth.acc[0]); lv_label_set_text(objects.acc_label_x, buf);
            sprintf(buf, "Y: %.2f", imuSmooth.acc[1]); lv_label_set_text(objects.acc_label_y, buf);
            sprintf(buf, "Z: %.2f", imuSmooth.acc[2]); lv_label_set_text(objects.acc_label_z, buf);
            sprintf(buf, "X: %.2f", imuSmooth.gyro[0]); lv_label_set_text(objects.gyro_label_x, buf);
            sprintf(buf, "Y: %.2f", imuSmooth.gyro[1]); lv_label_set_text(objects.gyro_label_y, buf);
            sprintf(buf, "Z: %.2f", imuSmooth.gyro[2]); lv_label_set_text(objects.gyro_label_z, buf);
        }

        vTaskDelay(pdMS_TO_TICKS(50));
    }
}

void startIMUTask() {
    xTaskCreatePinnedToCore(
        IMUTask,
        "IMU Task",
        4096,
        NULL,
        1,
        NULL,
        1
    );
}