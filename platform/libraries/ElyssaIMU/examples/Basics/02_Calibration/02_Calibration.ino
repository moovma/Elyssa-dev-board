/*
  Elyssa IMU - 02 Gyroscope calibration
  Every gyroscope shows a small rotation at rest. Calibration removes it.
  Keep the board STILL during the first second.
*/
void setup() {
  Serial.begin(115200);
  if (!elyssa_imu_begin()) { Serial.println("IMU not found"); while (true) delay(1000); }

  Serial.printf("Before: %.2f %.2f %.2f dps\n", elyssa_imu_gyro_x(), elyssa_imu_gyro_y(), elyssa_imu_gyro_z());
  Serial.println("Calibrating... don't move the board");
  elyssa_imu_calibrate_gyro();
  Serial.printf("After:  %.2f %.2f %.2f dps\n", elyssa_imu_gyro_x(), elyssa_imu_gyro_y(), elyssa_imu_gyro_z());
}

void loop() {}
