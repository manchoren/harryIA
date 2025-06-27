#include <windows.h>
#include <string>
#include "json.hpp"
#include <regex>
#include <iostream>
#include <fstream>
#include <string_view>
#include "httplib_wrapper.h"

using namespace std; 

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

string trimNewlines(const string& input) {
    size_t start = 0, end = input.length();
    
    // Encontrar el primer caracter que no sea un salto de línea
    while (start < end && input[start] == '\n') {
        ++start;
    }
    
    // Encontrar el último caracter que no sea un salto de línea
    while (end > start && input[end - 1] == '\n') {
        --end;
    }
    
    string str = input.substr(start, end - start);

    auto start2 = std::find_if(str.begin(), str.end(), [](unsigned char ch) { return !std::isspace(ch); });
    auto end2 = std::find_if(str.rbegin(), str.rend(), [](unsigned char ch) { return !std::isspace(ch); }).base();
    
    return (start2 < end2) ? std::string(start2, end2) : std::string();
}

string procesarJSON(const string& retornoJson) {
    try {
        nlohmann::json data = nlohmann::json::parse(retornoJson);
        if (data.contains("content") && data["content"].is_string()) {
            string content = data["content"];
            size_t pos = content.find("<|eot_id|>");
            if (pos != string::npos) {
                content.erase(pos);
            }
            return content;
        }
        return "Error: La respuesta no contiene 'content'.";
    } catch (const nlohmann::json::parse_error& e) {
        return "Error de parsing JSON: " + string(e.what());
    }
}

string limpiarRespuestaIA(const string& retorno, const bool& onlyOneLine) { 
    string resultado = retorno; // Hacer una copia modificable

    // la ia suele empezar a inventarse conversaciones. asi que si "retorno" contiene "USUARIO:", eliminarlo y todo lo que haya a la derecha
    size_t pos = resultado.find("USUARIO:");
    if (pos != string::npos) {
        resultado = resultado.substr(0, pos);
    }

    // eliminar "HARRY:"
    resultado = regex_replace(resultado, regex("HARRY:"), "");

    // Eliminar espacios y saltos de línea al inicio y final de 'resultado'
    resultado = trimNewlines(resultado);

    // si onlyOneLine entonces solo traemos la primera linea.   
    if(onlyOneLine) {
        pos = resultado.find("\n");
        if (pos != string::npos) {
            resultado = resultado.substr(0, pos);
        }
    }

    return resultado;
}

// Variable global para almacenar el identificador del proceso del servidor
static HANDLE servidorProcess = NULL;

