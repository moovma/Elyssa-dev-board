// Elyssa v5: Wi-Fi transmit power limit (Moovma, since 1.1.3)
//
// Why: on Elyssa v5 the Wi-Fi transmitter only gives a clean signal up to about
// 13 dBm. At the ESP32 default (20 dBm) the router cannot decode the board's
// frames: connecting fails (reason 2 AUTH_EXPIRE) or the speed drops to about
// 1 Mbit/s. Measured with elyssa_wifi_bench, elyssa_wifi_final and
// elyssa_wifi_graph (9 runs, 2 routers, 7-8 Oct 2026): full speed (about
// 22 Mbit/s) up to 11 dBm on every run; speed starts to drop at 12 dBm on the
// worst router and 14 dBm on the best; under 10 % of full speed from
// 13.5-15 dBm; no connection at 19-20 dBm. Reception is not affected.
//
// 11 dBm = the lowest point where the speed drops (12 dBm) minus 1 dB, and the
// lowest collapse (13.5 dBm) minus 2.5 dB: margin for other boards, routers,
// channels and temperatures.
//
// How: every way of starting Wi-Fi (WiFi.begin, WiFi.softAP, WiFi.mode,
// ESP-NOW, ...) ends in ESP-IDF's esp_wifi_start(). platform.txt links with
// -Wl,--wrap=esp_wifi_start, so those calls arrive here: we start Wi-Fi, then
// set the maximum transmit power before any connection is made.
//
// The references to the Wi-Fi functions are weak: a sketch without Wi-Fi does
// not pull any Wi-Fi code in (Blink keeps exactly the same size).
//
// A sketch can still choose another power after WiFi.mode() with
// WiFi.setTxPower(...). It stays until Wi-Fi is stopped and started again.

#include <stdint.h>
#include "esp_err.h"

#ifndef ELYSSA_WIFI_MAX_TX_QDBM
#define ELYSSA_WIFI_MAX_TX_QDBM 44   // in 0.25 dBm: 44 = 11 dBm
#endif

esp_err_t __real_esp_wifi_start(void) __attribute__((weak));
esp_err_t esp_wifi_set_max_tx_power(int8_t power) __attribute__((weak));

esp_err_t __wrap_esp_wifi_start(void) {
  esp_err_t err = __real_esp_wifi_start();
  if (err == ESP_OK && esp_wifi_set_max_tx_power) {
    esp_wifi_set_max_tx_power(ELYSSA_WIFI_MAX_TX_QDBM);
  }
  return err;
}
