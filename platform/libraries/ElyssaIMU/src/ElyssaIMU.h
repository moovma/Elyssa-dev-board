/*
  ElyssaIMU - advanced IMU functions for the Elyssa board (Moovma)
  Version 2.0.2

  The basic IMU functions are built into the Elyssa board package and need
  no #include: elyssa_imu_begin(), elyssa_imu_read_accel(), elyssa_imu_tapped()...
  This library ADDS the advanced features, in the same style. Both work
  together in the same sketch (same IMU, same settings, same events).

  Start the IMU with elyssa_imu_begin() as usual, then use these functions.
  Units: g, degrees per second (dps), degrees, microseconds.
  Built on ST's official LSM6DS3TR-C driver (included in the board package).
*/
#ifndef ELYSSA_IMU_H
#define ELYSSA_IMU_H

#include <Arduino.h>

#if !__has_include("elyssa_imu_internal.h")
#error "ElyssaIMU needs the Elyssa board: select Tools > Board > Elyssa"
#endif
#include "elyssa_imu_internal.h"

// =====================================================================
//  Values
// =====================================================================

// Direction of an event
enum ElyssaDirection : uint8_t {
  ELYSSA_DIRECTION_NONE,
  ELYSSA_X_POSITIVE, ELYSSA_X_NEGATIVE,
  ELYSSA_Y_POSITIVE, ELYSSA_Y_NEGATIVE,
  ELYSSA_Z_POSITIVE, ELYSSA_Z_NEGATIVE
};

// Acceleration filter (one at a time)
enum ElyssaAccelFilter : uint8_t {
  ELYSSA_ACCEL_FILTER_OFF,          // default
  ELYSSA_ACCEL_SMOOTH_LIGHT,        // low-pass, cut-off = rate / 9
  ELYSSA_ACCEL_SMOOTH_MEDIUM,       // rate / 50
  ELYSSA_ACCEL_SMOOTH_STRONG,       // rate / 100
  ELYSSA_ACCEL_SMOOTH_MAX,          // rate / 400
  ELYSSA_ACCEL_REMOVE_GRAVITY,      // high-pass, rate / 100: keeps only movements
  ELYSSA_ACCEL_REMOVE_GRAVITY_FAST  // high-pass, rate / 9: only quick movements
};

// Gyroscope smoothing (low-pass, useful at 833 Hz and above)
enum ElyssaGyroSmoothing : uint8_t {
  ELYSSA_GYRO_SMOOTH_OFF, ELYSSA_GYRO_SMOOTH_LIGHT, ELYSSA_GYRO_SMOOTH_MEDIUM,
  ELYSSA_GYRO_SMOOTH_STRONG, ELYSSA_GYRO_SMOOTH_MAX
};

// Gyroscope high-pass (removes slow drift), cut-off frequency
enum ElyssaGyroHighPass : uint8_t {
  ELYSSA_GYRO_HIGHPASS_OFF, ELYSSA_GYRO_HIGHPASS_0_016HZ, ELYSSA_GYRO_HIGHPASS_0_065HZ,
  ELYSSA_GYRO_HIGHPASS_0_26HZ, ELYSSA_GYRO_HIGHPASS_1HZ
};

// Free-fall threshold (lower = only real drops)
enum ElyssaFreeFallThreshold : uint8_t {
  ELYSSA_FREEFALL_156MG, ELYSSA_FREEFALL_219MG, ELYSSA_FREEFALL_250MG, ELYSSA_FREEFALL_312MG,
  ELYSSA_FREEFALL_344MG, ELYSSA_FREEFALL_406MG, ELYSSA_FREEFALL_469MG, ELYSSA_FREEFALL_500MG
};

// What the sensors do while the board is still
enum ElyssaStillMode : uint8_t {
  ELYSSA_STILL_KEEP_GYRO,    // accelerometer slows to 12.5 Hz, gyroscope unchanged
  ELYSSA_STILL_GYRO_SLEEP,   // ... and the gyroscope sleeps
  ELYSSA_STILL_GYRO_OFF      // ... and the gyroscope turns off
};

