#ifndef HTTPLIB_WRAPPER_H
#define HTTPLIB_WRAPPER_H

#include <string>

class HttpClient {
public:
    HttpClient(const std::string& server_url);
    std::string post(const std::string& endpoint, const std::string& body);
    
private:
    std::string server_url;
};

#endif // HTTPLIB_WRAPPER_H