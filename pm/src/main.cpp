#include "parameters.h"
#include "logger.h"
#include "init.h"
#include "mqtt.h"

Logger logger;

MqttBase mqttBase(MQTT_BROKER_ADDRESS, MQTT_BROKER_PORT, MQTT_CLIENT_ID);
Publisher pub(MQTT_PUB_TOPIC, mqttBase);
Subscriber sub(MQTT_SUB_TOPIC, mqttBase);

void setup() {

  serial_init(SERIAL_BAUDRATE);

  led_init(true);

  delay(5000);

  mqttBase.init();

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

  if (rc == 0) {
    rc = ntp_init();

    if (rc != 0)
      logger.error("NTP failed to get time from server");
  }

  led_init(false);

  logger.info("setup end!");
}

void loop() {

  mqttBase.loop();

  pub.publish("funzionerà???");

  delay(4000);
}