// Angle for orientation-change events
enum ElyssaOrientationAngle : uint8_t {
  ELYSSA_ANGLE_80, ELYSSA_ANGLE_70, ELYSSA_ANGLE_60, ELYSSA_ANGLE_50
};

// Recording (FIFO)
enum ElyssaRecordMode : uint8_t {
  ELYSSA_RECORD_KEEP_LATEST,     // continuous, overwrites the oldest samples
  ELYSSA_RECORD_STOP_WHEN_FULL   // keeps the first 341 samples
};

// =====================================================================
//  1. Sensors and power
// =====================================================================
bool  elyssa_imu_enable_accel(bool on);        // accelerometer on/off
bool  elyssa_imu_enable_gyro(bool on);         // gyroscope on/off (it uses most of the power)
bool  elyssa_imu_gyro_sleep(bool sleep);       // gyroscope standby, wakes up fast
bool  elyssa_imu_set_accel_rate(float hz);     // accelerometer only; 1.6 Hz in low power mode
bool  elyssa_imu_set_gyro_rate(float hz);      // gyroscope only
float elyssa_imu_get_gyro_rate();

// =====================================================================
//  2. Calibration and self-test
// =====================================================================
bool elyssa_imu_calibrate_accel(uint16_t ms = 1000);         // board FLAT, FACE UP, still
bool elyssa_imu_set_accel_offset(float x, float y, float z); // g, ADDED to readings (max 1.98)
bool elyssa_imu_get_accel_offset(float &x, float &y, float &z);
bool elyssa_imu_clear_accel_offset();
void elyssa_imu_set_gyro_offset(float x, float y, float z);  // dps, removed from readings
void elyssa_imu_get_gyro_offset(float &x, float &y, float &z);
void elyssa_imu_clear_gyro_offset();
bool elyssa_imu_self_test_accel(float result[3] = nullptr);  // change in mg (90..1700 = OK)
bool elyssa_imu_self_test_gyro(float result[3] = nullptr);   // change in dps (150..700 = OK)
bool elyssa_imu_self_test();                                 // both; board still

// =====================================================================
//  3. Timestamp (the IMU's own clock)
// =====================================================================
bool     elyssa_imu_enable_timestamp(bool on, bool fine = false);  // fine: 25 us steps (~7 min), else 6.4 ms (~30 h)
uint64_t elyssa_imu_timestamp();                                   // microseconds
bool     elyssa_imu_reset_timestamp();

// =====================================================================
//  4. Filters
// =====================================================================
bool elyssa_imu_set_accel_filter(ElyssaAccelFilter filter);   // strong smoothing: a few s to settle
bool elyssa_imu_set_accel_analog_400hz(bool on);          // anti-aliasing 400 Hz instead of 1.5 kHz
bool elyssa_imu_set_gyro_smoothing(ElyssaGyroSmoothing level);
bool elyssa_imu_set_gyro_highpass(ElyssaGyroHighPass cutoff);

// =====================================================================
//  5. Tap tuning (enable with elyssa_imu_enable_tap())
// =====================================================================
bool elyssa_imu_disable_tap();
bool elyssa_imu_set_tap_threshold(uint8_t level);  // 1..31, 1 step = range / 32 (default 12)
bool elyssa_imu_set_tap_axes(bool x, bool y, bool z);
bool elyssa_imu_set_tap_timing(uint8_t shock, uint8_t quiet, uint8_t duration);  // 0..3, 0..3, 0..15
ElyssaDirection elyssa_imu_tap_direction();        // of the last tap: the axis that shook the most

// =====================================================================
//  6. Free-fall and motion tuning
// =====================================================================
bool elyssa_imu_disable_freefall();
bool elyssa_imu_set_freefall_threshold(ElyssaFreeFallThreshold threshold);
bool elyssa_imu_set_freefall_duration(uint8_t samples);   // 0..63 (default 6)
bool elyssa_imu_disable_motion();
bool elyssa_imu_set_motion_duration(uint8_t samples);     // 0..3 samples above the threshold
ElyssaDirection elyssa_imu_motion_direction();            // axis of the last movement

