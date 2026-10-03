/*
  ElyssaIMU - Advanced 01 Recording (FIFO)
  The IMU records up to 341 samples by itself while the ESP32 is free,
  then the sketch reads them all at once.
*/
#include <ElyssaIMU.h>

void setup() {
  Serial.begin(115200);
  if (!elyssa_imu_begin()) { Serial.println("IMU not found"); while (true) delay(1000); }
  elyssa_imu_set_rate(208);
}

void loop() {
  Serial.println("Recording 1 s... move the board!");
  elyssa_imu_start_recording(ELYSSA_RECORD_STOP_WHEN_FULL);
  delay(1000);                                   // the ESP32 is free here

  uint16_t n = elyssa_imu_recorded_samples();
  float accel[3], gyro[3], maxA = 0, maxG = 0;
  while (elyssa_imu_read_recorded(accel, gyro)) {
    maxA = max(maxA, sqrtf(accel[0] * accel[0] + accel[1] * accel[1] + accel[2] * accel[2]));
    maxG = max(maxG, sqrtf(gyro[0] * gyro[0] + gyro[1] * gyro[1] + gyro[2] * gyro[2]));
  }
  elyssa_imu_stop_recording();
  Serial.printf("%u samples | max acceleration %.2f g | max rotation %.0f dps\n\n", n, maxA, maxG);
  delay(2000);
}
