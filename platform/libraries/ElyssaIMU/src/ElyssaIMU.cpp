/*
  ElyssaIMU - advanced IMU functions for the Elyssa board (Moovma)
  Version 2.0.2 - see ElyssaIMU.h and README.md
  Uses the core's ST driver context: same IMU state as the elyssa_imu_...()
  functions of the board package.
*/
#include "ElyssaIMU.h"

#define CTX     elyssa_imu_st_ctx()
#define OK(x)   ((x) == 0)
#define READY() elyssa_imu_started()

static const float ODR_HZ[] = {0, 12.5f, 26, 52, 104, 208, 416, 833, 1660, 3330, 6660, 1.6f};

static bool ts_fine = false;           // timestamp resolution
static void (*irq_callback)() = nullptr;

// ST driver bug: lsm6ds3tr_c_motion_sens_set(0) and lsm6ds3tr_c_timestamp_set(0)
// only write the register when ENABLING, so they can't turn the feature off.
// This clears the bit in CTRL10_C directly (read, clear, write).
static bool ctrl10_clear(uint8_t mask) {
  uint8_t v;
  if (!OK(lsm6ds3tr_c_read_reg(CTX, LSM6DS3TR_C_CTRL10_C, &v, 1))) return false;
  v &= (uint8_t)~mask;
  return OK(lsm6ds3tr_c_write_reg(CTX, LSM6DS3TR_C_CTRL10_C, &v, 1));
}
#define CTRL10_SIGN_MOTION_EN 0x01
#define CTRL10_TIMER_EN       0x20

// ---------- Helpers ----------

// Next rate at or above hz: code 1..10 (12.5 .. 6660 Hz), 0 = invalid
static uint8_t rate_code(float hz) {
  if (hz <= 0 || hz > 6660) return 0;
  uint8_t c = 1;
  while (c < 10 && ODR_HZ[c] < hz) c++;
  return c;
}

static bool route_int1(void (*edit)(lsm6ds3tr_c_int1_route_t &, bool), bool on) {
  lsm6ds3tr_c_int1_route_t r;
  if (!OK(lsm6ds3tr_c_pin_int1_route_get(CTX, &r))) return false;
  edit(r, on);
  return OK(lsm6ds3tr_c_pin_int1_route_set(CTX, r));
}

// Embedded functions (tilt, wrist tilt, significant motion) need >= 26 Hz
static bool need_26hz() {
  lsm6ds3tr_c_odr_xl_t odr;
  if (!OK(lsm6ds3tr_c_xl_data_rate_get(CTX, &odr))) return false;
  if (odr >= LSM6DS3TR_C_XL_ODR_26Hz && odr <= LSM6DS3TR_C_XL_ODR_6k66Hz) return true;
  return elyssa_imu_set_accel_rate(26);
}

static float accel_g_per_lsb() {
  return 0.061e-3f * (elyssa_imu_get_accel_range() / 2.0f);
}

static float gyro_dps_per_lsb() {
  switch (elyssa_imu_get_gyro_range()) {
    case 125:  return 4.375e-3f;
    case 500:  return 17.5e-3f;
    case 1000: return 35.0e-3f;
    case 2000: return 70.0e-3f;
    default:   return 8.75e-3f;
  }
}

static ElyssaDirection axis_dir(bool x, bool y, bool z, bool negative) {
  if (x) return negative ? ELYSSA_X_NEGATIVE : ELYSSA_X_POSITIVE;
  if (y) return negative ? ELYSSA_Y_NEGATIVE : ELYSSA_Y_POSITIVE;
  if (z) return negative ? ELYSSA_Z_NEGATIVE : ELYSSA_Z_POSITIVE;
  return ELYSSA_DIRECTION_NONE;
}

// Average n raw samples of one sensor, waiting for the data-ready flag
static bool average_raw(bool accel, uint8_t n, float out[3]) {
  float s[3] = {0, 0, 0};
  uint8_t got = 0;
  uint32_t t = millis();
  while (got < n) {
    if (millis() - t > 1500) return false;
    lsm6ds3tr_c_status_reg_t st;
    if (!OK(lsm6ds3tr_c_status_reg_get(CTX, &st))) return false;
    if (!(accel ? st.xlda : st.gda)) { delay(1); continue; }
    int16_t r[3];
    if (!OK(accel ? lsm6ds3tr_c_acceleration_raw_get(CTX, r) : lsm6ds3tr_c_angular_rate_raw_get(CTX, r)))
      return false;
    for (int i = 0; i < 3; i++) s[i] += r[i];
    got++;
  }
  for (int i = 0; i < 3; i++) out[i] = s[i] / n;
  return true;
}

// =====================================================================
//  1. Sensors and power
// =====================================================================
bool elyssa_imu_enable_accel(bool on) {
  if (!READY()) return false;
  if (!on) return OK(lsm6ds3tr_c_xl_data_rate_set(CTX, LSM6DS3TR_C_XL_ODR_OFF));
  return elyssa_imu_set_accel_rate(elyssa_imu_get_rate());
}

