/*
  Elyssa IMU - 05 Free-fall
  Drop the board onto something SOFT from about 20 cm: LED red.
*/
void setup() {
  Serial.begin(115200);
  if (!elyssa_imu_begin()) { Serial.println("IMU not found"); while (true) delay(1000); }
  elyssa_imu_set_rate(416);          // faster = shorter drops detected
  elyssa_imu_enable_freefall();
  Serial.println("Drop the board onto a cushion");
}

void loop() {
  if (elyssa_imu_fell()) {
    Serial.println("Free-fall!");
    setLedColor(ELYSSA_RED);
    delay(1000);
    setLedColor(ELYSSA_OFF);
  }
  delay(10);
}
