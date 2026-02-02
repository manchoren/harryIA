"""
Main entry point for HarryIA Python version.
"""

import signal
import sys
import threading
import time
import urllib.request
import urllib.error
from config import Config
from utils import delay_print, cargar_memoria, guardar_memoria, interactuar_con_modelo, procesar_json, limpiar_respuesta_ia
from speak import speak
from listen import listen
from server import ejecutar_servidor, detener_servidor

running = True

def signal_handler(sig, frame):
    """
    Handles Ctrl+C to shutdown gracefully.
    """
    global running
    print("\nCerrando el servidor...")
    running = False
    detener_servidor()
    sys.exit(0)

def main():
    config = Config()

    signal.signal(signal.SIGINT, signal_handler)

    if config.HABILITAR_SERVIDOR:
        # Espera activa hasta que el servidor responda o hasta agotar el tiempo de espera
        try:
            delay_print(f"Esperando hasta {config.TIEMPO_ESPERA_SERVIDOR} segundos a que el servidor arranque...", 10)
            server_thread = threading.Thread(target=ejecutar_servidor, args=(config,))
            server_thread.start()
        except Exception:
            # En caso de que delay_print falle por cualquier motivo, continuar con la espera normal
            pass

        host = config.API_URL.rstrip('/')
        endpoint = config.API_ENDPOINT if config.API_ENDPOINT.startswith('/') else '/' + config.API_ENDPOINT
        health_url = host + endpoint

        started = False
        start_time = time.time()
        while time.time() - start_time < config.TIEMPO_ESPERA_SERVIDOR:
            try:
                resp = urllib.request.urlopen(health_url, timeout=2)
                code = getattr(resp, 'getcode', lambda: None)()
                if code is None or (200 <= code < 400):
                    started = True
                    break
            except urllib.error.HTTPError as e:
                # Si hay respuesta HTTP pero no 2xx, consideramos que el servidor está arriba
                if 400 > e.code >= 200:
                    started = True
                    break
            except Exception:
                pass
            time.sleep(0.5)

        if started:
            delay_print("Servidor levantado correctamente.", 10)
        else:
            delay_print(f"Advertencia: el servidor no respondió en {config.TIEMPO_ESPERA_SERVIDOR} segundos.", 10)
    else:
        delay_print("ADVERTENCIA! modelo no iniciado!", 10)

    delay_print("Escribe 'salir' para terminar el programa.\n\n", 10)

    prompt = cargar_memoria(config)
    first_time = True
    main_loop = True

    while main_loop and running:
        if not first_time:
            user_input = input("USUARIO: ")
            if user_input.lower() == 'salir':
                guardar_memoria("USUARIO: Hola harry, como has estado en mi ausencia?", config)
                main_loop = False
                continue
            elif user_input.lower() == 'listen':
                user_input = listen()
                if not user_input:
                    continue
            prompt += "USUARIO: " + user_input + "\n"
            guardar_memoria("USUARIO: " + user_input, config)
        first_time = False

        if main_loop:
            response_json = interactuar_con_modelo(prompt, config)
            response = procesar_json(response_json)
            response = limpiar_respuesta_ia(response, False)

            guardar_memoria("HARRY: " + response + "\n", config)
            prompt += "HARRY: " + response + "\n"
            speak(response)
            delay_print("\nHARRY: " + response + "\n", 10)

    if config.HABILITAR_SERVIDOR:
        detener_servidor()

if __name__ == "__main__":
    main()