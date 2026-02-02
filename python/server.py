"""
Server management module for starting and stopping the llamafile server.
"""

import subprocess
import time
import signal
import os

server_process = None

def ejecutar_servidor(config):
    """
    Starts the llamafile server.
    """
    global server_process
    exe_path = os.path.join(os.path.dirname(__file__), '..', config.EJECUTABLE)
    command = [exe_path, config.MODO_SERVIDOR] + config.PARAMETROS_EXTRA.split()
    try:
        server_process = subprocess.Popen(command, stdout=subprocess.PIPE, stderr=subprocess.PIPE)
        print("Servidor iniciado.")
        time.sleep(config.TIEMPO_ESPERA_SERVIDOR)
    except Exception as e:
        print(f"Error al iniciar el servidor: {e}")

def detener_servidor():
    """
    Stops the llamafile server.
    """
    global server_process
    if server_process:
        server_process.terminate()
        try:
            server_process.wait(timeout=10)
        except subprocess.TimeoutExpired:
            server_process.kill()
        print("Servidor detenido.")