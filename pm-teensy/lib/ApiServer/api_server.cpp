#include "api_server.h"
#include "logger.h"

extern Logger logger;

ApiServer::ApiServer(uint16_t port) :
    EthernetServer(port)
{
}

ApiServer::~ApiServer() {

}

void ApiServer::init() {
    logger.info("ApiServer::init - Called.");

    begin();
}

void ApiServer::register_handler(const char* path, const char* verb, HttpHandlerFunction handler) {

    logger.info("ApiServer::register_handler - Begin. path=%s, verb=%s", path, verb);

    if (std::string(verb) == "GET") {
        _get_handlers[path] = handler;
    }
    else if (std::string(verb) == "POST") {
        logger.error("ApiServer::register_handler - %s is not supported", verb);
    }
    else if (std::string(verb) == "PUT") {
        logger.error("ApiServer::register_handler - %s is not supported", verb);
    }
    else if (std::string(verb) == "DELETE") {
        logger.error("ApiServer::register_handler - %s is not supported", verb);
    }
    else {
        logger.error("ApiServer::register_handler - unknown verb %s", verb);
    }

    logger.info("ApiServer::register_handler - End.");
}

HttpHandlerFunction ApiServer::get_handler(std::string& path, std::string& verb) {

    logger.info("ApiServer::get_handler - Begin. path=%s, verb=%s", path.c_str(), verb.c_str());

    HttpHandlerFunction handler = nullptr;

    if (std::string(verb) == "GET") {

        auto it = _get_handlers.find(std::string(path));

        if (it != _get_handlers.end()) {
            handler = (*it).second;
        }
    }
    else if (std::string(verb) == "POST") {
        logger.error("ApiServer::register_handler - %s is not supported", verb.c_str());
    }
    else if (std::string(verb) == "PUT") {
        logger.error("ApiServer::register_handler - %s is not supported", verb.c_str());
    }
    else if (std::string(verb) == "DELETE") {
        logger.error("ApiServer::register_handler - %s is not supported", verb.c_str());
    }
    else {
        logger.error("ApiServer::register_handler - unknown verb %s", verb.c_str());
    }

    logger.info("ApiServer::get_handler - End. handler %s", handler ? "found" : "not found");
    return handler;
}

void ApiServer::loop() {

    EthernetClient client = available();

    if (!client) {
        return;
    }

    if (client.available()) {

        std::stringstream request;

        int num_bytes = 0;
        do {
            memset(_buff, 0, sizeof(_buff));
            num_bytes = client.read(_buff, sizeof(_buff));
            request << _buff;
        } while (num_bytes > 0);

        logger.info("ApiServer::loop - request received: %s", request.str().c_str());

        std::vector<std::string> request_lines;
        StringUtils::tokenize(request.str(), '\n', request_lines);

        if (request_lines.size() == 0) {
            logger.error("ApiServer::loop - tokenizing request returned an empty vector");
            return;
        }

        std::string verb;
        std::string path;
        std::string http;
        std::map<std::string, std::string> query_params;
        bool ret = parse_request_line(request_lines, verb, path, http, query_params);

        if (!ret) {
            logger.error("ApiServer::loop - cannot parse request first line");
            return;
        }

        std::map<std::string, std::string> headers = get_headers(request_lines);

        std::stringstream body;
        for (auto it : request_lines)
            body << it;

        auto handler = get_handler(path, verb);

        HttpStatus::Code status_code;
        std::string response_body;

        if (handler) {
            status_code = handler(path, body.str(), query_params, response_body);
        }
        else {
            status_code = HttpStatus::Code::NotFound;
            response_body = print_request(verb, path, http, headers, body.str());
        }

        std::stringstream response;
        response << "HTTP/1.1 " << HttpStatus::toInt(status_code) << " " << HttpStatus::reasonPhrase(status_code) << "\n";
        response << "Content-Type: application/json" << "\n";
        response << "Content-Length: " << response_body.length() << "\n";
        response << "\n";
        response << response_body << "\n";
        response << "\n";

        // Return the response
        client.println(response.str().c_str());
        client.flush();
    }
}

bool ApiServer::parse_request_line(std::vector<std::string>& request_lines, std::string& verb,
    std::string& path, std::string& http, std::map<std::string, std::string>& query_params) {

    bool ret = false;
    std::string line_ = request_lines[0];
    std::vector<std::string> v;

    StringUtils::tokenize(
        StringUtils::remove_newline(line_), ' ', v);

    if (v.size() == 3) {
        verb = v[0];
        path = v[1];
        http = v[2];

        size_t query_pos = path.find('?');
        if (query_pos != std::string::npos) {
            std::string s = path.substr(query_pos + 1);
            path = path.substr(0, query_pos);

            std::vector<std::string> v_params;
            StringUtils::tokenize(s, '&', v_params);
            for (auto it : v_params) {
                size_t x = it.find('=');
                if (x != std::string::npos) {
                    query_params.insert(
                        std::pair<std::string, std::string>(
                            StringUtils::trim_copy(it.substr(0, x)),
                            StringUtils::trim_copy(it.substr(x + 1))
                        )
                    );
                }
            }
        }

        request_lines.erase(request_lines.begin());

        ret = true;
    }
    else {
        logger.error("ApiServer::parse_request_line - Invalid length (%d) tokenizing of request_line", v.size());
    }

    return ret;
}

std::map<std::string, std::string> ApiServer::get_headers(std::vector<std::string>& request_lines) {

    int count = 0;
    std::map<std::string, std::string> headers;

    for(auto it : request_lines) {

        std::string s = StringUtils::remove_newline(it);

        if (StringUtils::trim_copy(s).size() == 0)
            break;

        if (s.find(':') != std::string::npos) {
            std::vector<std::string> v;
            StringUtils::tokenize(s, ':', v);

            headers.insert(
                std::pair<std::string, std::string>(
                    StringUtils::trim_copy(v[0]),
                    StringUtils::trim_copy(v[1])
                )
            );

            count++;
        }
    }

    request_lines.erase(request_lines.begin(), request_lines.begin() + count + 1);

    return headers;
}

std::string ApiServer::print_request(std::string verb, std::string path,
    std::string http, std::map<std::string, std::string> headers, std::string body) {

    std::stringstream ss;
    ss << "{" << "\n";
    ss << "\t" << "\"verb\": \"" << verb << "\", " << "\n";
    ss << "\t" << "\"path\": \"" << path << "\", " << "\n";
    ss << "\t" << "\"http\": \"" << http << "\", " << "\n";
    ss << "\t" << "\"headers\": [" << "\n";
    for (auto it = headers.begin(); it != headers.end(); ++it) {
        ss << "\t\t" << "{\"name\": \"" << (*it).first << "\", \"value\": \"" << (*it).second << "\"}";
        ss << (std::next(it) == headers.end() ? "\n" : ",\n");
    }
    ss << "\t" << "]" << "\n";
    ss << "\t" << "\"body\": \"" << body << "\"" << "\n";
    ss << "}";

    return ss.str();
}
