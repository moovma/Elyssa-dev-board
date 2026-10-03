/*
  Elyssa IMU - 06 Motion
  Prints a message each time the board is moved.
*/
void setup() {
  Serial.begin(115200);
  if (!elyssa_imu_begin()) { Serial.println("IMU not found"); while (true) delay(1000); }
  elyssa_imu_enable_motion(3);       // sensitivity: 1 = most sensitive .. 63
  Serial.println("Move the board");
}

void loop() {
  if (elyssa_imu_moved()) Serial.println("Moved!");
  delay(50);
}
