#include <WebSocketsClient.h>
#include "logger.h"

class Ws {
public:
    Ws();
    virtual ~Ws();

    void init(const char * host, uint16_t port);
    void loop();
    void sendTxt(const char* payload);

private:
    void webSocketEvent(WStype_t type, uint8_t * payload, size_t length);

    WebSocketsClient _webSocket;
    bool _connected;
};

class WsWriter : public ILogWriter {
public:
    void set_ws(Ws* ws) {
        _ws = ws;
    }

    void write(const char* message) {
        _ws->sendTxt(message);
    }

private:
    Ws* _ws;
};