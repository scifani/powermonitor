#include "mqtt.h"
#include "logger.h"

extern Logger logger;

MqttBase::MqttBase() :
    _mqtt_client(*this) {
}

MqttBase::~MqttBase() {
}

void MqttBase::register_topic(const char* topic, std::function<void(char*, unsigned int)> callback) {
    _callbacks[topic] = callback;
}

void MqttBase::init(const char* host, int port, const char* user, const char* pswd, const char* client_id) {
    logger.debug("MqttBase::init - begin. host=%s, port=%d, user=%s, client_id=%s", host, port, user, client_id);
  
    _host = host;
    _port = port;
    _user = user;
    _pswd = pswd;
    _client_id = client_id;    

    _mqtt_client.setServer(_host.c_str(), _port);

    if (_callbacks.size() > 0) {        
        _mqtt_client.setCallback(MqttBase::subscribers_callback);
    }

    logger.debug("MqttBase::init - end.");
}

void MqttBase::publish(const char* topic, const char* message) {
    logger.debug("MqttBase::publish - begin. topic=%s, message=%s", topic, message);

    if (!_mqtt_client.connected())
        reconnect();

    _mqtt_client.publish(topic, message);

    logger.debug("MqttBase::publish - end.");
}

void MqttBase::loop() {

    if (!_mqtt_client.connected())
        reconnect();

    _mqtt_client.loop();
}

void MqttBase::reconnect(int num_retries) {
    logger.debug("MqttBase::reconnect - begin. client_id=%s, user=%s, pswd=%s, num_retries=%d", _client_id.c_str(), _user.c_str(), _pswd.c_str(), num_retries);

    bool connected = false;
    int count = 0;

    while (!_mqtt_client.connected()) {
        logger.debug("MqttBase::reconnect - attempting MQTT connection...");

        connected = _mqtt_client.connect(_client_id.c_str(), _user.c_str(), _pswd.c_str());
        count++;

        if (connected) {
            logger.info("MqttBase::reconnect - connected");

            for(const auto &it : _callbacks) {
                logger.debug("MqttBase::reconnect - registering topic %s", it.first.c_str());

                _mqtt_client.subscribe(it.first.c_str());
            }
        }
        else {
            if (num_retries > count) {
                logger.error("MqttBase::reconnect - rc = %d - Try again in 5 seconds...", _mqtt_client.state());
                delay(5000);
            }
            else {
                break;
            }
        }
    }

    logger.debug("MqttBase::reconnect - end.");
}

void MqttBase::subscribers_callback(char* topic, uint8_t* payload, unsigned int length) {

    logger.debug("MqttBase::subscribers_callback - topic=%s", topic);

    MqttBase::getInstance();

    auto it = MqttBase::getInstance()._callbacks.find(std::string(topic));

    if (it != MqttBase::getInstance()._callbacks.end()) {
        logger.debug("MqttBase::subscribers_callback - topic found");

        char* buff = new char[length + 1];
        strncpy(buff, (const char*)payload, length);

        (*it).second(buff, length);

    } else {
        logger.warning("MqttBase::subscribers_callback - topic not found");
    }
}

HttpStatus::Code MqttBase::mqtt_config_handler(
    const std::string& path,
    const std::string& body,
    const std::map<std::string, std::string>& query_params,
    std::string& response) {

    logger.info("MqttBase::mqtt_config_handler - Begin. path=%s, body=%s, query_params.len=%d", path.c_str(), body.c_str(), query_params.size());

    auto host_it = query_params.find("host");
    auto port_it = query_params.find("port");
    auto user_it = query_params.find("user");
    auto pswd_it = query_params.find("pswd");

    if (host_it != query_params.end() && port_it != query_params.end()) {
        _host = (*host_it).second;
        _port = atoi((*port_it).second.c_str());

        if (user_it != query_params.end() && pswd_it != query_params.end()) {
            _user = (*user_it).second;
            _pswd = (*pswd_it).second;
        }

        logger.info("MqttBase::mqtt_config_handler - mqtt broker set to %s:%d", _host.c_str(), _port);

        _mqtt_client.disconnect();
        _mqtt_client.setServer(_host.c_str(), _port);
        _mqtt_client.connect(_client_id.c_str(), _user.c_str(), _pswd.c_str());
    }

    HttpStatus::Code status_code = HttpStatus::Code::OK;

    logger.info("MqttBase::mqtt_config_handler - End. status_code=%d", (int)status_code);
    return status_code;
}

Publisher::Publisher(const char* topic, MqttBase& mqtt_base) {
    strncpy(_topic, topic, sizeof(_topic));
    this->_mqtt_base = &mqtt_base;
}

Publisher::~Publisher() {
}

void Publisher::publish(const char* message) {
    this->_mqtt_base->publish(_topic, message);
}

Subscriber::Subscriber(const char* topic, MqttBase& mqtt_base) {

    using namespace std::placeholders;

    strncpy(_topic, topic, sizeof(_topic));
    this->_mqtt_base = &mqtt_base;
    this->_mqtt_base->register_topic(topic,
        std::bind(&Subscriber::process, this, _1, _2));
}

Subscriber::~Subscriber() {
}

void Subscriber::process(char* message, unsigned int length) {
    logger.debug("Subscriber::process - message= %s", message);
}