static float gyro_rate_memory = 104;

bool elyssa_imu_enable_gyro(bool on) {
  if (!READY()) return false;
  if (!on) {
    gyro_rate_memory = elyssa_imu_get_gyro_rate();
    if (gyro_rate_memory <= 0) gyro_rate_memory = 104;
    return OK(lsm6ds3tr_c_gy_data_rate_set(CTX, LSM6DS3TR_C_GY_ODR_OFF));
  }
  return elyssa_imu_set_gyro_rate(gyro_rate_memory);
}

bool elyssa_imu_gyro_sleep(bool sleep) {
  return READY() && OK(lsm6ds3tr_c_gy_sleep_mode_set(CTX, sleep ? 1 : 0));
}

bool elyssa_imu_set_accel_rate(float hz) {
  if (!READY()) return false;
  uint8_t code = (hz > 1.5f && hz < 1.7f) ? 11 : rate_code(hz);   // 1.6 Hz: low power only
  if (code == 0) return false;
  if (!OK(lsm6ds3tr_c_xl_data_rate_set(CTX, (lsm6ds3tr_c_odr_xl_t)code))) return false;
  elyssa_imu_note_accel_rate(code == 11 ? 1 : code);
  return true;
}

bool elyssa_imu_set_gyro_rate(float hz) {
  uint8_t code = rate_code(hz);
  return READY() && code && OK(lsm6ds3tr_c_gy_data_rate_set(CTX, (lsm6ds3tr_c_odr_g_t)code));
}

float elyssa_imu_get_gyro_rate() {
  lsm6ds3tr_c_odr_g_t v;
  if (!READY() || !OK(lsm6ds3tr_c_gy_data_rate_get(CTX, &v)) || v > 10) return 0;
  return ODR_HZ[v];
}

// =====================================================================
//  2. Calibration and self-test
// =====================================================================
bool elyssa_imu_set_accel_offset(float x, float y, float z) {
  if (!READY()) return false;
  float m = fmaxf(fabsf(x), fmaxf(fabsf(y), fabsf(z)));
  bool fine = m <= 127.0f / 1024.0f;                       // 2^-10 g per step
  float step = fine ? (1.0f / 1024.0f) : (1.0f / 64.0f);   // or 2^-6 g
  if (m > 127.0f * step) return false;
  if (!OK(lsm6ds3tr_c_xl_offset_weight_set(CTX, fine ? LSM6DS3TR_C_LSb_1mg : LSM6DS3TR_C_LSb_16mg)))
    return false;
  // Measured on Elyssa v5: the register value is ADDED on X and Y but
  // SUBTRACTED on Z (datasheet says "added"): the sign of Z is inverted here
  // so that the user's value is always added to the readings.
  uint8_t v[3] = {(uint8_t)(int8_t)lroundf(x / step), (uint8_t)(int8_t)lroundf(y / step),
                  (uint8_t)(int8_t)lroundf(-z / step)};
  return OK(lsm6ds3tr_c_xl_usr_offset_set(CTX, v));
}

bool elyssa_imu_get_accel_offset(float &x, float &y, float &z) {
  lsm6ds3tr_c_usr_off_w_t w;
  uint8_t v[3];
  if (!READY() || !OK(lsm6ds3tr_c_xl_offset_weight_get(CTX, &w)) ||
      !OK(lsm6ds3tr_c_xl_usr_offset_get(CTX, v)))
    return false;
  float step = (w == LSM6DS3TR_C_LSb_16mg) ? (1.0f / 64.0f) : (1.0f / 1024.0f);
  x = (int8_t)v[0] * step; y = (int8_t)v[1] * step; z = -(int8_t)v[2] * step;   // Z inverted (see set)
  return true;
}

bool elyssa_imu_clear_accel_offset() { return elyssa_imu_set_accel_offset(0, 0, 0); }

bool elyssa_imu_calibrate_accel(uint16_t ms) {
  if (!READY() || !elyssa_imu_clear_accel_offset()) return false;
  delay(50);
  double s[3] = {0, 0, 0};
  uint32_t n = 0, t = millis();
  while (millis() - t < ms) {
    float a[3];
    if (elyssa_imu_ready() && elyssa_imu_read_accel(a[0], a[1], a[2])) {
      s[0] += a[0]; s[1] += a[1]; s[2] += a[2];
      n++;
    }
  }
  if (n < 10) return false;
  float mx = s[0] / n, my = s[1] / n, mz = s[2] / n;
  // The board must lie flat, face up: expected 0, 0, +1 g
  if (fabsf(mx) > 0.3f || fabsf(my) > 0.3f || fabsf(mz - 1.0f) > 0.3f) return false;
  return elyssa_imu_set_accel_offset(-mx, -my, 1.0f - mz);
}

void elyssa_imu_set_gyro_offset(float x, float y, float z) {
  float *o = elyssa_imu_gyro_offset_data();
  o[0] = x; o[1] = y; o[2] = z;
}

