#include <ESP8266WiFi.h>

#include <sstream>
#include <vector>
#include <map>

#include "http_status_codes.h"

typedef std::function<HttpStatus::Code(
    const std::string& path,
    const std::string& body,
    const std::map<std::string, std::string>& query_params,
    std::string& response)> HttpHandlerFunction;

class ApiServer : public WiFiServer {

public:
    ApiServer(uint16_t port);
    virtual ~ApiServer();

    void init();
    void loop();
    void register_handler(const char* path, const char* verb, HttpHandlerFunction handler);

private:
    bool parse_request_line(std::vector<std::string>& request_lines, std::string& verb,
        std::string& path, std::string& http, std::map<std::string, std::string>& query_params);
    std::map<std::string, std::string> get_headers(std::vector<std::string>& request_lines);
    HttpHandlerFunction get_handler(std::string& path, std::string& verb);
    std::string print_request(std::string verb, std::string path,
        std::string http, std::map<std::string, std::string> headers, std::string body);

    char _buff[512];
    std::map<std::string, HttpHandlerFunction> _get_handlers;
};