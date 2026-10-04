/*
  ElyssaIMU - Advanced 05 Tilt and wrist tilt
  Tilt: the board is tilted more than ~35 degrees and held.
  Wrist tilt: "raise to look" gesture, like a smartwatch.
*/
#include <ElyssaIMU.h>

void setup() {
  Serial.begin(115200);
  if (!elyssa_imu_begin()) { Serial.println("IMU not found"); while (true) delay(1000); }
  elyssa_imu_enable_tilt();
  // The 4 edges only. Don't enable Z+: lying flat, face up, would trigger it.
  elyssa_imu_set_wrist_tilt_axes(true, true, true, true, false, false);
  elyssa_imu_enable_wrist_tilt();
  Serial.println("Tilt the board, or turn it like looking at a watch");
}

void loop() {
  if (elyssa_imu_tilted()) Serial.println("Tilt!");
  const char *dir[] = {"?", "+X", "-X", "+Y", "-Y", "+Z", "-Z"};
  if (elyssa_imu_wrist_tilted()) Serial.printf("Wrist tilt %s\n", dir[elyssa_imu_wrist_tilt_direction()]);
  delay(20);
}
