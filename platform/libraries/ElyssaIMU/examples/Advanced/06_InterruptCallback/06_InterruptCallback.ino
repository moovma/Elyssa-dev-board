/*
  ElyssaIMU - Advanced 06 Interrupt callback
  A function is called by the IMU pin (GPIO21) at every new sample.
  The callback only counts: printing is done in loop().
*/
#include <ElyssaIMU.h>

volatile uint32_t samples = 0;
void IRAM_ATTR onSample() { samples++; }

void setup() {
  Serial.begin(115200);
  if (!elyssa_imu_begin()) { Serial.println("IMU not found"); while (true) delay(1000); }
  elyssa_imu_on_interrupt(onSample);
  elyssa_imu_enable_data_ready_interrupt(true);
}

void loop() {
  delay(1000);
  Serial.printf("%lu interrupts in 1 s (rate %.0f Hz)\n", (unsigned long)samples, elyssa_imu_get_rate());
  samples = 0;
}
