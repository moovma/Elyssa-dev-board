#include "Arduino.h"
#include "esp_sleep.h"

// Welcome banner printed by hello_elyssa()
static const char ELYSSA_BANNER[] = R"ELYSSA(



                                              -+
                                           ...++...                                                     .+-.                 --..--
           . ..                            ...+#...                                                     .-.                   .##.                 ...
           .##.                               -+           ..                                                                +-..-+                .#-
           --++.                                          ###.                                                                                    ..#-..
                                                          .-.                                                                                      ...
                                                                          .
                                           .-+.                          .#-...
                                           .-+.                          .-...-+.     .-#.
                                                                         .#-..+#.   .##--.
                       ..                                                  -+-..#. -++..#.   ......                                  +##+
                     ..-+..                                 .-+.            .-+#+- #+ -+- .+#+--+-.                                   ..
                     ..-+..                                 +..#-      +--+++-. .+.++.+-.-+.. .+#.   ++
                       ..                        .--..      #+ -#    .. +#....+#..#.-.  .#+.-+#-   .#..+.
                                               .++...+#-    #. .#.  ---- .###-..#-.#-   ++#+..    -#- .#.                        -#
                                                 -#..  +#. .#+.#+...#--#.   .....++#-..-#...###-. -#. .#.                     ..+###..
                                                  .#+...#-. .#.#. -#. .#.         -#+.#+  -######.-#..#+                      ..+###..
                                                    .-+-.--+-.-.  -#. +#.         .##+-.  -####+-..-#-.  ......                  -#
                                                .....     ..###   .#.-#. .++++-.-..###. .++.#+..   -.. -++-...--++.
                             .               .-#----+#.. .. .-##.  .#- .+#  ..-+...###.  #++   ...+####-..   --+.
                            .#.             .#-..   .-+--++---.#+...#-.#-.---+- ..-#-  .++ .---##+-..   .-+++..
                           -+#+-.             .-+###+..   ..-+####-.### .-..... .+##+ .--.-###-.. ..-##..
                            .-.                          ...   ######-.   .-#.  .+##+.######.     .-####-  ..+##.+.
                                                       -####++  .-###-   ++..#-..+#####+-. .+-+.  +#####-.+--..+-.
                                                       -#####+    .-### .#...#..####+...   ++..+. .+###..#-..+#-
                                                       .-###.-#.   .###..# +#- .####- .#. .#. .#.  .#. .-#+++. .---.
    -..                                       ..-#####..   .+++-.  .+##...#-  .+###  .+.#-.#..+#. .+..-#-....+#-...-#-..
   .+.                                      .-++.-   ..-#+---- .+.   +##-.-   +###+ .#..-#..#-+.  ++-##--++++++-...--+-.                      ..  ...
