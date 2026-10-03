/*
  ElyssaIMU - Advanced 03 Self-test and full calibration
  Board FLAT, FACE UP and STILL.
*/
#include <ElyssaIMU.h>

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);
  if (!elyssa_imu_begin()) { Serial.println("IMU not found"); while (true) delay(1000); }

  float r[3];
  bool a = elyssa_imu_self_test_accel(r);
  Serial.printf("Accelerometer self-test: %s (%.0f %.0f %.0f mg)\n", a ? "OK" : "FAILED", r[0], r[1], r[2]);
  bool g = elyssa_imu_self_test_gyro(r);
  Serial.printf("Gyroscope self-test:     %s (%.0f %.0f %.0f dps)\n", g ? "OK" : "FAILED", r[0], r[1], r[2]);

  elyssa_imu_calibrate_gyro();
  bool c = elyssa_imu_calibrate_accel();
  float x, y, z;
  elyssa_imu_get_accel_offset(x, y, z);
  Serial.printf("Accelerometer calibration: %s, correction %.3f %.3f %.3f g\n", c ? "OK" : "FAILED (not flat?)", x, y, z);
  elyssa_imu_read_accel(x, y, z);
  Serial.printf("Now: %.3f %.3f %.3f g (expected 0 0 1)\n", x, y, z);
}

void loop() {}