// =====================================================================
//  7. Stillness (uses the motion sensitivity)
// =====================================================================
bool  elyssa_imu_enable_still(ElyssaStillMode mode = ELYSSA_STILL_KEEP_GYRO);
bool  elyssa_imu_disable_still();
bool  elyssa_imu_is_still();                    // true while the board does not move
bool  elyssa_imu_set_still_time(float seconds); // rounded to steps of 512 samples
float elyssa_imu_get_still_time();

// =====================================================================
//  8. Orientation-change events (elyssa_imu_orientation() works without them)
// =====================================================================
bool elyssa_imu_enable_orientation_events();
bool elyssa_imu_disable_orientation_events();
bool elyssa_imu_orientation_changed();          // true once after the board changes side
bool elyssa_imu_set_orientation_angle(ElyssaOrientationAngle angle);

// =====================================================================
//  9. Steps (start with elyssa_imu_start_steps())
// =====================================================================
bool     elyssa_imu_stop_steps();                          // not while significant motion is on
bool     elyssa_imu_step_detected();                       // true once per step
bool     elyssa_imu_set_step_threshold(uint8_t level);      // 0..31 (default 16), higher = harder steps
bool     elyssa_imu_set_step_debounce(uint8_t steps, uint16_t ms);  // 0..7 steps, 0..2480 ms
uint32_t elyssa_imu_last_step_time();                       // microseconds (timestamp must be on)
bool     elyssa_imu_set_step_report(float seconds);          // 0 = off, steps of 1.64 s
bool     elyssa_imu_step_report_ready();

// =====================================================================
//  10. Tilt, wrist tilt, significant motion (need 26 Hz or more)
// =====================================================================
bool elyssa_imu_enable_tilt();
bool elyssa_imu_disable_tilt();
bool elyssa_imu_tilted();                       // tilted more than ~35 degrees and held

bool elyssa_imu_enable_wrist_tilt();            // "raise to look" gesture (choose the axes first)
bool elyssa_imu_disable_wrist_tilt();
bool elyssa_imu_wrist_tilted();
ElyssaDirection elyssa_imu_wrist_tilt_direction();
bool elyssa_imu_set_wrist_tilt(uint16_t mg, uint16_t ms);   // threshold 0..3984 mg, hold time 0..10200 ms
bool elyssa_imu_set_wrist_tilt_axes(bool xPos, bool xNeg, bool yPos, bool yNeg, bool zPos, bool zNeg);  // Z+ = lying flat!

bool    elyssa_imu_enable_significant_motion();  // someone walks away with the board
bool    elyssa_imu_disable_significant_motion();
bool    elyssa_imu_significant_motion();
bool    elyssa_imu_set_significant_motion_steps(uint8_t steps);  // default 6

// =====================================================================
//  11. Interrupt pin (INT1 -> GPIO21)
// =====================================================================
void elyssa_imu_on_interrupt(void (*callback)());   // keep the function very short!
void elyssa_imu_stop_interrupt();
bool elyssa_imu_interrupt_active();                 // current state of the pin
bool elyssa_imu_enable_data_ready_interrupt(bool on);  // a pulse at every new sample
bool elyssa_imu_set_events_latched(bool latched);   // see README: tap needs false

// =====================================================================
//  12. Recording (FIFO: up to 341 samples of acceleration + rotation)
// =====================================================================
bool     elyssa_imu_start_recording(ElyssaRecordMode mode = ELYSSA_RECORD_KEEP_LATEST);
bool     elyssa_imu_stop_recording();                      // also clears the recording
uint16_t elyssa_imu_recorded_samples();
bool     elyssa_imu_read_recorded(float accel[3], float gyro[3]);  // oldest sample, false if empty
bool     elyssa_imu_recording_full();
bool     elyssa_imu_set_recording_watermark(uint16_t samples);    // 0..341
bool     elyssa_imu_watermark_reached();

// =====================================================================
//  13. Expert: ST driver context (all lsm6ds3tr_c_...() functions)
// =====================================================================
stmdev_ctx_t *elyssa_imu_st();

#endif
