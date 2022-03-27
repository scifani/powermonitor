#include <Arduino.h>
#include <DateTime.h>
#include <logger.h>
#include <time.h>
#include "init.h"
#include "parameters.h"
#include "mqtt.h"
#include "api_server.h"
#include "pzem.h"

byte mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
IPAddress ip(192, 168, 1, 177);

Logger logger;
UsbSimulatorWriter usb_writer(&DEBUG_SERIAL);
Publisher pub(MQTT_PUB_TOPIC, MqttBase::getInstance());
Subscriber sub(MQTT_SUB_TOPIC, MqttBase::getInstance());
ApiServer server(SERVER_PORT);
PzemArray pzemArray;
Pzem pzem1(PZEM_1_RX_PIN, PZEM_1_TX_PIN);
Pzem pzem5(PZEM_5_RX_PIN, PZEM_5_TX_PIN);

time_t last_read = 0;

void setup() {

  serial_init(SERIAL_BAUDRATE);

  logger.add_writer(&usb_writer);

  eth_init(ip, mac);
    
  MqttBase::getInstance().init(MQTT_BROKER_ADDRESS, MQTT_BROKER_PORT, MQTT_BROKER_USER, MQTT_BROKER_PSWD, MQTT_CLIENT_ID);

  server.init();  

  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());

  pzem1.init();
  pzem5.init();

  pzemArray.addPzem(&pzem1, 1);
  pzemArray.addPzem(&pzem5, 5);
}

 void loop() {

  MqttBase::getInstance().loop();

  std::vector<PzemData> data;
  
  time_t now = DateTime.now();

  if ((now - last_read) > 10) {  
    if (pzemArray.loop(data)) {

      for (auto it : data) {
        std::string json;
        Pzem::data2json(it, json);
        pub.publish(json.c_str());
      }
    }
    else {
      pub.publish("{\"error\": \"cannot read pzem data\"}");
    }

    last_read = now;
  }

/*
  // listen for incoming clients
  EthernetClient client = server.available();
  if (client) {
    Serial.println("new client");
    // an http request ends with a blank line
    boolean currentLineIsBlank = true;
    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);
        // if you've gotten to the end of the line (received a newline
        // character) and the line is blank, the http request has ended,
        // so you can send a reply
        if (c == '\n' && currentLineIsBlank) {
          // send a standard http response header
          client.println("HTTP/1.1 200 OK");
          client.println("Content-Type: text/html");
          client.println("Connection: close");  // the connection will be closed after completion of the response
          client.println("Refresh: 5");  // refresh the page automatically every 5 sec
          client.println();
          client.println("<!DOCTYPE HTML>");
          client.println("<html>");
          // output the value of each analog input pin
          for (int analogChannel = 0; analogChannel < 6; analogChannel++) {
            int sensorReading = analogRead(analogChannel);
            client.print("analog input ");
            client.print(analogChannel);
            client.print(" is ");
            client.print(sensorReading);
            client.println("<br />");
          }
          client.println("</html>");
          break;
        }
        if (c == '\n') {
          // you're starting a new line
          currentLineIsBlank = true;
        } else if (c != '\r') {
          // you've gotten a character on the current line
          currentLineIsBlank = false;
        }
      }
    }
    // give the web browser time to receive the data
    delay(1);
    // close the connection:
    client.stop();
    Serial.println("client disconnected");
*/    

}