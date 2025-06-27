#ifndef UTILS_H
#define UTILS_H

#include <windows.h>
#include <string>
#include "json.hpp"
#include <regex>
#include <iostream>
#include <fstream>
#include "httplib_wrapper.h"

// Estructura para leer las configuraciones
struct Config {
    std::string EJECUTABLE;
    int HABILITAR_SERVIDOR;
    std::string MODO_SERVIDOR;
    std::string PARAMETROS_EXTRA;
    std::string API_URL;
    std::string API_ENDPOINT;
    int TIEMPO_ESPERA_SERVIDOR;
    std::string LOG_FILE;
    std::string MEMORY_FILE;
    std::string START_FILE;
    std::string GAMES_DIR;
    std::string GAMES_MEMORY_FILE;
    std::string GAMES_MEMORY_RUN_FILE;
    std::string DOSBOX_DIR;

    std::string PIPER_EXE;
    std::string PIPER_MODEL;
    std::string VLC_EXE;
    std::string VLC_EXTRA_PARAMS; 

    int DEBUG; 
};

std::string trimNewlines(const std::string& input);
std::string procesarJSON(const std::string& retornoJson);
std::string limpiarRespuestaIA(const std::string& retorno, const bool& onlyOneLine);
void ejecutarServidor(const Config& config);
void detenerServidor();
std::string interactuarConModelo(const std::string& prompt, const Config& config);
bool hayMemoria(const Config& config);
std::string cargarMemoria(const Config& config);
std::string cargarPromptJuegos(const std::string& gamesMemoryFile);
void guardarMemoria(const std::string& mensaje, const Config& config);
void guardarBAT(const std::string& gamefolder, const std::string& comando);
bool hayBat(const std::string& directory);
std::string reemplazarToken(std::string texto, const std::string& token, const std::string& reemplazo);
std::string toLower(const std::string& str) ;
void buildDirectoryTree(const std::filesystem::path& path, std::string& output, std::string& firstExeCom, const std::string& prefix = "");
void logToFile(const std::string& command, const std::string& file);
constexpr uint64_t getHash(const char* str, uint64_t hash = 14695981039346656037ULL) {
    return (*str ? getHash(str + 1, (hash ^ *str) * 1099511628211ULL) : hash);
}

#endif // UTILS_H
