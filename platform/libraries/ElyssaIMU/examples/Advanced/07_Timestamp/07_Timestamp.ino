/*
  ElyssaIMU - Advanced 07 Timestamp
  The IMU's own clock, useful to time events precisely.
*/
#include <ElyssaIMU.h>

void setup() {
  Serial.begin(115200);
  if (!elyssa_imu_begin()) { Serial.println("IMU not found"); while (true) delay(1000); }
  elyssa_imu_enable_timestamp(true, true);   // 25 us resolution
  elyssa_imu_reset_timestamp();
}

void loop() {
  Serial.printf("IMU clock: %.3f s | ESP32 millis: %.3f s\n", elyssa_imu_timestamp() / 1e6, millis() / 1e3);
  delay(1000);
}