void ejecutarServidor(const Config& config) {
    STARTUPINFO si = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION pi;
    string comando = config.EJECUTABLE + " " + config.MODO_SERVIDOR + " " + config.PARAMETROS_EXTRA;

    if (CreateProcess(NULL, (LPSTR)comando.c_str(), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        servidorProcess = pi.hProcess;  // Guardamos el handle del proceso
        CloseHandle(pi.hThread);  // No necesitamos el handle del hilo
    } else {
        cerr << "Error al iniciar el servidor." << endl;
    }
}

void detenerServidor() {
    cout << "cerrando..." << endl;
    if (servidorProcess) {
        cout << "Deteniendo el servidor..." << endl;
        TerminateProcess(servidorProcess, 0);  // Mata el proceso
        CloseHandle(servidorProcess);  // Libera el handle
        servidorProcess = NULL;  // Limpiamos la variable
    }
}

string interactuarConModelo(const string& prompt,const Config& config) {
    HttpClient client(config.API_URL.c_str());
    nlohmann::json jsonBody = {
        {"prompt", prompt},
        {"n_predict", 128}
    };
    
    string respuesta = client.post(config.API_ENDPOINT, jsonBody.dump());
    return respuesta;
}

string cargarPromptJuegos(const string& gamesMemoryFile) {
    ifstream memoryFile(gamesMemoryFile);
    if (memoryFile.is_open()) {
        string contenido((istreambuf_iterator<char>(memoryFile)), istreambuf_iterator<char>());
        memoryFile.close();
        return contenido;
    }
    return "";
}

bool hayMemoria(const Config& config) { 
    ifstream memoryFile(config.MEMORY_FILE);
    return memoryFile.is_open();
}

string cargarMemoria(const Config& config) {
    ifstream memoryFile(config.MEMORY_FILE);
    if (memoryFile.is_open()) {
        string contenido((istreambuf_iterator<char>(memoryFile)), istreambuf_iterator<char>());
        memoryFile.close();
        return contenido;
    }
    
    ifstream startFile(config.START_FILE);
    if (startFile.is_open()) {
        string contenido((istreambuf_iterator<char>(startFile)), istreambuf_iterator<char>());
        startFile.close();
        ofstream newMemoryFile(config.MEMORY_FILE);
        newMemoryFile << contenido;
        newMemoryFile.close();
        return contenido;
    }
    
    return "";
}

void guardarMemoria(const string& mensaje, const Config& config) {
    ofstream memoryFile(config.MEMORY_FILE, ios::app);
    if (memoryFile.is_open()) {
        memoryFile << mensaje << endl;
        memoryFile.close();
    }
}

void guardarBAT(const string& gamefolder, const string& comando) {
    const char sep = std::filesystem::path::preferred_separator;
    ofstream batFile(gamefolder +sep+"HARRYRUN.BAT", ios::app);
    if (batFile.is_open()) {
        batFile << "@echo off" << endl;
        batFile << comando << endl;
        batFile.close();
    }
}

bool hayBat(const string& directory) {
    const char sep = std::filesystem::path::preferred_separator;
    std::filesystem::path filePath = directory + sep +  "HARRYRUN.BAT";
    return std::filesystem::exists(filePath);
}

std::string reemplazarToken(std::string texto, const std::string& token, const std::string& reemplazo) {
    size_t pos = 0;
    while ((pos = texto.find(token, pos)) != std::string::npos) {
        texto.replace(pos, token.length(), reemplazo);
        pos += reemplazo.length(); // Avanzar para evitar bucles infinitos
    }
    return texto;
}

std::string toLower(const std::string& str) {
    std::string lowerStr = str;
    std::transform(lowerStr.begin(), lowerStr.end(), lowerStr.begin(), [](unsigned char c) {
        return std::tolower(c);
    });
    return lowerStr;
}

bool isIgnoredFile(const std::string& filename) {
    std::string name = toLower(filename);
    return (name == "setup" || name == "install");
}

void buildDirectoryTree(const std::filesystem::path& path, std::string& output, std::string& firstExeCom, const std::string& prefix = "") {
    for (const auto& entry : std::filesystem::directory_iterator(path)) {
        if (entry.is_directory()) {
            output += prefix + "[DIR] " + entry.path().filename().string() + "\n";
            buildDirectoryTree(entry.path(), output, firstExeCom, prefix + "  ");
        } else {
            std::string filename = entry.path().stem().string(); // Nombre sin extensión
            std::string ext = toLower(entry.path().extension().string());

            if ((ext == ".exe" || ext == ".com") && !isIgnoredFile(filename)) {
                output += prefix + "[FILE] " + entry.path().filename().string() + "\n";
                
                // Guardar el primer archivo encontrado si no ha sido asignado aún
                if (firstExeCom.empty()) {
                    firstExeCom = entry.path().string();
                }
            }
        }
    }
}

// Función para guardar el comando en un archivo log
void logToFile(const std::string& command, const std::string& file) {
    std::ofstream logFile(file, std::ios::app); // Abre el archivo en modo de añadir
    if (logFile.is_open()) {
        logFile << command << std::endl;
        logFile.close();
    } else {
        std::cerr << "Error al abrir el archivo " << file << std::endl;
    }
}

constexpr uint64_t getHash(const char* str, uint64_t hash = 14695981039346656037ULL) {
    return (*str ? getHash(str + 1, (hash ^ *str) * 1099511628211ULL) : hash);
}
