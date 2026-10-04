/*
  ElyssaIMU - Advanced 02 Filters
  Type a number + Enter to change the acceleration filter:
    0 off, 1..4 smoothing (light .. max), 5 remove gravity, 6 remove gravity (fast)
  With "remove gravity", the board at rest reads about 0, 0, 0 g.
*/
#include <ElyssaIMU.h>

void setup() {
  Serial.begin(115200);
  if (!elyssa_imu_begin()) { Serial.println("IMU not found"); while (true) delay(1000); }
}

void loop() {
  if (Serial.available()) {
    int f = Serial.parseInt();
    while (Serial.available()) Serial.read();
    if (f >= 0 && f <= 6) {
      elyssa_imu_set_accel_filter((ElyssaAccelFilter)f);
      Serial.printf("--> filter %d\n", f);
    }
  }
  float x, y, z;
  elyssa_imu_read_accel(x, y, z);
  Serial.printf("acc %6.3f %6.3f %6.3f g\n", x, y, z);
  delay(100);
}