void elyssa_imu_get_gyro_offset(float &x, float &y, float &z) {
  float *o = elyssa_imu_gyro_offset_data();
  x = o[0]; y = o[1]; z = o[2];
}

void elyssa_imu_clear_gyro_offset() { elyssa_imu_set_gyro_offset(0, 0, 0); }

// Self-test: ST's procedure (example lsm6ds3tr_c_self_test.c, datasheet limits).
// The settings and the gyro calibration are restored afterwards.
struct SavedSettings {
  uint8_t accel_range;
  uint16_t gyro_range;
  lsm6ds3tr_c_odr_xl_t xl_odr;
  lsm6ds3tr_c_odr_g_t gy_odr;
  float offset[3];
};

static void save_settings(SavedSettings &s) {
  s.accel_range = elyssa_imu_get_accel_range();
  s.gyro_range = elyssa_imu_get_gyro_range();
  lsm6ds3tr_c_xl_data_rate_get(CTX, &s.xl_odr);
  lsm6ds3tr_c_gy_data_rate_get(CTX, &s.gy_odr);
  elyssa_imu_get_gyro_offset(s.offset[0], s.offset[1], s.offset[2]);
}

static void restore_settings(const SavedSettings &s) {
  lsm6ds3tr_c_xl_data_rate_set(CTX, s.xl_odr);
  lsm6ds3tr_c_gy_data_rate_set(CTX, s.gy_odr);
  elyssa_imu_set_accel_range(s.accel_range);
  elyssa_imu_set_gyro_range(s.gyro_range);               // clears the offset...
  elyssa_imu_set_gyro_offset(s.offset[0], s.offset[1], s.offset[2]);   // ...put it back
}

bool elyssa_imu_self_test_accel(float result[3]) {
  if (!READY()) return false;
  SavedSettings saved;
  save_settings(saved);
  bool pass = false;
  float off[3], on[3];
  if (OK(lsm6ds3tr_c_xl_full_scale_set(CTX, LSM6DS3TR_C_4g)) &&
      OK(lsm6ds3tr_c_xl_data_rate_set(CTX, LSM6DS3TR_C_XL_ODR_52Hz))) {
    delay(100);
    if (average_raw(true, 1, off) && average_raw(true, 5, off) &&
        OK(lsm6ds3tr_c_xl_self_test_set(CTX, LSM6DS3TR_C_XL_ST_POSITIVE))) {
      delay(100);
      if (average_raw(true, 1, on) && average_raw(true, 5, on)) {
        pass = true;
        for (int i = 0; i < 3; i++) {
          float mg = fabsf(on[i] - off[i]) * 0.122f;   // mg per LSB at 4 g
          if (result) result[i] = mg;
          if (mg < 90.0f || mg > 1700.0f) pass = false;
        }
      }
    }
  }
  lsm6ds3tr_c_xl_self_test_set(CTX, LSM6DS3TR_C_XL_ST_DISABLE);
  delay(100);
  restore_settings(saved);
  return pass;
}

bool elyssa_imu_self_test_gyro(float result[3]) {
  if (!READY()) return false;
  SavedSettings saved;
  save_settings(saved);
  bool pass = false;
  float off[3], on[3];
  if (OK(lsm6ds3tr_c_gy_full_scale_set(CTX, LSM6DS3TR_C_2000dps)) &&
      OK(lsm6ds3tr_c_gy_data_rate_set(CTX, LSM6DS3TR_C_GY_ODR_208Hz))) {
    delay(150);
    if (average_raw(false, 1, off) && average_raw(false, 5, off) &&
        OK(lsm6ds3tr_c_gy_self_test_set(CTX, LSM6DS3TR_C_GY_ST_POSITIVE))) {
      delay(50);
      if (average_raw(false, 1, on) && average_raw(false, 5, on)) {
        pass = true;
        for (int i = 0; i < 3; i++) {
          float dps = fabsf(on[i] - off[i]) * 0.070f;  // dps per LSB at 2000 dps
          if (result) result[i] = dps;
          if (dps < 150.0f || dps > 700.0f) pass = false;
        }
      }
    }
  }
  lsm6ds3tr_c_gy_self_test_set(CTX, LSM6DS3TR_C_GY_ST_DISABLE);
  delay(50);
  restore_settings(saved);
  return pass;
}

bool elyssa_imu_self_test() {
  bool a = elyssa_imu_self_test_accel();
  bool g = elyssa_imu_self_test_gyro();
  return a && g;
}

// =====================================================================
//  3. Timestamp
// =====================================================================
bool elyssa_imu_enable_timestamp(bool on, bool fine) {
  if (!READY()) return false;
  if (!on) return ctrl10_clear(CTRL10_TIMER_EN);   // ST's timestamp_set(0) does nothing
  if (!OK(lsm6ds3tr_c_timestamp_res_set(CTX, fine ? LSM6DS3TR_C_LSB_25us : LSM6DS3TR_C_LSB_6ms4)))
    return false;
  if (!OK(lsm6ds3tr_c_timestamp_set(CTX, 1))) return false;
  if (on) ts_fine = fine;
  return true;
}

