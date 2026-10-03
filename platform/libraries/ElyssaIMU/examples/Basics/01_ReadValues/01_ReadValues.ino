/*
  Elyssa IMU - 01 Read values
  Acceleration (g), rotation (degrees per second) and temperature.
  The IMU functions are built into the Elyssa board: no #include needed.
*/
void setup() {
  Serial.begin(115200);
  if (!elyssa_imu_begin()) {
    Serial.println("IMU not found");
    while (true) delay(1000);
  }
}

void loop() {
  float ax, ay, az, gx, gy, gz;
  elyssa_imu_read_accel(ax, ay, az);
  elyssa_imu_read_gyro(gx, gy, gz);
  Serial.printf("acc %5.2f %5.2f %5.2f g | rotation %7.1f %7.1f %7.1f dps | %.1f C\n",
                ax, ay, az, gx, gy, gz, elyssa_imu_temperature());
  delay(200);
}
