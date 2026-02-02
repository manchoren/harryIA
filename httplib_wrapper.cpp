#include "httplib_wrapper.h"
#include "httplib.h"
#include "json.hpp"

HttpClient::HttpClient(const std::string &server_url) : server_url(server_url) {}

std::string HttpClient::post(const std::string &endpoint, const std::string &body)
{
    httplib::Client cli(server_url.c_str(), 8080);
    cli.set_connection_timeout(30); // 30 segundos para conexión
    cli.set_read_timeout(60);       // 60 segundos para leer respuesta
    cli.set_write_timeout(30);      // 30 segundos para escribir

    // cli.set_default_headers({{"Accept-Charset", "utf-8"}});

    auto res = cli.Post(endpoint.c_str(), body, "application/json");

    if (res)
    {
        if (res->status == 200)
        {
            return res->body;
        }
        else
        {
            // El servidor respondió, pero con un error (ej. 404, 500)
            nlohmann::json errorJson = {
                {"error", "http_error"},
                {"status", res->status},
                {"message", res->body}};
            return errorJson.dump();
        }
    }
    else
    {
        // La conexión falló, aquí es donde obtienes el error
        auto err = res.error();
        nlohmann::json errorJson = {
            {"error", "connection_error"},
            {"message", "Error de conexión: " + std::string(httplib::to_string(err))}};
        return errorJson.dump();
    }
}

std::string HttpClient::get(const std::string &endpoint)
{
    httplib::Client cli(server_url.c_str(), 8080);
    cli.set_connection_timeout(30); // 30 segundos para conexión
    cli.set_read_timeout(60);       // 60 segundos para leer respuesta
    cli.set_write_timeout(30);      // 30 segundos para escribir

    // cli.set_default_headers({{"Accept-Charset", "utf-8"}});

    auto res = cli.Get(endpoint.c_str());
    if (res)
    {
        if (res->status == 200)
        {
            return res->body;
        }
        else
        {
            // El servidor respondió, pero con un error (ej. 404, 500)
            nlohmann::json errorJson = {
                {"error", "http_error"},
                {"status", res->status},
                {"message", res->body}};
            return errorJson.dump();
        }
    }
    else
    {
        // La conexión falló, aquí es donde obtienes el error
        auto err = res.error();
        nlohmann::json errorJson = {
            {"error", "connection_error"},
            {"message", "Error de conexión: " + std::string(httplib::to_string(err))}};
        return errorJson.dump();
    }
}