uint64_t elyssa_imu_timestamp() {
  uint8_t b[3];
  if (!READY() || !OK(lsm6ds3tr_c_read_reg(CTX, LSM6DS3TR_C_TIMESTAMP0_REG, b, 3))) return 0;
  uint32_t ticks = (uint32_t)b[0] | ((uint32_t)b[1] << 8) | ((uint32_t)b[2] << 16);
  return (uint64_t)ticks * (ts_fine ? 25 : 6400);
}

bool elyssa_imu_reset_timestamp() {
  uint8_t v = 0xAA;   // datasheet: writing AAh to TIMESTAMP2_REG resets the counter
  return READY() && OK(lsm6ds3tr_c_write_reg(CTX, LSM6DS3TR_C_TIMESTAMP2_REG, &v, 1));
}

// =====================================================================
//  4. Filters
// =====================================================================
bool elyssa_imu_set_accel_filter(ElyssaAccelFilter f) {
  if (!READY()) return false;
  switch (f) {
    case ELYSSA_ACCEL_FILTER_OFF:
      return OK(lsm6ds3tr_c_xl_lp1_bandwidth_set(CTX, LSM6DS3TR_C_XL_LP1_ODR_DIV_2));
    case ELYSSA_ACCEL_SMOOTH_LIGHT:
    case ELYSSA_ACCEL_SMOOTH_MEDIUM:
    case ELYSSA_ACCEL_SMOOTH_STRONG:
    case ELYSSA_ACCEL_SMOOTH_MAX: {
      static const lsm6ds3tr_c_input_composite_t LP[] = {
        LSM6DS3TR_C_XL_LOW_NOISE_LP_ODR_DIV_9, LSM6DS3TR_C_XL_LOW_NOISE_LP_ODR_DIV_50,
        LSM6DS3TR_C_XL_LOW_NOISE_LP_ODR_DIV_100, LSM6DS3TR_C_XL_LOW_NOISE_LP_ODR_DIV_400};
      // Datasheet: FUNC_EN also enables the accelerometer LPF2 and high-pass filters
      return OK(lsm6ds3tr_c_func_en_set(CTX, PROPERTY_ENABLE)) &&
             OK(lsm6ds3tr_c_xl_lp2_bandwidth_set(CTX, LP[f - ELYSSA_ACCEL_SMOOTH_LIGHT]));
    }
    case ELYSSA_ACCEL_REMOVE_GRAVITY:
      return OK(lsm6ds3tr_c_func_en_set(CTX, PROPERTY_ENABLE)) &&
             OK(lsm6ds3tr_c_xl_hp_bandwidth_set(CTX, LSM6DS3TR_C_XL_HP_ODR_DIV_100));
    case ELYSSA_ACCEL_REMOVE_GRAVITY_FAST:
      return OK(lsm6ds3tr_c_func_en_set(CTX, PROPERTY_ENABLE)) &&
             OK(lsm6ds3tr_c_xl_hp_bandwidth_set(CTX, LSM6DS3TR_C_XL_HP_ODR_DIV_9));
    default:
      return false;
  }
}

bool elyssa_imu_set_accel_analog_400hz(bool on) {
  return READY() && OK(lsm6ds3tr_c_xl_filter_analog_set(CTX, on ? LSM6DS3TR_C_XL_ANA_BW_400Hz
                                                                : LSM6DS3TR_C_XL_ANA_BW_1k5Hz));
}

// Gyro filter register = high-pass part | low-pass part (ST's band-pass enum)
static ElyssaGyroSmoothing gyro_lp = ELYSSA_GYRO_SMOOTH_OFF;
static ElyssaGyroHighPass gyro_hp = ELYSSA_GYRO_HIGHPASS_OFF;

static bool apply_gyro_filter() {
  static const uint8_t HP[] = {0x00, 0x80, 0x90, 0xA0, 0xB0};
  // LPF1 on (bit 3) + FTYPE, from widest to narrowest bandwidth (datasheet Table 68)
  static const uint8_t LP[] = {0x00, 0x08 | 3, 0x08 | 0, 0x08 | 1, 0x08 | 2};
  return READY() && OK(lsm6ds3tr_c_gy_band_pass_set(CTX, (lsm6ds3tr_c_lpf1_sel_g_t)(HP[gyro_hp] | LP[gyro_lp])));
}

bool elyssa_imu_set_gyro_smoothing(ElyssaGyroSmoothing level) {
  if (level > ELYSSA_GYRO_SMOOTH_MAX) return false;
  gyro_lp = level;
  return apply_gyro_filter();
}

bool elyssa_imu_set_gyro_highpass(ElyssaGyroHighPass cutoff) {
  if (cutoff > ELYSSA_GYRO_HIGHPASS_1HZ) return false;
  gyro_hp = cutoff;
  return apply_gyro_filter();
}

