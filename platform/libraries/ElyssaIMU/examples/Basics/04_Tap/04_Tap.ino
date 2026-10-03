/*
  Elyssa IMU - 04 Tap and double tap
  Tap the board: LED green. Double tap: LED blue.
  Check taps often: no long delay() in loop().
*/
void setup() {
  Serial.begin(115200);
  if (!elyssa_imu_begin()) { Serial.println("IMU not found"); while (true) delay(1000); }
  elyssa_imu_enable_tap();
  Serial.println("Tap or double-tap the board");
}

void loop() {
  if (elyssa_imu_double_tapped()) {
    Serial.println("Double tap!");
    setLedColor(ELYSSA_BLUE);
  } else if (elyssa_imu_tapped()) {
    Serial.println("Tap");
    setLedColor(ELYSSA_GREEN);
  }
  delay(2);
}
