# Elyssa IMU

The Elyssa board has a 6-axis motion sensor (IMU): an **accelerometer** (acceleration, in g) and a **gyroscope** (rotation speed, in degrees per second), plus a temperature sensor. The chip is an STMicroelectronics LSM6DS3TR-C.

You can use it at two levels:

| | Basic functions | Advanced functions |
|---|---|---|
| Where | built into the Elyssa board package | this library, **ElyssaIMU** (also in the board package) |
| `#include` | **none** | `#include <ElyssaIMU.h>` |
| For | reading values, angles, tap, free-fall, motion, steps, wake-up | recording, filters, self-test, calibration offsets, stillness, tilt, wrist tilt, fine tuning |

Both use the same style (`elyssa_imu_...`) and work together in the same sketch. Type `elyssa_imu_` in the Arduino IDE to see them all.

---

## Contents

1. [Quick start](#1-quick-start)
2. [Basic functions (no include)](#2-basic-functions-no-include)
3. [Advanced functions (ElyssaIMU library)](#3-advanced-functions-elyssaimu-library)
4. [Values](#4-values)
5. [Rules to know](#5-rules-to-know)
6. [Examples](#6-examples)
7. [Troubleshooting](#7-troubleshooting)
8. [Expert: ST driver](#8-expert-st-driver)

---

## 1. Quick start

Select **Tools → Board → Elyssa**, then upload:

```cpp
void setup() {
  Serial.begin(115200);
  if (!elyssa_imu_begin()) {          // always first
    Serial.println("IMU not found");
    while (true) delay(1000);
  }
}

void loop() {
  float x, y, z;
  elyssa_imu_read_accel(x, y, z);
  Serial.printf("x %.2f  y %.2f  z %.2f g\n", x, y, z);
  delay(200);
}
```

Board flat, components up: about **0, 0, 1 g** (gravity).

Every IMU sketch follows the same 3 steps:
1. `elyssa_imu_begin()` in `setup()`.
2. Optional settings or features in `setup()` (range, calibration, `enable_...`).
3. Read values or check events in `loop()`.

---

## 2. Basic functions (no include)

### Start
| Function | What it does |
|---|---|
| `elyssa_imu_begin()` | Starts the IMU. `false` if not found. Defaults: ±2 g, ±250 dps, 104 Hz. |
| `elyssa_imu_ready()` | `true` when a new sample is ready. |

### Reading
| Function | What it does |
|---|---|
| `elyssa_imu_accel_x()`, `_y()`, `_z()` | Acceleration of one axis, in g. |
| `elyssa_imu_gyro_x()`, `_y()`, `_z()` | Rotation speed of one axis, in dps. |
| `elyssa_imu_read_accel(x, y, z)` | The 3 axes from the same sample. |
| `elyssa_imu_read_gyro(x, y, z)` | Same for rotation. |
| `elyssa_imu_temperature()` | Temperature in °C. |

### Settings
| Function | What it does |
|---|---|
| `elyssa_imu_set_accel_range(g)` | 2, 4, 8 or 16. Larger = fast movements without clipping, less precise. |
| `elyssa_imu_set_gyro_range(dps)` | 125, 250, 500, 1000 or 2000. Clears the gyro calibration. |
| `elyssa_imu_set_rate(hz)` | 12.5, 26, 52, 104, 208, 416, 833, 1660, 3330, 6660 (other values: next higher). |
| `elyssa_imu_get_accel_range()`, `get_gyro_range()`, `get_rate()` | Current settings. |
| `elyssa_imu_low_power(true)` | Less current, a bit more noise. |

### Angles and orientation
| Function | What it does |
|---|---|
| `elyssa_imu_pitch()` / `elyssa_imu_roll()` | Tilt angles in degrees (board fairly still). |
| `elyssa_imu_orientation()` | Which side faces up: `ELYSSA_FACE_UP`, `ELYSSA_FACE_DOWN`, `ELYSSA_X_UP`, `ELYSSA_X_DOWN`, `ELYSSA_Y_UP`, `ELYSSA_Y_DOWN`, `ELYSSA_ORIENTATION_UNKNOWN`. |

### Calibration
| Function | What it does |
|---|---|
| `elyssa_imu_calibrate_gyro()` | Board still: removes the gyroscope's rotation at rest (default 1 s). |

### Events (each check is `true` once per event)
| Enable in `setup()` | Check in `loop()` |
|---|---|
| `elyssa_imu_enable_tap()` | `elyssa_imu_tapped()`, `elyssa_imu_double_tapped()` |
| `elyssa_imu_enable_freefall()` | `elyssa_imu_fell()` |
| `elyssa_imu_enable_motion(sensitivity)` (1 = most sensitive .. 63, default 2) | `elyssa_imu_moved()` |
| `elyssa_imu_start_steps()` | `elyssa_imu_steps()`, `elyssa_imu_reset_steps()` |

### Deep sleep
| Function | What it does |
|---|---|
| `elyssa_imu_wake_on_motion(sensitivity)` | Call just before `esp_deep_sleep_start()`: moving the board wakes the ESP32 up. |

Older names still work (`accel_return_ax()`, `gyro_return_ax()`, `elyssa_imu_read(gyro, accel, &temp)`, `elyssa_imu_whoami()`, `elyssa_imu_read_reg()`).

---

## 3. Advanced functions (ElyssaIMU library)

Add `#include <ElyssaIMU.h>`, start with `elyssa_imu_begin()` as usual.

### Sensors and power
| Function | What it does |
|---|---|
| `elyssa_imu_enable_accel(on)` / `elyssa_imu_enable_gyro(on)` | Turn one sensor on/off. The gyroscope uses most of the power. |
| `elyssa_imu_gyro_sleep(on)` | Gyroscope standby: less power, wakes up fast. |
| `elyssa_imu_set_accel_rate(hz)` / `elyssa_imu_set_gyro_rate(hz)` | Separate rates. The accelerometer also accepts 1.6 Hz in low power mode. |
| `elyssa_imu_get_gyro_rate()` | Gyroscope rate. |

### Calibration and self-test
| Function | What it does |
|---|---|
| `elyssa_imu_calibrate_accel()` | Board **flat, face up**, still: corrects each axis to read exactly 0, 0, 1 g (stored in the IMU). |
| `elyssa_imu_set_accel_offset(x, y, z)` / `get_` / `clear_` | Correction in g added to every reading (up to ±1.98 g). |
| `elyssa_imu_set_gyro_offset(x, y, z)` / `get_` / `clear_` | Gyroscope correction in dps, e.g. to reuse a saved calibration. |
| `elyssa_imu_self_test_accel(result)` / `_gyro(result)` / `elyssa_imu_self_test()` | Checks the sensors work (board still). `result` (optional, 3 floats) gets the change measured: accelerometer 90..1700 mg, gyroscope 150..700 dps. Settings and calibration are restored. |

### Timestamp
| Function | What it does |
|---|---|
| `elyssa_imu_enable_timestamp(on, fine)` | The IMU's own clock. `fine = true`: 25 µs steps (~7 min); else 6.4 ms (~30 h). |
| `elyssa_imu_timestamp()` | Microseconds since start or reset. |
| `elyssa_imu_reset_timestamp()` | Back to 0. |

### Filters
| Function | What it does |
|---|---|
| `elyssa_imu_set_accel_filter(f)` | `ELYSSA_ACCEL_SMOOTH_LIGHT` .. `_MAX` remove vibrations; `ELYSSA_ACCEL_REMOVE_GRAVITY` (`_FAST`) keeps only movements; `ELYSSA_ACCEL_FILTER_OFF`. Strong smoothing reacts slowly: after turning it on, values need a few seconds to settle (about 3 s for `_MAX` at 104 Hz). |
| `elyssa_imu_set_accel_analog_400hz(on)` | Anti-aliasing 400 Hz instead of 1.5 kHz. |
| `elyssa_imu_set_gyro_smoothing(level)` | `ELYSSA_GYRO_SMOOTH_OFF` .. `_MAX` (useful at 833 Hz and above). |
| `elyssa_imu_set_gyro_highpass(cutoff)` | Removes slow drift: `ELYSSA_GYRO_HIGHPASS_OFF`, `_0_016HZ`, `_0_065HZ`, `_0_26HZ`, `_1HZ`. |

### Tap, free-fall, motion: tuning
| Function | What it does |
|---|---|
| `elyssa_imu_set_tap_threshold(1..31)` | Tap strength needed (1 step = range / 32, default 12). |
| `elyssa_imu_set_tap_axes(x, y, z)` | Which directions count. |
| `elyssa_imu_set_tap_timing(shock, quiet, duration)` | 0..3, 0..3, 0..15 (default 3, 3, 7). |
| `elyssa_imu_tap_direction()` | Direction of the last tap (`ELYSSA_X_POSITIVE`, ...): the axis that shook the most. Approximate: a tap on an edge can also shake the board vertically (Z). |
| `elyssa_imu_disable_tap()` | Stops tap detection. |
| `elyssa_imu_set_freefall_threshold(t)` / `_duration(samples)` / `elyssa_imu_disable_freefall()` | Free-fall tuning (default 312 mg, 6 samples). |
| `elyssa_imu_set_motion_duration(0..3)` / `elyssa_imu_motion_direction()` / `elyssa_imu_disable_motion()` | Motion tuning. |

### Stillness
| Function | What it does |
|---|---|
| `elyssa_imu_enable_still(mode)` / `elyssa_imu_disable_still()` | Detects when the board stops moving. While still, the accelerometer slows to 12.5 Hz and the gyroscope stays on (`ELYSSA_STILL_KEEP_GYRO`), sleeps (`_GYRO_SLEEP`) or turns off (`_GYRO_OFF`). Uses the motion sensitivity. |
| `elyssa_imu_is_still()` | `true` while the board does not move. |
| `elyssa_imu_set_still_time(seconds)` / `get_still_time()` | Time before "still" (steps of 512 samples, ~4.9 s at 104 Hz; 0 = shortest). |

### Orientation events
| Function | What it does |
|---|---|
| `elyssa_imu_enable_orientation_events()` / `disable_` | Events when the board changes side. |
| `elyssa_imu_orientation_changed()` | `true` once after a change. |
| `elyssa_imu_set_orientation_angle(a)` | `ELYSSA_ANGLE_80`, `_70`, `_60` (default), `_50`. |

### Steps
| Function | What it does |
|---|---|
| `elyssa_imu_step_detected()` | `true` once per step. |
| `elyssa_imu_set_step_threshold(0..31)` | Step strength (default 16). |
| `elyssa_imu_set_step_debounce(steps, ms)` | Steps before counting starts (0..7), pause that restarts it (0..2480 ms). |
| `elyssa_imu_last_step_time()` | Time of the last step in µs (timestamp on). |
| `elyssa_imu_set_step_report(seconds)` / `step_report_ready()` | Periodic report (steps of 1.64 s). |
| `elyssa_imu_stop_steps()` | Stops counting. Significant motion uses the step counter: while it is on, steps keep being counted. |

### Tilt, wrist tilt, significant motion
| Function | What it does |
|---|---|
| `elyssa_imu_enable_tilt()` / `disable_` / `elyssa_imu_tilted()` | Tilted more than ~35° and held. |
| `elyssa_imu_enable_wrist_tilt()` / `disable_` / `elyssa_imu_wrist_tilted()` | "Raise to look" gesture. `elyssa_imu_wrist_tilt_direction()`. |
| `elyssa_imu_set_wrist_tilt(mg, ms)` / `set_wrist_tilt_axes(...)` | Threshold, hold time, directions. By default only one direction (X positive) is active: call `set_wrist_tilt_axes()` to choose. Use the edges (X±, Y±): with Z+ on, simply lying flat face up triggers the gesture (Z− = face down). One event per gesture, even if the position is held. |
| `elyssa_imu_enable_significant_motion()` / `disable_` / `elyssa_imu_significant_motion()` | Someone walks away with the board. `set_significant_motion_steps(n)`. |

### Interrupt pin (INT1 → GPIO21)
| Function | What it does |
|---|---|
| `elyssa_imu_on_interrupt(function)` / `stop_interrupt()` | Calls your function on the pin. Keep it very short (count or set a flag). |
| `elyssa_imu_enable_data_ready_interrupt(on)` | A pulse at every new sample. |
| `elyssa_imu_interrupt_active()` | Current state of the pin. |
| `elyssa_imu_set_events_latched(on)` | Events kept until read (see rules). |

### Recording (FIFO)
| Function | What it does |
|---|---|
| `elyssa_imu_start_recording(mode)` | The IMU records by itself (up to 341 samples): `ELYSSA_RECORD_KEEP_LATEST` or `ELYSSA_RECORD_STOP_WHEN_FULL`. Both sensors on, same rate. |
| `elyssa_imu_recorded_samples()` | Samples waiting. |
| `elyssa_imu_read_recorded(accel, gyro)` | Oldest sample (two arrays of 3). `false` when empty. |
| `elyssa_imu_recording_full()` | Buffer full. |
| `elyssa_imu_set_recording_watermark(n)` / `watermark_reached()` | Signal when n samples are recorded. |
| `elyssa_imu_stop_recording()` | Stops and clears. |

---

## 4. Values

| Setting | Values |
|---|---|
| Orientation | `ELYSSA_FACE_UP`, `ELYSSA_FACE_DOWN`, `ELYSSA_X_UP`, `ELYSSA_X_DOWN`, `ELYSSA_Y_UP`, `ELYSSA_Y_DOWN`, `ELYSSA_ORIENTATION_UNKNOWN` |
| Direction | `ELYSSA_DIRECTION_NONE`, `ELYSSA_X_POSITIVE`, `ELYSSA_X_NEGATIVE`, `ELYSSA_Y_POSITIVE`, `ELYSSA_Y_NEGATIVE`, `ELYSSA_Z_POSITIVE`, `ELYSSA_Z_NEGATIVE` |
| Acceleration filter | `ELYSSA_ACCEL_FILTER_OFF`, `ELYSSA_ACCEL_SMOOTH_LIGHT`, `_MEDIUM`, `_STRONG`, `_MAX`, `ELYSSA_ACCEL_REMOVE_GRAVITY`, `ELYSSA_ACCEL_REMOVE_GRAVITY_FAST` |
| Gyroscope smoothing | `ELYSSA_GYRO_SMOOTH_OFF`, `_LIGHT`, `_MEDIUM`, `_STRONG`, `_MAX` |
| Gyroscope high-pass | `ELYSSA_GYRO_HIGHPASS_OFF`, `_0_016HZ`, `_0_065HZ`, `_0_26HZ`, `_1HZ` |
| Free-fall threshold | `ELYSSA_FREEFALL_156MG`, `_219MG`, `_250MG`, `_312MG`, `_344MG`, `_406MG`, `_469MG`, `_500MG` |
| Stillness | `ELYSSA_STILL_KEEP_GYRO`, `ELYSSA_STILL_GYRO_SLEEP`, `ELYSSA_STILL_GYRO_OFF` |
| Orientation angle | `ELYSSA_ANGLE_80`, `_70`, `_60`, `_50` |
| Recording | `ELYSSA_RECORD_KEEP_LATEST`, `ELYSSA_RECORD_STOP_WHEN_FULL` |

---

## 5. Rules to know

1. **`elyssa_imu_begin()` first**, and check that it returns `true`.
2. **Events are `true` once.** Check them in `loop()`.
3. **Tap**: check often (no long `delay()` in `loop()`), and **don't combine tap with free-fall or motion** in the same sketch. The IMU has one "latched events" setting: tap needs it off, free-fall and motion turn it on. The last `enable_` call wins.
4. **Tap also switches orientation events to 4 positions** (ST's tap method); call `elyssa_imu_enable_orientation_events()` after `enable_tap()` to get 6 again.
5. **Some features change the sample rate**: tap sets 416 Hz, steps / tilt / wrist tilt / significant motion need 26 Hz or more.
6. **Changing the gyroscope range clears the gyroscope calibration**: calibrate again after.
7. **Recording** needs both sensors at the same rate.
8. In an interrupt function, never use `Serial` or the IMU: set a flag and do the work in `loop()`.
9. **Significant motion keeps the step counter running**, even after `elyssa_imu_stop_steps()`.

**Known chip / driver behaviours handled by the library (tested on Elyssa v5):** the accelerometer user offset is subtracted on Z (added on X and Y) and the library corrects the sign; tap direction bits are reported without TAP_IA; the wrist-tilt direction must be read before FUNC_SRC2; ST's `lsm6ds3tr_c_tilt_src_set()` reads instead of writing, so the library writes the wrist-tilt mask itself; ST's `lsm6ds3tr_c_motion_sens_set(0)` and `lsm6ds3tr_c_timestamp_set(0)` never write the register, so the library clears those bits itself to turn significant motion and the timestamp off; the wrist-tilt event flag stays set while the position is held and its direction register is cleared after the first read, so the library counts one event per gesture and keeps the direction.

---

## 6. Examples

**File → Examples → ElyssaIMU**

| Basics (no #include) | Advanced |
|---|---|
| `01_ReadValues` | `01_Recording` |
| `02_Calibration` | `02_Filters` |
| `03_AnglesAndOrientation` | `03_SelfTestAndCalibration` |
| `04_Tap` | `04_Stillness` |
| `05_FreeFall` | `05_TiltAndWristTilt` |
| `06_Motion` | `06_InterruptCallback` |
| `07_StepCounter` | `07_Timestamp` |
| `08_WakeOnMotion` | `08_TuneTap` |

---

## 7. Troubleshooting

| Problem | What to check |
|---|---|
| `elyssa_imu_begin()` returns `false` | Board selected = Elyssa? |
| Rotation not 0 at rest | Normal: call `elyssa_imu_calibrate_gyro()` with the board still. |
| Values stop at ~2 g / ~250 dps | Movement too fast for the range: `elyssa_imu_set_accel_range()` / `set_gyro_range()`. |
| Taps not detected | Check often, no free-fall/motion in the same sketch, tap the board itself. Lower `set_tap_threshold()`. |
| Steps not counted at first | Normal: counting starts after a few steps. |
| "multiple definition of lsm6ds3tr_c_..." | Another library contains ST's driver: the Elyssa board already has it, remove the other one. |
| Compile error "needs the Elyssa board" | Select Tools → Board → Elyssa. |

---

## 8. Expert: ST driver

The board package contains ST's official driver. `elyssa_imu_st()` returns its context, for every `lsm6ds3tr_c_...()` function (they return 0 on success):

```cpp
#include <ElyssaIMU.h>
lsm6ds3tr_c_tap_detection_on_z_set(elyssa_imu_st(), 1);
```

Don't change ranges or rates with ST functions directly: use the `elyssa_imu_` functions so the conversions stay correct.

ST driver: github.com/STMicroelectronics/lsm6ds3tr-c-pid, BSD-3-Clause (see `LICENSE_ST.txt` in the board package).