// =====================================================================
//  5. Tap tuning
// =====================================================================
bool elyssa_imu_disable_tap() {
  if (!READY()) return false;
  lsm6ds3tr_c_tap_detection_on_x_set(CTX, 0);
  lsm6ds3tr_c_tap_detection_on_y_set(CTX, 0);
  lsm6ds3tr_c_tap_detection_on_z_set(CTX, 0);
  return route_int1([](lsm6ds3tr_c_int1_route_t &r, bool) {
    r.int1_single_tap = 0;
    r.int1_double_tap = 0;
  }, false);
}

bool elyssa_imu_set_tap_threshold(uint8_t level) {
  return READY() && level >= 1 && level <= 31 && OK(lsm6ds3tr_c_tap_threshold_x_set(CTX, level));
}

bool elyssa_imu_set_tap_axes(bool x, bool y, bool z) {
  return READY() && OK(lsm6ds3tr_c_tap_detection_on_x_set(CTX, x)) &&
         OK(lsm6ds3tr_c_tap_detection_on_y_set(CTX, y)) && OK(lsm6ds3tr_c_tap_detection_on_z_set(CTX, z));
}

bool elyssa_imu_set_tap_timing(uint8_t shock, uint8_t quiet, uint8_t duration) {
  if (!READY() || shock > 3 || quiet > 3 || duration > 15) return false;
  return OK(lsm6ds3tr_c_tap_shock_set(CTX, shock)) && OK(lsm6ds3tr_c_tap_quiet_set(CTX, quiet)) &&
         OK(lsm6ds3tr_c_tap_dur_set(CTX, duration));
}

ElyssaDirection elyssa_imu_tap_direction() {
  uint8_t t = elyssa_imu_event_info()->tap_src;   // z b0, y b1, x b2, sign b3
  return axis_dir(t & 0x04, t & 0x02, t & 0x01, t & 0x08);
}

// =====================================================================
//  6. Free-fall and motion tuning
// =====================================================================
bool elyssa_imu_disable_freefall() {
  return READY() && route_int1([](lsm6ds3tr_c_int1_route_t &r, bool v) { r.int1_ff = v; }, false);
}

bool elyssa_imu_set_freefall_threshold(ElyssaFreeFallThreshold threshold) {
  return READY() && threshold <= ELYSSA_FREEFALL_500MG &&
         OK(lsm6ds3tr_c_ff_threshold_set(CTX, (lsm6ds3tr_c_ff_ths_t)threshold));
}

bool elyssa_imu_set_freefall_duration(uint8_t samples) {
  return READY() && samples <= 63 && OK(lsm6ds3tr_c_ff_dur_set(CTX, samples));
}

bool elyssa_imu_disable_motion() {
  return READY() && route_int1([](lsm6ds3tr_c_int1_route_t &r, bool v) { r.int1_wu = v; }, false);
}

bool elyssa_imu_set_motion_duration(uint8_t samples) {
  return READY() && samples <= 3 && OK(lsm6ds3tr_c_wkup_dur_set(CTX, samples));
}

ElyssaDirection elyssa_imu_motion_direction() {
  uint8_t w = elyssa_imu_event_info()->wake_up_src;   // z b0, y b1, x b2
  return axis_dir(w & 0x04, w & 0x02, w & 0x01, false);
}

// =====================================================================
//  7. Stillness
// =====================================================================
static uint8_t still_steps = 0;

bool elyssa_imu_enable_still(ElyssaStillMode mode) {
  static const lsm6ds3tr_c_inact_en_t M[] = {
    LSM6DS3TR_C_XL_12Hz5_GY_NOT_AFFECTED, LSM6DS3TR_C_XL_12Hz5_GY_SLEEP, LSM6DS3TR_C_XL_12Hz5_GY_PD};
  if (!READY() || mode > ELYSSA_STILL_GYRO_OFF) return false;
  uint8_t ths = 0;
  lsm6ds3tr_c_wkup_threshold_get(CTX, &ths);
  if (ths == 0 && !OK(lsm6ds3tr_c_wkup_threshold_set(CTX, 2))) return false;  // activity threshold
  return OK(lsm6ds3tr_c_act_sleep_dur_set(CTX, still_steps)) &&
         OK(lsm6ds3tr_c_act_mode_set(CTX, M[mode])) &&
         route_int1([](lsm6ds3tr_c_int1_route_t &r, bool v) { r.int1_inact_state = v; }, true);
}

bool elyssa_imu_disable_still() {
  return READY() && OK(lsm6ds3tr_c_act_mode_set(CTX, LSM6DS3TR_C_PROPERTY_DISABLE)) &&
         route_int1([](lsm6ds3tr_c_int1_route_t &r, bool v) { r.int1_inact_state = v; }, false);
}

bool elyssa_imu_is_still() { return READY() && elyssa_imu_event_info()->still; }

