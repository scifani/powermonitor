#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <map>

#include "http_status_codes.h"

class MqttBase : WiFiClient {

public:
    MqttBase(const char* host, int port, const char* client_id);
    virtual ~MqttBase();

    void register_topic(const char* topic, std::function<void(char*, unsigned int)> callback);
    void init();
    void publish(const char* topic, const char* message);
    void loop();

    HttpStatus::Code mqtt_config_handler(
        const std::string& path,
        const std::string& body,
        const std::map<std::string, std::string>& query_params,
        std::string& response);

private:
    void reconnect(int num_retries = 0);
    void subscribers_callback(char* topic, uint8_t* payload, unsigned int length);

    PubSubClient _mqtt_client;
    char _client_id[128];
    char _host[128];
    int _port;
    std::map<std::string, std::function<void(char*, unsigned int)>> _callbacks;
};


class Publisher {

public:
    Publisher(const char* topic, MqttBase& mqtt_base);
    virtual ~Publisher();

    void publish(const char* message);

private:
    MqttBase* _mqtt_base;
    char _topic[128];
};

class Subscriber {

public:
    Subscriber(const char* topic, MqttBase& mqtt_base);
    virtual ~Subscriber();

    void process(char* message, unsigned int length);

private:
    MqttBase* _mqtt_base;
    char _topic[128];
};
