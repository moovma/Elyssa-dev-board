/*
  ElyssaIMU - Advanced 08 Tune the tap
  Type a number 1..31 + Enter to change the tap threshold
  (lower = lighter taps; default 12). Prints the direction of each tap.
*/
#include <ElyssaIMU.h>

void setup() {
  Serial.begin(115200);
  if (!elyssa_imu_begin()) { Serial.println("IMU not found"); while (true) delay(1000); }
  elyssa_imu_enable_tap();
  Serial.println("Tap the board. Type 1..31 to change the threshold.");
}

void loop() {
  if (Serial.available()) {
    int t = Serial.parseInt();
    while (Serial.available()) Serial.read();
    if (elyssa_imu_set_tap_threshold(t)) Serial.printf("--> threshold %d\n", t);
  }
  const char *dir[] = {"?", "+X", "-X", "+Y", "-Y", "+Z", "-Z"};
  if (elyssa_imu_double_tapped()) Serial.printf("Double tap %s\n", dir[elyssa_imu_tap_direction()]);
  else if (elyssa_imu_tapped())   Serial.printf("Tap %s\n", dir[elyssa_imu_tap_direction()]);
  delay(2);
}