bool elyssa_imu_set_still_time(float seconds) {
  float odr = elyssa_imu_get_rate();
  if (!READY() || odr <= 0 || seconds < 0) return false;
  long steps = lroundf(seconds * odr / 512.0f);   // 1 step = 512 samples
  if (steps > 15) steps = 15;
  if (!OK(lsm6ds3tr_c_act_sleep_dur_set(CTX, (uint8_t)steps))) return false;
  still_steps = (uint8_t)steps;
  return true;
}

float elyssa_imu_get_still_time() {
  float odr = elyssa_imu_get_rate();
  if (odr <= 0) return 0;
  return still_steps == 0 ? 16.0f / odr : still_steps * 512.0f / odr;
}

// =====================================================================
//  8. Orientation-change events
// =====================================================================
bool elyssa_imu_enable_orientation_events() {
  if (!READY()) return false;
  elyssa_imu_take_event(ELYSSA_EV_ORIENTATION);   // forget an old event
  return OK(lsm6ds3tr_c_4d_mode_set(CTX, PROPERTY_DISABLE)) &&   // all 6 sides
         route_int1([](lsm6ds3tr_c_int1_route_t &r, bool v) { r.int1_6d = v; }, true);
}

bool elyssa_imu_disable_orientation_events() {
  return READY() && route_int1([](lsm6ds3tr_c_int1_route_t &r, bool v) { r.int1_6d = v; }, false);
}

bool elyssa_imu_orientation_changed() { return READY() && elyssa_imu_take_event(ELYSSA_EV_ORIENTATION); }

bool elyssa_imu_set_orientation_angle(ElyssaOrientationAngle angle) {
  return READY() && angle <= ELYSSA_ANGLE_50 && OK(lsm6ds3tr_c_6d_threshold_set(CTX, (lsm6ds3tr_c_sixd_ths_t)angle));
}

// =====================================================================
//  9. Steps
// =====================================================================
bool elyssa_imu_stop_steps() { return READY() && OK(lsm6ds3tr_c_pedo_sens_set(CTX, PROPERTY_DISABLE)); }

bool elyssa_imu_step_detected() { return READY() && elyssa_imu_take_event(ELYSSA_EV_STEP); }

bool elyssa_imu_set_step_threshold(uint8_t level) {
  return READY() && level <= 31 && OK(lsm6ds3tr_c_pedo_threshold_set(CTX, level));
}

bool elyssa_imu_set_step_debounce(uint8_t steps, uint16_t ms) {
  long t = lroundf(ms / 80.0f);   // 1 step = 80 ms
  return READY() && steps <= 7 && t <= 31 && OK(lsm6ds3tr_c_pedo_debounce_steps_set(CTX, steps)) &&
         OK(lsm6ds3tr_c_pedo_timeout_set(CTX, (uint8_t)t));
}

uint32_t elyssa_imu_last_step_time() {
  uint8_t b[2];
  if (!READY() || !OK(lsm6ds3tr_c_read_reg(CTX, LSM6DS3TR_C_STEP_TIMESTAMP_L, b, 2))) return 0;
  return ((uint32_t)b[0] | ((uint32_t)b[1] << 8)) * (ts_fine ? 25 : 6400);
}

bool elyssa_imu_set_step_report(float seconds) {
  long v = lroundf(seconds / 1.6384f);   // 1 step = 1.6384 s
  if (!READY() || seconds < 0 || v > 255) return false;
  uint8_t b = (uint8_t)v;
  if (!OK(lsm6ds3tr_c_pedo_steps_period_set(CTX, &b))) return false;
  // Datasheet: needs the timer on, with 6.4 ms resolution
  return b == 0 || elyssa_imu_enable_timestamp(true, false);
}

bool elyssa_imu_step_report_ready() { return READY() && elyssa_imu_take_event(ELYSSA_EV_STEP_REPORT); }

// =====================================================================
//  10. Tilt, wrist tilt, significant motion
// =====================================================================
bool elyssa_imu_enable_tilt() {
  if (!READY() || !need_26hz()) return false;
  elyssa_imu_take_event(ELYSSA_EV_TILT);
  return OK(lsm6ds3tr_c_tilt_sens_set(CTX, PROPERTY_ENABLE)) &&
         route_int1([](lsm6ds3tr_c_int1_route_t &r, bool v) { r.int1_tilt = v; }, true);
}

bool elyssa_imu_disable_tilt() {
  return READY() && OK(lsm6ds3tr_c_tilt_sens_set(CTX, PROPERTY_DISABLE)) &&
         route_int1([](lsm6ds3tr_c_int1_route_t &r, bool v) { r.int1_tilt = v; }, false);
}

bool elyssa_imu_tilted() { return READY() && elyssa_imu_take_event(ELYSSA_EV_TILT); }

// Wrist tilt can only be routed to INT2 (not connected on Elyssa): polled
bool elyssa_imu_enable_wrist_tilt() {
  if (!READY() || !need_26hz()) return false;
  elyssa_imu_take_event(ELYSSA_EV_WRIST_TILT);
  return OK(lsm6ds3tr_c_wrist_tilt_sens_set(CTX, PROPERTY_ENABLE));
}