.#######+                                    .+########- .. .-####-  ..###  .####-  .#..#+. #+  .###...        -###-.                         .+-.#.
   .+..                                          ....        ..######-.###  .####-  .-+-#. .#+.#####-..######.                                 .##-
    -.                                             ..####.+#####.  .#######-.###.     -+ ..####-    .+#....  -#-.                             --...+.
                                                  .+-   .-+. .+.      .+########.    .##-#####----.   ..+#+++#+-.
                                                 .#-..-+#-  +#+.        .-######.   -+#####---#+--+-+++..
                   .+.                           .-+++-.  -#####+.       .######. .+#####..   .+. -#. .+#+.
                  .+#-       ..                           -#####+.         +####..-###+     .-###+..#-.  ++.                                              .+-.
                           .###.                            ###.           +####-+####.     ######- .##...#-.                       .+                    .+-.
                             ..                                             .#######-       -+###-.   .-----.                      .-#-.
                                                                            .######          .....                                  ..
                                                                              +####
                                         .-####..                             +####
             .#.                        -##-..-#+.                            +####
           .#####                      -#-.###+.##.                           +####                                 .-------.
             .#.                       -#+.+-.#.++.                           +####                                  .+####..
                                       .-######.+#.                           +####                                   -#+-
                                         ...+#-.-##.                          +#####-                                .+#-
                                            +#.#--##.                       .#######-.                              .-##+.
                                            +#.##-.##+..                   .-########+.                          ..-##+.#-
                                            +#.##+-.++###++++.           .+############++.               .-+++#######+..#+                ..+..
                                           .#..+####-...----###############################################+----....-##.#+                ..+..
                                           ##.##############-..............................................+#######--+#.#-
                                          .#+.+#######..######################################################..###-.-#.+#+                                ..
                                           .+-.---++++--###-.+##+ -###..#### .#### .###+ -###-.+###..###- +###--#######+ +#-.                            ..++..
                                           .## .-+##++----++###################################################----...-++.-#+.                           ..++..
                                            -## -+...-######-..................................... ..........++########-....##-                            ..
                                             -##. -###......+#################################################+-......-####  .##.
                                              .###-. .--####+---.....................     ............. .. ..+#####+---..--+###+.
                                    .+#+.       .+#--.     ....++######++.--++++++----###+--++++++++---+#####-..  --+++---.... .--.
                                     .+##-    -###+++###+   -##+--.  .--###+.   .+++##---+##+-    .++###-...++###. . -+###+...+##+.
                                       .-######--.  .--.#####...        ..++######++.     .-+#######++.      .-+######+...-####-..
                                            ...             .                                                  . .....
                                            ...   ...####...     ...+###-.          ..+++-.           ...####+...      ....
                                            +##-++###-..-###-....###+...##---.   ---##---+#++-    .---###-...+###.....###..                                .+-..-+
  .-.+                                        .+++-        .#####+       .+-+#####-+.     .-+#######++-        .+#####+ .                                   .-##.
  .++-                                                                      ......           .......                                                       .-+.-+-
                                                                                                                                                           ..   ..




                .                    -##-+##+. .-###..    ..######.     .+#####..   +##     +##. +##.+##-. .+##+.   .########...
               .#.                   -###+-+##+##--##++ .-+#-----+#+. .+#+-----##+. +##     +##. +###+-+##+#+-+##+- .-------+##-
             -#####.                 -##-   .##+   .### .###     .##- -##-    .###. +##.    +##. +##.   -##-.  .##-  ........##-
               .#-                   -##-   .##+   .### .###     .##- -##-    .###.  +##.  ###   +##.   -##-   .##- .##########+
                .                    -##-   .##+   .### .###     .##- .##-    .###.  .-##-##+.   +##.   -##-   .##- .##+.....##-
                                     -##-   .##+   .###   -########.   .########+     -#####+    +##.   -##-   .##- .##+    .##-             .-+
                                     .--.   .--.   .---    .------       .-----.       .---.     ---.   .--.   .--. .--.    .--.             .-+.





                                 ╔══════════════════════════════════════════════════════════════════════════════════════════════╗
                                 ║                                       --- ELYSSA  ---                                        ║
                                 ╠══════════════════════════════════════════════════════════════════════════════════════════════╣
                                 ║                                           Ahla!                                              ║
                                 ║                                                                                              ║
                                 ║      Thabet mli7 felli bin idik. Hedhi mch juste pcb, hadhi awel dev board tounsia,          ║
                                 ║                         hadhi awel 5atwa sad9a f thneya jdida.                               ║
                                 ║                                                                                              ║
                                 ║           Kima Elyssa el malika eli harbet mel dholm bech tiktib teri5 Carthage              ║
                                 ║                bmo5ha w dhkeha w 3azimetha, a7na lyoum fi Moovma harbin                      ║
                                 ║              mel dholm mta3 ettaba3iya ettechnologiya eli n3ichou feha lyoum.                ║
                                 ║               w ma rdhinech bech no93dou te7et ra7met ghirna, nasta3mlou felli               ║
                                 ║                         yetefradh 3lina w nconsomiouw feli yeta3talna,                       ║
                                 ║                                                                                              ║
                                 ║          Moovma mch juste startup, hiya el 'movement' eli bech t5ali el9ayd yankasser,       ║
                                 ║                 Moovma Hiya el mo5 etounsi ki y9arrer yfok blastou fel 3alam.                ║
                                 ║                                                                                              ║
                                 ║           Kima el Zitouna etounseya m3ar9a fi trabna, thabta w 9weya w ma y9ala3ha           ║
                                 ║           7atta ri7, l'innovation mechi 7eker 3la cha3eb men cha3eb matansech eli            ║
                                 ║                        rak tounsi tari5ek kbir w majdek sebe9.                               ║
                                 ║                                                                                              ║
                                 ║            Moovma jetbech t9oul elli el hardware mte3na nasn3ouh bidina, bech nkounou        ║
                                 ║             a7rar w na7na barka eli nemlkou mfeta7 mosta9belena, wel 7a9 yetfak              ║
                                 ║                                       mayeta3tach.                                           ║
                                 ║                                                                                              ║
                                 ║          w Heka 3leh 2027 mch bech tkoun kima 2027. Bech nbadlou mab3adhna el mosta9bel.     ║
                                 ║           2027 bech ykoun el 3am elli Tounis twalli hiya elli t5alla9 el technologie,        ║
                                 ║          mch elli techriha. Thabet mli7, 5ater fel dev board hadhi, maktoub el mosta9bel     ║
                                 ║                                  elli bech nasn3ouh mab3adhna.                               ║
                                 ║                                                                                              ║
                                 ║            Tafaker dima: 'Tijri el rya7 kima tijri sfinetna'.. 5ater a7na el ry7,            ║
                                 ║            a7na el b7ar, w a7na sfinet Carthage el jdida elli mcheya lel mosta9bel.          ║
                                 ║                                                                                              ║
                                 ║            El khedma badet.. w el legacy mte3ek yabda lyoum, el moovma bdet bech             ║
                                 ║                      nraj3ou glory carthage: na3mlou Moovma sa7bi                            ║
                                 ╠══════════════════════════════════════════════════════════════════════════════════════════════╣
                                 ║                                   Copyright © 2026 Moovma                                    ║
                                 ║                                      All Rights Reserved                                     ║
                                 ╚══════════════════════════════════════════════════════════════════════════════════════════════╝
)ELYSSA";

