#include <ESP8266WiFi.h>

class ApiServer : public WiFiServer {

public:
    ApiServer(uint16_t port);
    virtual ~ApiServer();

    void init();
    void listen();
};