#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "logger.h"

extern Logger logger;

class Subscriber : WiFiClient {

public:
    Subscriber(const char* host, int port, const char* client_id, const char* topic) :
        _mqtt_client(*this)
    {
        strncpy(_host, host, sizeof(_host));
        strncpy(_client_id, client_id, sizeof(_client_id));
        strncpy(_topic, topic, sizeof(_topic));
        _port = port;
    }

    virtual ~Subscriber() {

    }

    static void callback(char* topic, byte* payload, unsigned int length) {
        Serial.print("Message arrived [");
        Serial.print(topic);
        Serial.print("] ");
        for (int i=0;i<length;i++) {
            Serial.print((char)payload[i]);
        }
        Serial.println();
    }

    void reconnect() {
        logger.debug("Subscriber::reconnect - begin.");

        while (!_mqtt_client.connected()) {
            logger.debug("Subscriber::reconnect - attempting MQTT connection...");

            if (_mqtt_client.connect(_client_id)) {
                logger.info("Subscriber::reconnect - connected");
            } else {
                logger.error("Subscriber::reconnect - rc = %d - Try again in 5 seconds...", _mqtt_client.state());
                delay(5000);
            }
        }

        logger.debug("Subscriber::reconnect - end.");
    }

    void init() {
        logger.debug("Subscriber::init - begin. host=%s, port=%d", _host, _port);

        _mqtt_client.setServer(_host, _port);
        _mqtt_client.setCallback(this->callback);

        logger.debug("Subscriber::init - end.");
    }

    void loop() {
        _mqtt_client.loop();
    }


private:
    PubSubClient _mqtt_client;
    char _client_id[128];
    char _topic[128];
    char _host[128];
    int _port;
};