bool hello_elyssa(uint32_t wait_ms) {
  static bool serial_started = false;
  if (!serial_started) {
    Serial.begin(115200);
    serial_started = true;
  }

  unsigned long start = millis();
  while (!Serial && (millis() - start < wait_ms)) {
    delay(10);
  }
  if (!Serial) {
    return false;          // no monitor: skip the banner
  }
  delay(500);

  Serial.println(ELYSSA_BANNER);
  return true;
}

//
// IMU: LSM6DS3TR-C on its dedicated I2C bus.
// Uses STMicroelectronics' official driver (lsm6ds3tr-c_reg.c/.h in this
// folder, BSD-3-Clause, see LICENSE_ST.txt). Each feature follows ST's
// official example for it (STMems_Standard_C_drivers).
// Core functions for most applications. Advanced features: ElyssaIMU library.
//
#include <string.h>
#include "lsm6ds3tr-c_reg.h"

#define ELYSSA_IMU_I2C 1             // I2C controller 1 = same controller as Wire1
#define I2C_TIMEOUT_MS 10
#define IMU_MAX_READ   32            // largest read the core does (14 bytes)

// Event flags remembered between checks (each check returns true once).
// Shared with the ElyssaIMU library (elyssa_imu_internal.h).
#include "elyssa_imu_internal.h"
#define EV_TAP     ELYSSA_EV_TAP
#define EV_DTAP    ELYSSA_EV_DOUBLE_TAP
#define EV_FALL    ELYSSA_EV_FALL
#define EV_MOTION  ELYSSA_EV_MOTION

static bool     gyro_initialized = false;
static uint8_t  accel_range = 2;           // g
static uint16_t gyro_range  = 250;         // dps
static lsm6ds3tr_c_odr_xl_t odr = LSM6DS3TR_C_XL_ODR_104Hz;
static float    gyro_offset[3] = { 0, 0, 0 };
static uint16_t imu_events = 0;
static ElyssaImuEventInfo imu_info = { 0, 0, 0, 0, false };
static float_t (*accel_to_mg)(int16_t)   = lsm6ds3tr_c_from_fs2g_to_mg;
static float_t (*gyro_to_mdps)(int16_t)  = lsm6ds3tr_c_from_fs250dps_to_mdps;

static const float ODR_HZ[] = { 0, 12.5f, 26, 52, 104, 208, 416, 833, 1660, 3330, 6660 };

// ---------- Platform layer for the ST driver (ESP32 I2C HAL, controller 1) ----------
static int32_t platform_write(void *handle, uint8_t reg, const uint8_t *bufp, uint16_t len) {
  (void)handle;
  uint8_t buf[IMU_MAX_READ + 1];
  if (len > IMU_MAX_READ) return -1;
  buf[0] = reg;
  memcpy(&buf[1], bufp, len);
  return i2cWrite(ELYSSA_IMU_I2C, GYRO_ADDR, buf, len + 1, I2C_TIMEOUT_MS) == ESP_OK ? 0 : -1;
}

static int32_t platform_read(void *handle, uint8_t reg, uint8_t *bufp, uint16_t len) {
  (void)handle;
  size_t count = 0;
  return (i2cWriteReadNonStop(ELYSSA_IMU_I2C, GYRO_ADDR, &reg, 1, bufp, len,
                              I2C_TIMEOUT_MS, &count) == ESP_OK && count == len) ? 0 : -1;
}

static void platform_delay(uint32_t ms) { delay(ms); }

static stmdev_ctx_t dev_ctx = { platform_write, platform_read, platform_delay, NULL, NULL };

#define ST_OK(x) ((x) == 0)

// ---------- Events: read all sources once (as ST's examples do), remember them ----------
// Reading clears latched events, so EVERY event is recorded here, also the
// ones used only by the ElyssaIMU library.
static void imu_poll_events() {
  if (!gyro_initialized) return;
  // The wrist-tilt direction is cleared when FUNC_SRC2 is read, and ST's
  // all_sources_get() reads FUNC_SRC2 first: read the direction BEFORE it
  // (tested on Elyssa v5).
  uint8_t wrist_dir = 0;
  lsm6ds3tr_c_read_reg(&dev_ctx, LSM6DS3TR_C_WRIST_TILT_IA, &wrist_dir, 1);
  lsm6ds3tr_c_all_sources_t src;
  if (!ST_OK(lsm6ds3tr_c_all_sources_get(&dev_ctx, &src))) return;
  uint8_t b;
  if (src.wake_up_src.ff_ia)  imu_events |= ELYSSA_EV_FALL;
  if (src.wake_up_src.wu_ia) {
    imu_events |= ELYSSA_EV_MOTION;
    memcpy(&b, &src.wake_up_src, 1); imu_info.wake_up_src = b;
  }
  imu_info.still = src.wake_up_src.sleep_state_ia;
  if (src.tap_src.single_tap) imu_events |= ELYSSA_EV_TAP;
  if (src.tap_src.double_tap) imu_events |= ELYSSA_EV_DOUBLE_TAP;
  // Tap direction: TAP_IA is never set when events are not latched (tested on
  // Elyssa v5), so save the direction bits with every single or double tap
  if (src.tap_src.single_tap || src.tap_src.double_tap) {
    memcpy(&b, &src.tap_src, 1);
    if (b & 0x07) imu_info.tap_src = b;
  }
  if (src.d6d_src.d6d_ia) {
    imu_events |= ELYSSA_EV_ORIENTATION;
    memcpy(&b, &src.d6d_src, 1); imu_info.d6d_src = b;
  }
  if (src.func_src1.step_detected)       imu_events |= ELYSSA_EV_STEP;
  if (src.func_src1.tilt_ia)             imu_events |= ELYSSA_EV_TILT;
  if (src.func_src1.sign_motion_ia)      imu_events |= ELYSSA_EV_SIGNIFICANT_MOTION;
  if (src.func_src1.step_count_delta_ia) imu_events |= ELYSSA_EV_STEP_REPORT;
  // Wrist tilt (tested on Elyssa v5): the event flag stays set while the
  // position is held, but the direction register is cleared after its first
  // read. So: one event per gesture (flag going 0 -> 1), and keep the last
  // non-zero direction.
  static bool wrist_prev = false;
  bool wrist_now = src.func_src2.wrist_tilt_ia;
  if (wrist_now && !wrist_prev) imu_events |= ELYSSA_EV_WRIST_TILT;
  wrist_prev = wrist_now;
  if (wrist_dir) imu_info.wrist_tilt_ia = wrist_dir;
}

