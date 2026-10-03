/*
  ElyssaIMU - Advanced 04 Stillness
  Detects when the board stops moving (to switch things off automatically).
*/
#include <ElyssaIMU.h>

bool wasStill = false;

void setup() {
  Serial.begin(115200);
  if (!elyssa_imu_begin()) { Serial.println("IMU not found"); while (true) delay(1000); }
  elyssa_imu_enable_motion(2);      // the motion sensitivity is also used for stillness
  elyssa_imu_set_still_time(5);
  elyssa_imu_enable_still(ELYSSA_STILL_GYRO_SLEEP);   // the gyroscope sleeps while still
  Serial.printf("Still after %.1f s without movement\n", elyssa_imu_get_still_time());
}

void loop() {
  bool still = elyssa_imu_is_still();
  if (still != wasStill) {
    Serial.println(still ? "--- still ---" : "--- moving ---");
    setLedColor(still ? ELYSSA_BLUE : ELYSSA_OFF);
    wasStill = still;
  }
  delay(100);
}
