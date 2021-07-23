#include "ws.h"

extern Logger logger;

Ws::Ws() {
    _connected = false;
}

Ws::~Ws() {

}

void Ws::init(const char * host, uint16_t port) {

    using namespace std::placeholders;

    _webSocket.begin(host, port, "/");
    _webSocket.onEvent(std::bind(&Ws::webSocketEvent, this, _1, _2, _3));
}

void Ws::loop() {
    _webSocket.loop();
}

void Ws::sendTxt(const char* payload) {
    _webSocket.sendTXT(payload);
}

void Ws::webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {

    switch(type) {
        case WStype_DISCONNECTED:
            _connected = false;
            break;
        case WStype_CONNECTED:
            _connected = true;
            _webSocket.sendTXT("Connected");
            break;
        case WStype_TEXT:
            break;
        case WStype_BIN:
            hexdump(payload, length);
            break;
        case WStype_PING:
            // pong will be send automatically
            break;
        case WStype_PONG:
            // answer to a ping we send
            break;
        case WStype_ERROR:
            break;
        default:
            break;
    }
}