static bool imu_take_event(uint16_t ev) {
  imu_poll_events();
  bool hit = imu_events & ev;
  imu_events &= ~ev;
  return hit;
}

// ---------- Connection point for the ElyssaIMU library (elyssa_imu_internal.h) ----------
stmdev_ctx_t *elyssa_imu_st_ctx() { return &dev_ctx; }
bool elyssa_imu_started() { return gyro_initialized; }
bool elyssa_imu_take_event(uint16_t ev) { return imu_take_event(ev); }
const ElyssaImuEventInfo *elyssa_imu_event_info() { imu_poll_events(); return &imu_info; }
float *elyssa_imu_gyro_offset_data() { return gyro_offset; }
void elyssa_imu_note_accel_rate(uint8_t code) { odr = (lsm6ds3tr_c_odr_xl_t)code; }

static bool imu_route_int1(void (*edit)(lsm6ds3tr_c_int1_route_t &)) {
  lsm6ds3tr_c_int1_route_t int_1_reg;
  if (!ST_OK(lsm6ds3tr_c_pin_int1_route_get(&dev_ctx, &int_1_reg))) return false;
  edit(int_1_reg);
  return ST_OK(lsm6ds3tr_c_pin_int1_route_set(&dev_ctx, int_1_reg));
}

// ---------- Start ----------
uint8_t elyssa_imu_read_reg(uint8_t reg) {
  uint8_t v = 0xFF;
  lsm6ds3tr_c_read_reg(&dev_ctx, reg, &v, 1);
  return v;
}

// Starts the IMU on its dedicated I2C bus (controller 1 = Wire1, pins SDA1/SCL1).
// If the sketch also uses Wire1, call Wire1.begin(SDA1, SCL1, 400000) BEFORE
// elyssa_imu_begin() (see TIPS_AND_KNOWN_ISSUES.md).
// Defaults: +-2 g, +-250 dps, 104 Hz, high performance, no events.
bool elyssa_imu_begin() {
  gyro_initialized = false;
  if (!i2cIsInit(ELYSSA_IMU_I2C) &&
      i2cInit(ELYSSA_IMU_I2C, SDA_gyro, SCL_gyro, 400000) != ESP_OK) return false;
  pinMode(INT_gyro, INPUT);

  // ST start sequence: check the ID, software reset, wait for the end of reset
  uint8_t whoamI = 0, rst = 1;
  if (!ST_OK(lsm6ds3tr_c_device_id_get(&dev_ctx, &whoamI)) || whoamI != LSM6DS3TR_C_ID) return false;
  if (!ST_OK(lsm6ds3tr_c_reset_set(&dev_ctx, PROPERTY_ENABLE))) return false;
  uint32_t t = millis();
  do {
    if (millis() - t > 50) return false;
    lsm6ds3tr_c_reset_get(&dev_ctx, &rst);
  } while (rst);

  // ST read_data_polling example: BDU, then rates and full scales
  if (!ST_OK(lsm6ds3tr_c_block_data_update_set(&dev_ctx, PROPERTY_ENABLE)) ||
      !ST_OK(lsm6ds3tr_c_auto_increment_set(&dev_ctx, PROPERTY_ENABLE)) ||
      !ST_OK(lsm6ds3tr_c_xl_data_rate_set(&dev_ctx, LSM6DS3TR_C_XL_ODR_104Hz)) ||
      !ST_OK(lsm6ds3tr_c_gy_data_rate_set(&dev_ctx, LSM6DS3TR_C_GY_ODR_104Hz)) ||
      !ST_OK(lsm6ds3tr_c_xl_full_scale_set(&dev_ctx, LSM6DS3TR_C_2g)) ||
      !ST_OK(lsm6ds3tr_c_gy_full_scale_set(&dev_ctx, LSM6DS3TR_C_250dps)))
    return false;

  accel_range = 2;   accel_to_mg  = lsm6ds3tr_c_from_fs2g_to_mg;
  gyro_range  = 250; gyro_to_mdps = lsm6ds3tr_c_from_fs250dps_to_mdps;
  odr = LSM6DS3TR_C_XL_ODR_104Hz;
  gyro_offset[0] = gyro_offset[1] = gyro_offset[2] = 0;
  imu_events = 0;

  // Discard the first 10 samples (about 100 ms at 104 Hz): right after
  // power-on the gyro is still settling (Elyssa v5: with only 3 samples
  // skipped, X/Y gyro values were unsettled; clean after 100 ms)
  uint8_t skip = 10;
  t = millis();
  while (skip) {
    if (millis() - t > 500) return false;
    lsm6ds3tr_c_status_reg_t st;
    if (ST_OK(lsm6ds3tr_c_status_reg_get(&dev_ctx, &st)) && st.xlda && st.gda) {
      int16_t raw[3];
      lsm6ds3tr_c_acceleration_raw_get(&dev_ctx, raw);   // reading clears the data-ready flags
      lsm6ds3tr_c_angular_rate_raw_get(&dev_ctx, raw);
      skip--;
    } else {
      delay(1);
    }
  }
  gyro_initialized = true;
  return true;
}

