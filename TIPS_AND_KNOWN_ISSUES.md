# Tips and known issues

## IMU

### Using Wire1 and the IMU in the same sketch
The IMU uses I2C controller 1, the same as `Wire1`. If your sketch also uses `Wire1`, call `Wire1.begin(SDA1, SCL1, 400000)` **before** `elyssa_imu_begin()`.

### Tap can't be combined with free-fall or motion
The IMU has a single "latched events" setting. Tap only works with it off, free-fall and motion turn it on. The last `elyssa_imu_enable_...()` call wins. Use tap in one sketch, free-fall / motion in another, or switch between them.

### Tap: check often
Without the latch, a tap is visible for about 29 ms. Call `elyssa_imu_tapped()` / `elyssa_imu_double_tapped()` often: no long `delay()` in `loop()`.

### The gyroscope shows a small rotation at rest
Normal for every gyroscope (a few dps on Elyssa, it changes with temperature). Call `elyssa_imu_calibrate_gyro()` with the board still. Changing the gyroscope range clears the calibration.

### Some features change the sample rate
Tap sets 416 Hz. The step counter, tilt, wrist tilt and significant motion need 26 Hz or more.

### Strong smoothing reacts slowly
`ELYSSA_ACCEL_SMOOTH_MAX` at 104 Hz needs about 3 s to settle after being turned on.

### Tap direction is approximate
`elyssa_imu_tap_direction()` gives the axis that shook the most. A tap on an edge can also shake the board vertically (Z).

### Wrist tilt: use the edges, not Z+
With Z+ enabled in `elyssa_imu_set_wrist_tilt_axes()`, simply lying flat face up triggers the gesture. Use X+-, Y+-.

### Known issue: wrist-tilt direction
`elyssa_imu_wrist_tilted()` works, but `elyssa_imu_wrist_tilt_direction()` can return `ELYSSA_DIRECTION_NONE`, and holding the position can count a second event. Workaround: after a wrist tilt, use `elyssa_imu_orientation()` to know which edge is down. Under investigation.

### Chip and ST driver behaviours handled by the board package
Tested on Elyssa v5:
- The accelerometer user offset is subtracted on Z (added on X and Y); the library corrects the sign.
- TAP_IA is never set when events are not latched; the direction bits are read without it.
- ST driver bugs worked around: `lsm6ds3tr_c_tilt_src_set()` reads instead of writing; `lsm6ds3tr_c_motion_sens_set(0)` and `lsm6ds3tr_c_timestamp_set(0)` never write the register.

### "multiple definition of lsm6ds3tr_c_..."
Another installed library contains ST's LSM6DS3TR-C driver. The Elyssa board package already includes it: remove the other library.

## RGB LED
The green channel looks brighter than red and blue (same 330 ohm resistors, different LED efficiencies). Use `setLedRGB()` to balance mixed colours.
