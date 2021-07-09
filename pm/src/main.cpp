#include <ESP8266WiFi.h>

#include <sstream>

#include "parameters.h"
#include "logger.h"
#include "init.h"
#include "publisher.h"

#define SERIAL_RX D5

Logger logger;

Publisher pub("192.168.8.110", 1883, "pm-001", "home/power");


void setup() {

  serial_init(SERIAL_BAUDRATE);

  led_init(true);

  delay(5000);

  pub.init();

  int wl_status;
  int rc = wifi_init(WIFI_SSID, WIFI_PSWD, wl_status, 30000);

  if (rc == 0) {
    logger.info("Wifi connected! IP: %s", WiFi.localIP().toString().c_str());

    led_blink(3, 500, true);
  }
  else {
    logger.error("Wifi not connected! Err code: %d", wl_status);

    led_blink(10, 250, true);
  }

  led_init(false);
}

void loop() {

  pub.publish("funzionerà???");

  delay(2000);
}