// true when a new acceleration AND rotation sample is ready
bool elyssa_imu_ready() {
  lsm6ds3tr_c_status_reg_t st;
  return gyro_initialized && ST_OK(lsm6ds3tr_c_status_reg_get(&dev_ctx, &st)) && st.xlda && st.gda;
}

uint8_t elyssa_imu_whoami() {
  uint8_t id = 0xFF;
  lsm6ds3tr_c_device_id_get(&dev_ctx, &id);
  return id;
}

// ---------- Reading ----------
static float accel_axis(uint8_t i) {
  int16_t raw[3];
  if (!gyro_initialized || !ST_OK(lsm6ds3tr_c_acceleration_raw_get(&dev_ctx, raw))) return NAN;
  return accel_to_mg(raw[i]) / 1000.0f;
}

static float gyro_axis(uint8_t i) {
  int16_t raw[3];
  if (!gyro_initialized || !ST_OK(lsm6ds3tr_c_angular_rate_raw_get(&dev_ctx, raw))) return NAN;
  return gyro_to_mdps(raw[i]) / 1000.0f - gyro_offset[i];
}

float gyro_return_ax()  { return gyro_axis(0); }
float gyro_return_ay()  { return gyro_axis(1); }
float gyro_return_az()  { return gyro_axis(2); }
float accel_return_ax() { return accel_axis(0); }
float accel_return_ay() { return accel_axis(1); }
float accel_return_az() { return accel_axis(2); }

// Clear names (same values as the old *_return_* functions)
float elyssa_imu_accel_x() { return accel_axis(0); }
float elyssa_imu_accel_y() { return accel_axis(1); }
float elyssa_imu_accel_z() { return accel_axis(2); }
float elyssa_imu_gyro_x()  { return gyro_axis(0); }
float elyssa_imu_gyro_y()  { return gyro_axis(1); }
float elyssa_imu_gyro_z()  { return gyro_axis(2); }

float elyssa_imu_temperature() {
  int16_t raw;
  if (!gyro_initialized || !ST_OK(lsm6ds3tr_c_temperature_raw_get(&dev_ctx, &raw))) return NAN;
  return lsm6ds3tr_c_from_lsb_to_celsius(raw);
}

// Reads temperature, gyro and accel in ONE transaction: all values from the same sample.
// Any pointer can be NULL. Returns false if the IMU is not started or on I2C error.
bool elyssa_imu_read(float gyro[3], float accel[3], float *temp_c) {
  uint8_t b[14];   // OUT_TEMP_L (20h) .. OUTZ_H_XL (2Dh)
  if (!gyro_initialized || !ST_OK(lsm6ds3tr_c_read_reg(&dev_ctx, LSM6DS3TR_C_OUT_TEMP_L, b, sizeof(b))))
    return false;
  int16_t raw[7];
  for (int i = 0; i < 7; i++) raw[i] = (int16_t)(b[2 * i] | (b[2 * i + 1] << 8));
  if (temp_c) *temp_c = lsm6ds3tr_c_from_lsb_to_celsius(raw[0]);
  for (int i = 0; i < 3; i++) {
    if (gyro)  gyro[i]  = gyro_to_mdps(raw[1 + i]) / 1000.0f - gyro_offset[i];
    if (accel) accel[i] = accel_to_mg(raw[4 + i]) / 1000.0f;
  }
  return true;
}

// Acceleration of the 3 axes from the same sample, in g
bool elyssa_imu_read_accel(float &x, float &y, float &z) {
  float a[3];
  if (!elyssa_imu_read(NULL, a, NULL)) return false;
  x = a[0]; y = a[1]; z = a[2];
  return true;
}

// Rotation of the 3 axes from the same sample, in dps (calibration offset removed)
bool elyssa_imu_read_gyro(float &x, float &y, float &z) {
  float g[3];
  if (!elyssa_imu_read(g, NULL, NULL)) return false;
  x = g[0]; y = g[1]; z = g[2];
  return true;
}

