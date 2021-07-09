#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "logger.h"

extern Logger logger;

class Publisher : WiFiClient {

public:
    Publisher(const char* host, int port, const char* client_id, const char* topic) :
        _mqtt_client(*this)
    {
        strncpy(_host, host, sizeof(_host));
        strncpy(_client_id, client_id, sizeof(_client_id));
        strncpy(_topic, topic, sizeof(_topic));
        _port = port;
    }

    virtual ~Publisher() {

    }

    void reconnect() {
        logger.debug("Publisher::reconnect - begin.");

        while (!_mqtt_client.connected()) {
            logger.debug("Publisher::reconnect - attempting MQTT connection...");

            if (_mqtt_client.connect(_client_id)) {
                logger.info("Publisher::reconnect - connected");
            } else {
                logger.error("Publisher::reconnect - rc = %d - Try again in 5 seconds...", _mqtt_client.state());
                delay(5000);
            }
        }

        logger.debug("Publisher::reconnect - end.");
    }

    void init() {
        logger.debug("publisher.init - begin. host=%s, port=%d", _host, _port);

        _mqtt_client.setServer(_host, _port);

        logger.debug("publisher.init - end.");
    }

    void publish(const char* message) {
        if (!_mqtt_client.connected())
            reconnect();

        _mqtt_client.publish(_topic, message);
    }

private:
    PubSubClient _mqtt_client;
    char _client_id[128];
    char _topic[128];
    char _host[128];
    int _port;
};
