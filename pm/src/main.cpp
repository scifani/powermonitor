#include "parameters.h"
#include "logger.h"
#include "init.h"
#include "mqtt.h"
#include "api_server.h"
#include "ws.h"

SerialWriter serial_writer(&DEBUG_SERIAL);
SerialWriter serial_writer1(&Serial);
//WsWriter ws_writer;
Logger logger;

MqttBase mqttBase(MQTT_BROKER_ADDRESS, MQTT_BROKER_PORT, MQTT_CLIENT_ID);
Publisher pub(MQTT_PUB_TOPIC, mqttBase);
Subscriber sub(MQTT_SUB_TOPIC, mqttBase);
ApiServer server(SERVER_PORT);
Ws ws;

int publish_count = 0;
int uart_read_count = 0;
char serial_buff[128];

void setup() {

  serial_init(SERIAL_BAUDRATE);

  logger.add_writer(&serial_writer);
  logger.add_writer(&serial_writer1);

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

    //ws_writer.set_ws(&ws);
    //logger.add_writer(&ws_writer);
  }

  led_init(false);

  logger.info("setup end!");
}

void loop() {

  mqttBase.loop();

  server.loop();

  ws.loop();

  if (publish_count == 10000) {
    pub.publish("funzionerà???");
    publish_count = 0;

    ws.sendTxt("funzionerà???");
  }
  else {
    publish_count++;
  }

/*
  if (uart_read_count == 10000) {
    if(Serial.available() > 0) {
      int sz = Serial.read(serial_buff, sizeof(serial_buff));
      std::stringstream ss;
      ss << "RX[" << sz << "]: " << serial_buff;
      pub.publish(ss.str().c_str());
    }
    uart_read_count = 0;
  }
  else {
    uart_read_count++;
  }
*/
  //0x01 + 0x04 + 0x00 + 0x00 + 0x00 + 0x0A + 0xHH + 0xLL


}