// After a range change the IMU still holds samples measured with the old
// range: skip the current one and wait for 'n' new ones (by the data-ready flag)
static bool imu_wait_fresh(bool accel, uint8_t n) {
  int16_t raw[3];
  if (accel) lsm6ds3tr_c_acceleration_raw_get(&dev_ctx, raw);   // clears the flag
  else       lsm6ds3tr_c_angular_rate_raw_get(&dev_ctx, raw);
  uint32_t t = millis();
  while (n) {
    if (millis() - t > 500) return false;
    lsm6ds3tr_c_status_reg_t st;
    if (ST_OK(lsm6ds3tr_c_status_reg_get(&dev_ctx, &st)) && (accel ? st.xlda : st.gda)) {
      if (accel) lsm6ds3tr_c_acceleration_raw_get(&dev_ctx, raw);
      else       lsm6ds3tr_c_angular_rate_raw_get(&dev_ctx, raw);
      n--;
    } else {
      delay(1);
    }
  }
  return true;
}

// ---------- Settings ----------

// Acceleration range: 2, 4, 8 or 16 (g). Returns after 2 new samples (~20 ms at 104 Hz).
bool elyssa_imu_set_accel_range(uint8_t g) {
  lsm6ds3tr_c_fs_xl_t fs;
  float_t (*conv)(int16_t);
  switch (g) {
    case 2:  fs = LSM6DS3TR_C_2g;  conv = lsm6ds3tr_c_from_fs2g_to_mg;  break;
    case 4:  fs = LSM6DS3TR_C_4g;  conv = lsm6ds3tr_c_from_fs4g_to_mg;  break;
    case 8:  fs = LSM6DS3TR_C_8g;  conv = lsm6ds3tr_c_from_fs8g_to_mg;  break;
    case 16: fs = LSM6DS3TR_C_16g; conv = lsm6ds3tr_c_from_fs16g_to_mg; break;
    default: return false;
  }
  if (!gyro_initialized || !ST_OK(lsm6ds3tr_c_xl_full_scale_set(&dev_ctx, fs))) return false;
  accel_to_mg = conv;
  accel_range = g;
  return imu_wait_fresh(true, 2);
}

uint8_t elyssa_imu_get_accel_range() { return accel_range; }

// Rotation range: 125, 250, 500, 1000 or 2000 (dps). Returns after 2 new samples.
bool elyssa_imu_set_gyro_range(uint16_t dps) {
  lsm6ds3tr_c_fs_g_t fs;
  float_t (*conv)(int16_t);
  switch (dps) {
    case 125:  fs = LSM6DS3TR_C_125dps;  conv = lsm6ds3tr_c_from_fs125dps_to_mdps;  break;
    case 250:  fs = LSM6DS3TR_C_250dps;  conv = lsm6ds3tr_c_from_fs250dps_to_mdps;  break;
    case 500:  fs = LSM6DS3TR_C_500dps;  conv = lsm6ds3tr_c_from_fs500dps_to_mdps;  break;
    case 1000: fs = LSM6DS3TR_C_1000dps; conv = lsm6ds3tr_c_from_fs1000dps_to_mdps; break;
    case 2000: fs = LSM6DS3TR_C_2000dps; conv = lsm6ds3tr_c_from_fs2000dps_to_mdps; break;
    default: return false;
  }
  if (!gyro_initialized || !ST_OK(lsm6ds3tr_c_gy_full_scale_set(&dev_ctx, fs))) return false;
  gyro_offset[0] = gyro_offset[1] = gyro_offset[2] = 0;   // offset was measured at the old scale
  gyro_to_mdps = conv;
  gyro_range = dps;
  return imu_wait_fresh(false, 2);
}

uint16_t elyssa_imu_get_gyro_range() { return gyro_range; }

// Sample rate for both sensors, in Hz: 12.5, 26, 52, 104, 208, 416, 833, 1660, 3330, 6660.
// Other values: the next higher rate is used.
bool elyssa_imu_set_rate(float hz) {
  if (!gyro_initialized || hz <= 0 || hz > 6660) return false;
  uint8_t code = 1;
  while (code < 10 && ODR_HZ[code] < hz) code++;
  if (!ST_OK(lsm6ds3tr_c_xl_data_rate_set(&dev_ctx, (lsm6ds3tr_c_odr_xl_t)code)) ||
      !ST_OK(lsm6ds3tr_c_gy_data_rate_set(&dev_ctx, (lsm6ds3tr_c_odr_g_t)code)))
    return false;
  odr = (lsm6ds3tr_c_odr_xl_t)code;
  return true;
}

float elyssa_imu_get_rate() { return ODR_HZ[odr]; }

// Power saving: less current, a bit more noise (both sensors)
bool elyssa_imu_low_power(bool on) {
  return gyro_initialized &&
         ST_OK(lsm6ds3tr_c_xl_power_mode_set(&dev_ctx, on ? LSM6DS3TR_C_XL_NORMAL
                                                          : LSM6DS3TR_C_XL_HIGH_PERFORMANCE)) &&
         ST_OK(lsm6ds3tr_c_gy_power_mode_set(&dev_ctx, on ? LSM6DS3TR_C_GY_NORMAL
                                                          : LSM6DS3TR_C_GY_HIGH_PERFORMANCE));
}

