#include <SPI.h>
#include <NativeEthernet.h>
#include <PubSubClient.h>
#include "http_status_codes.h"
#include "../StringUtils/string_utils.h"
#include <map>
#include <functional>

class MqttBase : EthernetClient {

public:
    static MqttBase& getInstance()
    {
        static MqttBase instance;
        return instance;
    }
private:
    MqttBase();

public:
    MqttBase(MqttBase const&)       = delete;
    void operator=(MqttBase const&) = delete;    
    virtual ~MqttBase();
    
    void register_topic(const char* topic, std::function<void(char*, unsigned int)> callback);
    void init(const char* host, int port, const char* user, const char* pswd, const char* client_id);
    void publish(const char* topic, const char* message);
    void loop();

    HttpStatus::Code mqtt_config_handler(
        const std::string& path,
        const std::string& body,
        const std::map<std::string, std::string>& query_params,
        std::string& response);

private:
    void reconnect(int num_retries = 0);
    static void subscribers_callback(char* topic, uint8_t* payload, unsigned int length);    

    PubSubClient _mqtt_client;
    std::string _client_id;
    std::string _user;
    std::string _pswd;
    std::string _host;
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
