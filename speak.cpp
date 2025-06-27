#include <iostream>
#include <string>
#include <cstdlib>
#include "utils.h" 
#include <string>
#include <windows.h>
#include <vector>
#include <thread>
#include <sstream>
#include <unordered_map>

// Función para eliminar acentos, reemplazar "ñ" por "ni" y corregir "HARRY" → "JARRY"
std::string normalizeText(const std::string& text) {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
    std::wstring wtext = converter.from_bytes(text);
    
    std::wstring result;
    std::unordered_map<wchar_t, wchar_t> replacements = {
        //descomentar lo siguiente para que elimine los acentos
     //   {L'Á', L'A'}, {L'É', L'E'}, {L'Í', L'I'}, {L'Ó', L'O'}, {L'Ú', L'U'},
    //    {L'á', L'a'}, {L'é', L'e'}, {L'í', L'i'}, {L'ó', L'o'}, {L'ú', L'u'},
     //   {L'Ü', L'U'}, {L'ü', L'u'}, {L'Ñ', L'N'}, {L'ñ', L'n'}

     // elimina signos de puntuacion y demas simbolos
     {L'.', L' '}, {L',', L' '}, {L';', L' '}, {L'?', L' '}, {L'¿', L' '}, {L'!', L' '}, {L'¡', L' '}
    };

    // Reemplazo de acentos y caracteres especiales
    for (wchar_t wc : wtext) {
        if (replacements.count(wc)) {
            result += replacements[wc];
        } else if (wc != L'"' && wc != L'\'') { // Eliminar comillas dobles y simples
            result += wc;
        }
    }

    // Convertir a mayúsculas para buscar "HARRY"
    std::wstring wtextUpper = result;
    for (wchar_t& wc : wtextUpper) wc = towupper(wc);

    // Buscar y reemplazar "HARRY" con "JARRY"
    size_t startPos = 0;
    while ((startPos = wtextUpper.find(L"HARRY", startPos)) != std::wstring::npos) {
        result.replace(startPos, 5, L"JARRY");
        startPos += 5;
    }

    return converter.to_bytes(result);
}

void speak_async(const Config& config, const std::string& text) {
    std::istringstream stream(text);
    std::string line;
    std::vector<std::string> lines;

    // Dividir el texto en líneas
    while (std::getline(stream, line)) {
        if (!line.empty()) {
            lines.push_back(line);
        }
    }

    for (size_t i = 0; i < lines.size(); ++i) {
        std::string command = "cmd /C \"echo " + normalizeText(lines[i]) + 
                          " | " + config.PIPER_EXE + " --model " + config.PIPER_MODEL + " --output-raw " +
                          " | " + config.VLC_EXE + " "+ config.VLC_EXTRA_PARAMS +" > NUL 2>&1\""; 

        if (config.DEBUG == 1) {
            logToFile(command, "speak.log");
        }

        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        si.dwFlags |= STARTF_USESHOWWINDOW;
        si.wShowWindow = SW_HIDE; // Oculta la ventana de la consola

        ZeroMemory(&pi, sizeof(pi));

        // Ejecutar la línea en segundo plano
        if (CreateProcess(NULL, (LPSTR)command.c_str(), NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
            // Esperar a que termine esta línea antes de seguir con la siguiente
            WaitForSingleObject(pi.hProcess, INFINITE);
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
        } else {
            std::cerr << "Error al ejecutar el comando" << std::endl;
        }

        // Agregar una pequeña pausa entre líneas (simula una pausa natural)
        if (i < lines.size() - 1) {
            Sleep(500); // Pausa de 500ms
        }
    }
}

// Función para iniciar `speak_async` en un hilo separado
void speak(const Config& config, const std::string& text) {
    std::thread t(speak_async, std::ref(config), text);
    t.detach(); // Permite que el hilo se ejecute en segundo plano sin bloquear el programa
}