// ---------- Angles and orientation (from gravity: the board should be fairly still) ----------
float elyssa_imu_pitch() {
  float a[3];
  if (!elyssa_imu_read(NULL, a, NULL)) return NAN;
  return atan2f(-a[0], sqrtf(a[1] * a[1] + a[2] * a[2])) * 57.29578f;
}

float elyssa_imu_roll() {
  float a[3];
  if (!elyssa_imu_read(NULL, a, NULL)) return NAN;
  return atan2f(a[1], a[2]) * 57.29578f;
}

ElyssaOrientation elyssa_imu_orientation() {
  float a[3];
  if (!elyssa_imu_read(NULL, a, NULL)) return ELYSSA_ORIENTATION_UNKNOWN;
  int ax = 0;
  for (int i = 1; i < 3; i++) if (fabsf(a[i]) > fabsf(a[ax])) ax = i;
  if (fabsf(a[ax]) < 0.7f) return ELYSSA_ORIENTATION_UNKNOWN;   // between two sides
  bool up = a[ax] > 0;
  if (ax == 2) return up ? ELYSSA_FACE_UP : ELYSSA_FACE_DOWN;
  if (ax == 0) return up ? ELYSSA_X_UP : ELYSSA_X_DOWN;
  return up ? ELYSSA_Y_UP : ELYSSA_Y_DOWN;
}

// ---------- Gyroscope calibration: keep the board still during 'ms' ----------
bool elyssa_imu_calibrate_gyro(uint16_t ms) {
  if (!gyro_initialized) return false;
  float saved[3] = { gyro_offset[0], gyro_offset[1], gyro_offset[2] };
  gyro_offset[0] = gyro_offset[1] = gyro_offset[2] = 0;
  double sum[3] = { 0, 0, 0 };
  uint32_t n = 0, t = millis();
  while (millis() - t < ms) {
    float g[3];
    if (elyssa_imu_ready() && elyssa_imu_read(g, NULL, NULL)) {
      sum[0] += g[0]; sum[1] += g[1]; sum[2] += g[2];
      n++;
    }
  }
  if (n < 10) {
    for (int i = 0; i < 3; i++) gyro_offset[i] = saved[i];
    return false;
  }
  for (int i = 0; i < 3; i++) gyro_offset[i] = sum[i] / n;
  return true;
}

// ---------- Events: each check returns true ONCE per event ----------

// Tap and double tap: ST example lsm6dsm_tap_double.c (sibling chip, same registers).
// Needs 416 Hz: the rate is raised if it is lower.
// Tap works only with events NOT latched (as in ST's example; tested on
// Elyssa v5: no tap detected in latched mode). The latch is one setting for
// the whole chip: free-fall and motion turn it on, tap turns it off, so the
// last enable_ call wins. Tap events are short (~29 ms): call the checks often.
bool elyssa_imu_enable_tap() {
  if (!gyro_initialized) return false;
  if (odr < LSM6DS3TR_C_XL_ODR_416Hz && !elyssa_imu_set_rate(416)) return false;
  return ST_OK(lsm6ds3tr_c_int_notification_set(&dev_ctx, LSM6DS3TR_C_INT_PULSED)) &&
         ST_OK(lsm6ds3tr_c_tap_detection_on_z_set(&dev_ctx, PROPERTY_ENABLE)) &&
         ST_OK(lsm6ds3tr_c_tap_detection_on_y_set(&dev_ctx, PROPERTY_ENABLE)) &&
         ST_OK(lsm6ds3tr_c_tap_detection_on_x_set(&dev_ctx, PROPERTY_ENABLE)) &&
         ST_OK(lsm6ds3tr_c_4d_mode_set(&dev_ctx, PROPERTY_ENABLE)) &&
         ST_OK(lsm6ds3tr_c_tap_threshold_x_set(&dev_ctx, 0x0c)) &&   // 750 mg at +-2 g
         ST_OK(lsm6ds3tr_c_tap_dur_set(&dev_ctx, 0x07)) &&           // 538.5 ms
         ST_OK(lsm6ds3tr_c_tap_quiet_set(&dev_ctx, 0x03)) &&         // 28.8 ms
         ST_OK(lsm6ds3tr_c_tap_shock_set(&dev_ctx, 0x03)) &&         // 57.7 ms
         ST_OK(lsm6ds3tr_c_tap_mode_set(&dev_ctx, LSM6DS3TR_C_BOTH_SINGLE_DOUBLE)) &&
         imu_route_int1([](lsm6ds3tr_c_int1_route_t &r) { r.int1_double_tap = PROPERTY_ENABLE; });
}

bool elyssa_imu_tapped()        { return imu_take_event(EV_TAP); }
bool elyssa_imu_double_tapped() { return imu_take_event(EV_DTAP); }

// Free-fall: ST example lsm6dsm_free_fall.c (latched, 6 samples, 312 mg).
// Turns the latch on: don't combine with tap in the same sketch.
bool elyssa_imu_enable_freefall() {
  return gyro_initialized &&
         ST_OK(lsm6ds3tr_c_int_notification_set(&dev_ctx, LSM6DS3TR_C_INT_LATCHED)) &&
         ST_OK(lsm6ds3tr_c_ff_dur_set(&dev_ctx, 0x06)) &&
         ST_OK(lsm6ds3tr_c_ff_threshold_set(&dev_ctx, LSM6DS3TR_C_FF_TSH_312mg)) &&
         imu_route_int1([](lsm6ds3tr_c_int1_route_t &r) { r.int1_ff = PROPERTY_ENABLE; });
}