bool elyssa_imu_disable_wrist_tilt() {
  return READY() && OK(lsm6ds3tr_c_wrist_tilt_sens_set(CTX, PROPERTY_DISABLE));
}

bool elyssa_imu_wrist_tilted() { return READY() && elyssa_imu_take_event(ELYSSA_EV_WRIST_TILT); }

ElyssaDirection elyssa_imu_wrist_tilt_direction() {
  uint8_t w = elyssa_imu_event_info()->wrist_tilt_ia;   // zneg b2, zpos b3, yneg b4, ypos b5, xneg b6, xpos b7
  if (w & 0x80) return ELYSSA_X_POSITIVE;
  if (w & 0x40) return ELYSSA_X_NEGATIVE;
  if (w & 0x20) return ELYSSA_Y_POSITIVE;
  if (w & 0x10) return ELYSSA_Y_NEGATIVE;
  if (w & 0x08) return ELYSSA_Z_POSITIVE;
  if (w & 0x04) return ELYSSA_Z_NEGATIVE;
  return ELYSSA_DIRECTION_NONE;
}

bool elyssa_imu_set_wrist_tilt(uint16_t mg, uint16_t ms) {
  long t = lroundf(mg / 15.625f), l = lroundf(ms / 40.0f);
  if (!READY() || t > 255 || l > 255) return false;
  uint8_t tb = (uint8_t)t, lb = (uint8_t)l;
  return OK(lsm6ds3tr_c_tilt_threshold_set(CTX, &tb)) && OK(lsm6ds3tr_c_tilt_latency_set(CTX, &lb));
}

bool elyssa_imu_set_wrist_tilt_axes(bool xPos, bool xNeg, bool yPos, bool yNeg, bool zPos, bool zNeg) {
  if (!READY()) return false;
  // ST driver bug: lsm6ds3tr_c_tilt_src_set() READS the register instead of
  // writing it. Write A_WRIST_TILT_MASK (bank B) directly instead.
  uint8_t m = (xPos << 7) | (xNeg << 6) | (yPos << 5) | (yNeg << 4) | (zPos << 3) | (zNeg << 2);
  int32_t ret = lsm6ds3tr_c_mem_bank_set(CTX, LSM6DS3TR_C_BANK_B);
  if (ret == 0) ret = lsm6ds3tr_c_write_reg(CTX, LSM6DS3TR_C_A_WRIST_TILT_MASK, &m, 1);
  ret += lsm6ds3tr_c_mem_bank_set(CTX, LSM6DS3TR_C_USER_BANK);
  return OK(ret);
}

bool elyssa_imu_enable_significant_motion() {
  if (!READY() || !need_26hz()) return false;
  elyssa_imu_take_event(ELYSSA_EV_SIGNIFICANT_MOTION);
  return OK(lsm6ds3tr_c_motion_sens_set(CTX, PROPERTY_ENABLE)) &&
         route_int1([](lsm6ds3tr_c_int1_route_t &r, bool v) { r.int1_sign_mot = v; }, true);
}

bool elyssa_imu_disable_significant_motion() {
  return READY() && ctrl10_clear(CTRL10_SIGN_MOTION_EN) &&   // ST's motion_sens_set(0) does nothing
         route_int1([](lsm6ds3tr_c_int1_route_t &r, bool v) { r.int1_sign_mot = v; }, false);
}

bool elyssa_imu_significant_motion() { return READY() && elyssa_imu_take_event(ELYSSA_EV_SIGNIFICANT_MOTION); }

bool elyssa_imu_set_significant_motion_steps(uint8_t steps) {
  return READY() && OK(lsm6ds3tr_c_motion_threshold_set(CTX, &steps));
}

// =====================================================================
//  11. Interrupt pin
// =====================================================================
void elyssa_imu_on_interrupt(void (*callback)()) {
  if (irq_callback) detachInterrupt(digitalPinToInterrupt(INT_gyro));
  irq_callback = callback;
  if (callback) attachInterrupt(digitalPinToInterrupt(INT_gyro), callback, RISING);
}

void elyssa_imu_stop_interrupt() { elyssa_imu_on_interrupt(nullptr); }

bool elyssa_imu_interrupt_active() { return digitalRead(INT_gyro) == HIGH; }

bool elyssa_imu_enable_data_ready_interrupt(bool on) {
  return READY() && OK(lsm6ds3tr_c_data_ready_mode_set(CTX, LSM6DS3TR_C_DRDY_PULSED)) &&
         route_int1([](lsm6ds3tr_c_int1_route_t &r, bool v) {
           r.int1_drdy_xl = v;
           r.int1_drdy_g = v;
         }, on);
}

bool elyssa_imu_set_events_latched(bool latched) {
  return READY() && OK(lsm6ds3tr_c_int_notification_set(CTX, latched ? LSM6DS3TR_C_INT_LATCHED
                                                                      : LSM6DS3TR_C_INT_PULSED));
}

