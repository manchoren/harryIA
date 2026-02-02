#define CPPHTTPLIB_NO_SSL
#include <windows.h>
#include <thread>
#include <string>
#include <chrono>
#include "ini.h"
#include <locale>
#include <codecvt>
#include <csignal>
#include <atomic>

#include "autoexec.h"
#include "utils.h"
#include "speak.h"
// #include "listen.h"

using namespace std;

atomic<bool> running(true);
thread servidorThread;

// Función para manejar señales (ej. Ctrl + C)
BOOL WINAPI signalHandler(DWORD signal)
{
    if (signal == CTRL_C_EVENT)
    {
        cout << "\nCerrando el servidor...\n";
        running = false;

        detenerServidor();

        if (servidorThread.joinable())
        {
            servidorThread.join(); // Asegura que el hilo del servidor se cierre
        }

        exit(0); // Salir limpiamente
    }
    return TRUE;
}

// Callback para leer las claves del archivo .ini
static int handler(void *user, const char *section, const char *name, const char *value)
{
    Config *config = (Config *)user;
    if (strcmp(name, "EJECUTABLE") == 0)
        config->EJECUTABLE = value;
    else if (strcmp(name, "HABILITAR_SERVIDOR") == 0)
        config->HABILITAR_SERVIDOR = stoi(value);
    else if (strcmp(name, "MODO_SERVIDOR") == 0)
        config->MODO_SERVIDOR = value;
    else if (strcmp(name, "PARAMETROS_EXTRA") == 0)
        config->PARAMETROS_EXTRA = value;
    else if (strcmp(name, "API_URL") == 0)
        config->API_URL = value;
    else if (strcmp(name, "API_ENDPOINT") == 0)
        config->API_ENDPOINT = value;
    else if (strcmp(name, "TIEMPO_ESPERA_SERVIDOR") == 0)
        config->TIEMPO_ESPERA_SERVIDOR = stoi(value);
    else if (strcmp(name, "LOG_FILE") == 0)
        config->LOG_FILE = value;
    else if (strcmp(name, "MEMORY_FILE") == 0)
        config->MEMORY_FILE = value;
    else if (strcmp(name, "START_FILE") == 0)
        config->START_FILE = value;
    else if (strcmp(name, "PIPER_EXE") == 0)
        config->PIPER_EXE = value;
    else if (strcmp(name, "PIPER_MODEL") == 0)
        config->PIPER_MODEL = value;
    else if (strcmp(name, "VLC_EXE") == 0)
        config->VLC_EXE = value;
    else if (strcmp(name, "VLC_EXTRA_PARAMS") == 0)
        config->VLC_EXTRA_PARAMS = value;
    else if (strcmp(name, "DEBUG") == 0)
        config->DEBUG = stoi(value);
    return 1;
}

int main()
{
    static Config config;

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // Leer el archivo INI
    if (ini_parse("config.ini", handler, &config) < 0)
    {
        cerr << "Error al leer el archivo INI" << endl;
        return 1;
    }

    // Capturar Ctrl + C
    SetConsoleCtrlHandler(signalHandler, TRUE);

    if (config.HABILITAR_SERVIDOR == 1)
    {
        // Iniciar el servidor en un hilo separado
        servidorThread = thread(ejecutarServidor, ref(config));
        // Esperar un tiempo para que el servidor inicie
        this_thread::sleep_for(chrono::seconds(config.TIEMPO_ESPERA_SERVIDOR));
    }
    else
    {
        delayPrint("ADVERTENCIA! modelo no iniciado!", 10);
    }

    delayPrint("Escribe 'salir' para terminar el programa, o 'menu' para más opciones.\n\n", 10);

    string prompt = cargarMemoria(config);
    string newPrompt;
    bool firstTime = true;
    bool mainLoop = true;

    while (mainLoop)
    {
        if (!firstTime)
        {
            cout << "USUARIO: ";
            getline(cin, newPrompt);

            switch (getHash(newPrompt.c_str()))
            {
            case getHash("listen"):
                // newPrompt = listen();
                continue;
            case getHash("salir"):
                guardarMemoria("USUARIO: Hola harry, como has estado en mi ausencia?", config);
                mainLoop = false;
                continue;
            }

            prompt += "USUARIO: " + newPrompt;
            guardarMemoria("USUARIO: " + newPrompt, config);
        }
        firstTime = false;

        if (mainLoop)
        {
            string retornoJson = interactuarConModelo(prompt, config);
            string retorno = procesarJSON(retornoJson);

            retorno = limpiarRespuestaIA(retorno, false);

            guardarMemoria("HARRY: " + retorno + "\n", config);
            prompt += "HARRY: " + retorno + "\n";
            speak(config, retorno);
            delayPrint("\nHARRY: " + retorno + "\n", 10);
        }
    }
    // Asegurar que el servidor se cierre correctamente
    if (config.HABILITAR_SERVIDOR == 1)
    {
        if (servidorThread.joinable())
        {
            servidorThread.join();
        }
        detenerServidor();
    }
    return 0;
}