#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <map>

class MqttBase : WiFiClient {

public:
    MqttBase(const char* host, int port, const char* client_id);
    virtual ~MqttBase();

    void register_topic(const char* topic, std::function<void(char*, unsigned int)> callback);
    void init();
    void publish(const char* topic, const char* message);
    void loop();

private:
    void reconnect();
    void subscribers_callback(char* topic, uint8_t* payload, unsigned int length);

    PubSubClient _mqtt_client;
    char _client_id[128];
    char _host[128];
    int _port;
    std::map<String, std::function<void(char*, unsigned int)>> _callbacks;
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
