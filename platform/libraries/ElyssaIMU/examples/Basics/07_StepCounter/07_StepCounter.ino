/*
  Elyssa IMU - 07 Step counter
  Walk with the board, or shake it up and down in a steady rhythm.
  Counting starts after a few steps.
*/
uint16_t lastSteps = 0;

void setup() {
  Serial.begin(115200);
  if (!elyssa_imu_begin()) { Serial.println("IMU not found"); while (true) delay(1000); }
  elyssa_imu_start_steps();
  elyssa_imu_reset_steps();
  Serial.println("Walk!");
}

void loop() {
  uint16_t s = elyssa_imu_steps();
  if (s != lastSteps) {
    Serial.printf("Steps: %u\n", s);
    lastSteps = s;
  }
  delay(100);
}
