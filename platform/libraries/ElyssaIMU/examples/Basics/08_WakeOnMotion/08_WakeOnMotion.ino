/*
  Elyssa IMU - 08 Wake on motion
  The ESP32 sleeps (very low power) until the board is moved.
  After each wake-up, the sketch starts again from setup().
*/
#include "esp_sleep.h"

void setup() {
  Serial.begin(115200);
  delay(1500);   // time for the Serial Monitor to reconnect

  if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0) {
    Serial.println("Woken up by motion!");
    setLedColor(ELYSSA_GREEN);
    delay(1000);
  }
  if (!elyssa_imu_begin()) { Serial.println("IMU not found"); while (true) delay(1000); }
  elyssa_imu_low_power(true);
  elyssa_imu_wake_on_motion(3);
  Serial.println("Sleeping. Move the board to wake it up.");
  setLedColor(ELYSSA_OFF);
  delay(100);
  esp_deep_sleep_start();
}

void loop() {}