// =====================================================================
//  12. Recording (FIFO): 1 sample = 6 words, gyro X,Y,Z then accel X,Y,Z
// =====================================================================
static uint16_t wtm_samples = 0;

bool elyssa_imu_start_recording(ElyssaRecordMode mode) {
  if (!READY()) return false;
  lsm6ds3tr_c_odr_xl_t xl;
  lsm6ds3tr_c_odr_g_t gy;
  if (!OK(lsm6ds3tr_c_xl_data_rate_get(CTX, &xl)) || !OK(lsm6ds3tr_c_gy_data_rate_get(CTX, &gy))) return false;
  // Both sensors on and at the same rate (the sample pattern depends on it)
  if (xl == LSM6DS3TR_C_XL_ODR_OFF || xl > LSM6DS3TR_C_XL_ODR_6k66Hz || (uint8_t)xl != (uint8_t)gy) return false;
  return OK(lsm6ds3tr_c_fifo_mode_set(CTX, LSM6DS3TR_C_BYPASS_MODE)) &&   // clears the FIFO
         OK(lsm6ds3tr_c_fifo_xl_batch_set(CTX, LSM6DS3TR_C_FIFO_XL_NO_DEC)) &&
         OK(lsm6ds3tr_c_fifo_gy_batch_set(CTX, LSM6DS3TR_C_FIFO_GY_NO_DEC)) &&
         OK(lsm6ds3tr_c_fifo_data_rate_set(CTX, (lsm6ds3tr_c_odr_fifo_t)xl)) &&
         OK(lsm6ds3tr_c_fifo_watermark_set(CTX, wtm_samples * 6)) &&
         OK(lsm6ds3tr_c_fifo_mode_set(CTX, mode == ELYSSA_RECORD_STOP_WHEN_FULL ? LSM6DS3TR_C_FIFO_MODE
                                                                              : LSM6DS3TR_C_STREAM_MODE));
}

bool elyssa_imu_stop_recording() {
  return READY() && OK(lsm6ds3tr_c_fifo_mode_set(CTX, LSM6DS3TR_C_BYPASS_MODE)) &&
         OK(lsm6ds3tr_c_fifo_data_rate_set(CTX, LSM6DS3TR_C_FIFO_DISABLE));
}

// FIFO_STATUS1..4: level (words), flags, pattern (next word of the 6-word sample)
static bool fifo_status(uint16_t &level, uint16_t &pattern, uint8_t &flags) {
  uint8_t s[4];
  if (!READY() || !OK(lsm6ds3tr_c_read_reg(CTX, LSM6DS3TR_C_FIFO_STATUS1, s, 4))) return false;
  level = (uint16_t)s[0] | ((uint16_t)(s[1] & 0x07) << 8);
  flags = s[1];
  pattern = (uint16_t)s[2] | ((uint16_t)(s[3] & 0x03) << 8);
  return true;
}

uint16_t elyssa_imu_recorded_samples() {
  uint16_t level, pattern;
  uint8_t flags;
  if (!fifo_status(level, pattern, flags)) return 0;
  uint16_t skip = pattern ? (6 - pattern) : 0;
  return level > skip ? (level - skip) / 6 : 0;
}

bool elyssa_imu_read_recorded(float accel[3], float gyro[3]) {
  uint16_t level, pattern;
  uint8_t flags, b[2];
  if (!fifo_status(level, pattern, flags)) return false;
  while (pattern != 0 && level > 0) {          // re-align on the start of a sample
    if (!OK(lsm6ds3tr_c_fifo_raw_data_get(CTX, b, 2))) return false;
    pattern = (pattern + 1) % 6;
    level--;
  }
  if (level < 6) return false;
  int16_t w[6];
  for (int i = 0; i < 6; i++) {               // one word per read
    if (!OK(lsm6ds3tr_c_fifo_raw_data_get(CTX, b, 2))) return false;
    w[i] = (int16_t)(b[0] | (b[1] << 8));
  }
  float ga = accel_g_per_lsb(), gg = gyro_dps_per_lsb();
  float *o = elyssa_imu_gyro_offset_data();
  for (int i = 0; i < 3; i++) {
    if (gyro)  gyro[i]  = w[i] * gg - o[i];
    if (accel) accel[i] = w[3 + i] * ga;
  }
  return true;
}

bool elyssa_imu_recording_full() {
  uint16_t level, pattern;
  uint8_t flags;
  return fifo_status(level, pattern, flags) && (flags & 0x60);   // FIFO_FULL_SMART | OVER_RUN
}

bool elyssa_imu_set_recording_watermark(uint16_t samples) {
  if (!READY() || samples > 341) return false;
  wtm_samples = samples;
  return OK(lsm6ds3tr_c_fifo_watermark_set(CTX, samples * 6));
}

bool elyssa_imu_watermark_reached() {
  uint16_t level, pattern;
  uint8_t flags;
  return fifo_status(level, pattern, flags) && (flags & 0x80);
}

// =====================================================================
//  13. Expert
// =====================================================================
stmdev_ctx_t *elyssa_imu_st() { return CTX; }
