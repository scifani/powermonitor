#include "parameters.h"
#include "logger.h"
#include "init.h"
#include "mqtt.h"
#include "api_server.h"
#include "ws.h"
#include "pzem-004t.h"

DateTimeClass dt;
time_t last_read = 0;

SerialWriter serial_writer(&DEBUG_SERIAL);
WsWriter ws_writer;
Logger logger;

MqttBase mqttBase(MQTT_BROKER_ADDRESS, MQTT_BROKER_PORT, MQTT_BROKER_USER, MQTT_BROKER_PSWD, MQTT_CLIENT_ID);
Publisher pub(MQTT_PUB_TOPIC, mqttBase);
Subscriber sub(MQTT_SUB_TOPIC, mqttBase);
ApiServer server(SERVER_PORT);
Ws ws;
Pzem pzem;


void setup() {

  serial_init(SERIAL_BAUDRATE);

  logger.add_writer(&serial_writer);

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

  if (rc == 0) {
    using namespace std::placeholders;

    server.init();
    server.register_handler("/config/mqtt", "GET",
      std::bind(&MqttBase::mqtt_config_handler, &mqttBase, _1, _2, _3, _4));

    ws.init(WS_SERVER, WS_PORT);

    ws_writer.set_ws(&ws);
    logger.add_writer(&ws_writer);

    pzem.init();
  }

  led_init(false);

  logger.info("setup end!");
}

void loop() {

  mqttBase.loop();

  server.loop();

  ws.loop();

  time_t now = dt.now();

  if (difftime(now, last_read) > 10) {

    PzemData data;
    std::string json;

    if (pzem.loop(data)) {

      Pzem::data2json(data, json);

      pub.publish(json.c_str());
    }
    else {
      pub.publish("{\"error\": \"cannot read pzem data\"}");
    }

    last_read = now;
  }
}