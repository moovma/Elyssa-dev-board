/*
  Elyssa IMU - connection point between the core (variant.cpp) and the
  ElyssaIMU library. NOT for sketches: use the elyssa_imu_...() functions.
*/
#ifndef ELYSSA_IMU_INTERNAL_H
#define ELYSSA_IMU_INTERNAL_H

#include <stdint.h>
#include "lsm6ds3tr-c_reg.h"

#define ELYSSA_IMU_CORE_VERSION 2

// Events recorded by the core (all IMU event registers are read together)
#define ELYSSA_EV_TAP                0x0001
#define ELYSSA_EV_DOUBLE_TAP         0x0002
#define ELYSSA_EV_FALL               0x0004
#define ELYSSA_EV_MOTION             0x0008
#define ELYSSA_EV_ORIENTATION        0x0010
#define ELYSSA_EV_STEP               0x0020
#define ELYSSA_EV_TILT               0x0040
#define ELYSSA_EV_SIGNIFICANT_MOTION 0x0080
#define ELYSSA_EV_WRIST_TILT         0x0100
#define ELYSSA_EV_STEP_REPORT        0x0200

// Details of the last events (raw source registers)
typedef struct {
  uint8_t tap_src;        // TAP_SRC of the last tap
  uint8_t wake_up_src;    // WAKE_UP_SRC of the last motion
  uint8_t d6d_src;        // D6D_SRC of the last orientation change
  uint8_t wrist_tilt_ia;  // WRIST_TILT_IA of the last wrist tilt
  bool    still;          // current activity state (true = still)
} ElyssaImuEventInfo;

#ifdef __cplusplus
stmdev_ctx_t *elyssa_imu_st_ctx();                 // ST driver context of the core
bool elyssa_imu_started();                         // elyssa_imu_begin() succeeded
bool elyssa_imu_take_event(uint16_t ev);           // read events, true once per event
const ElyssaImuEventInfo *elyssa_imu_event_info(); // read events, return details
float *elyssa_imu_gyro_offset_data();              // gyro calibration offset [3], dps
void elyssa_imu_note_accel_rate(uint8_t odr_code); // accel rate changed outside the core
#endif

#endif
