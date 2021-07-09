#include "api_server.h"

ApiServer::ApiServer(uint16_t port) :
    WiFiServer(port)
{
}

ApiServer::~ApiServer() {

}

void ApiServer::init() {

    begin();
}

void ApiServer::listen() {

    WiFiClient client = available();
    if (!client)
        return;

    while(!client.available()){
        delay(1);
    }

    char buff[256];
    String request;

    while (client.read(buff, sizeof(buff)) > 0)
        request.concat(buff);

    Serial.println(request);

    // Return the response
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: application/json");
    client.println(""); //  do not forget this one
    client.println("{\"name\": \"abcd\"}");

    client.flush();
}