bool elyssa_imu_fell() { return imu_take_event(EV_FALL); }

// Motion: ST example lsm6dsm_wake_up.c (high-pass filter, duration 0).
// Latched so that a movement is never missed (and for wake-up from sleep).
// Turns the latch on: don't combine with tap in the same sketch.
// Sensitivity 1 (most sensitive) .. 63, 1 step = range / 64 (ST example: 2)
bool elyssa_imu_enable_motion(uint8_t sensitivity) {
  if (!gyro_initialized || sensitivity < 1 || sensitivity > 63) return false;
  return ST_OK(lsm6ds3tr_c_int_notification_set(&dev_ctx, LSM6DS3TR_C_INT_LATCHED)) &&
         ST_OK(lsm6ds3tr_c_xl_hp_path_internal_set(&dev_ctx, LSM6DS3TR_C_USE_HPF)) &&
         ST_OK(lsm6ds3tr_c_wkup_dur_set(&dev_ctx, 0)) &&
         ST_OK(lsm6ds3tr_c_wkup_threshold_set(&dev_ctx, sensitivity)) &&
         imu_route_int1([](lsm6ds3tr_c_int1_route_t &r) { r.int1_wu = PROPERTY_ENABLE; });
}

bool elyssa_imu_moved() { return imu_take_event(EV_MOTION); }

// ---------- Step counter (needs 26 Hz or more: the rate is raised if lower) ----------
bool elyssa_imu_start_steps() {
  if (!gyro_initialized) return false;
  if (odr < LSM6DS3TR_C_XL_ODR_26Hz && !elyssa_imu_set_rate(26)) return false;
  return ST_OK(lsm6ds3tr_c_pedo_sens_set(&dev_ctx, PROPERTY_ENABLE));
}

uint16_t elyssa_imu_steps() {
  uint8_t b[2];
  if (!gyro_initialized || !ST_OK(lsm6ds3tr_c_read_reg(&dev_ctx, LSM6DS3TR_C_STEP_COUNTER_L, b, 2))) return 0;
  return b[0] | (b[1] << 8);
}

// The IMU applies the reset at its next sample (38 ms at 26 Hz): keep the
// reset bit set until the counter reads 0, then release it.
bool elyssa_imu_reset_steps() {
  if (!gyro_initialized || !ST_OK(lsm6ds3tr_c_pedo_step_reset_set(&dev_ctx, PROPERTY_ENABLE))) return false;
  uint32_t t = millis();
  while (elyssa_imu_steps() != 0 && millis() - t < 200) delay(5);
  bool ok = elyssa_imu_steps() == 0;
  return ST_OK(lsm6ds3tr_c_pedo_step_reset_set(&dev_ctx, PROPERTY_DISABLE)) && ok;
}

// ---------- Wake on motion: call just before esp_deep_sleep_start() ----------
// The IMU keeps measuring while the ESP32 sleeps; moving the board wakes it up.
bool elyssa_imu_wake_on_motion(uint8_t sensitivity) {
  if (!elyssa_imu_enable_motion(sensitivity)) return false;
  imu_poll_events();                    // clear a pending event: INT1 must be low
  imu_events = 0;
  return esp_sleep_enable_ext0_wakeup((gpio_num_t)INT_gyro, 1) == ESP_OK;
}

//
// RGB LED (Elyssa v5): red = LED_RED (39), green = LED_GREEN (38), blue = LED_BLUE (33)
// All channels active HIGH.
//

// Named colours: each bit of 'color' switches one channel on/off.
// pinMode() on every call releases the PWM if setLedRGB() was used before,
// so users can switch freely between setLedColor(), setLedRGB() and digitalWrite().
void setLedColor(ElyssaColor color) {
  const uint8_t pins[3] = { LED_RED, LED_GREEN, LED_BLUE };
  for (uint8_t i = 0; i < 3; i++) {
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], ((color >> i) & 1) ? HIGH : LOW);
  }
}

// Any colour mix with PWM: 0 = off, 255 = full brightness
void setLedRGB(uint8_t r, uint8_t g, uint8_t b) {
  analogWrite(LED_RED, r);
  analogWrite(LED_GREEN, g);
  analogWrite(LED_BLUE, b);
}

//
// Startup: called by the ESP32 core before setup() (initArduino -> initVariant)
//
// The red LED is on GPIO39 = MTCK (JTAG clock). At reset the ESP32-S3 enables
// the internal weak pull-up of this pin (datasheet, pin 44, note 7), which makes
// the red LED glow when the sketch never uses the LED. Driving the three LED
// pins LOW here turns the LED fully off as soon as any sketch starts.
// (Before this point - upload, ROM boot - only hardware can change it.)
//
extern "C" void initVariant(void) {
  const uint8_t pins[3] = { LED_RED, LED_GREEN, LED_BLUE };
  for (uint8_t i = 0; i < 3; i++) {
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], LOW);
  }
}
