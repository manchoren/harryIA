#include "httplib_wrapper.h"
#include "httplib.h"  // Incluir la biblioteca solo en este archivo

HttpClient::HttpClient(const std::string& server_url) : server_url(server_url) {}

std::string HttpClient::post(const std::string& endpoint, const std::string& body) {
    httplib::Client cli(server_url.c_str());

    cli.set_default_headers({{"Accept-Charset", "utf-8"}});
    auto res = cli.Post(endpoint.c_str(), body, "application/json");
    
    if (res && res->status == 200) {
        return res->body;
    } else {
        return "Error en la solicitud HTTP";
    }
}