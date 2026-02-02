"""
Configuration module for HarryIA Python version.
Loads settings from config.ini file.
"""

import configparser
import os

class Config:
    def __init__(self, config_file='config.ini'):
        self.config = configparser.ConfigParser()
        config_path = os.path.join(os.path.dirname(__file__), '..', config_file)
        self.config.read(config_path)

        # General config
        self.EJECUTABLE = self.config.get('config', 'EJECUTABLE', fallback='llamafile-0.9.3.exe')
        self.HABILITAR_SERVIDOR = self.config.getint('config', 'HABILITAR_SERVIDOR', fallback=1)
        self.MODO_SERVIDOR = self.config.get('config', 'MODO_SERVIDOR', fallback='--server')
        self.PARAMETROS_EXTRA = self.config.get('config', 'PARAMETROS_EXTRA', fallback='--ctx-size 2048 --nobrowser -m unsloth_Q4_K_M.gguf')
        self.API_URL = self.config.get('config', 'API_URL', fallback='http://127.0.0.1:8080')
        self.API_ENDPOINT = self.config.get('config', 'API_ENDPOINT', fallback='/completion')
        self.TIEMPO_ESPERA_SERVIDOR = self.config.getint('config', 'TIEMPO_ESPERA_SERVIDOR', fallback=60)
        self.LOG_FILE = self.config.get('config', 'LOG_FILE', fallback='server.log')
        self.MEMORY_FILE = self.config.get('config', 'MEMORY_FILE', fallback='memory.txt')
        self.START_FILE = self.config.get('config', 'START_FILE', fallback='start.txt')
        self.DEBUG = self.config.getint('config', 'DEBUG', fallback=1)

        # Sound config
        self.PIPER_EXE = self.config.get('sound', 'PIPER_EXE', fallback=r'.\piper\piper.exe')
        self.PIPER_MODEL = self.config.get('sound', 'PIPER_MODEL', fallback=r'.\piper\es_MX-ald-medium.onnx')
        self.VLC_EXE = self.config.get('sound', 'VLC_EXE', fallback=r'C:\Program Files\VideoLAN\VLC\vlc.exe')
        self.VLC_EXTRA_PARAMS = self.config.get('sound', 'VLC_EXTRA_PARAMS', fallback='--intf dummy --demux=rawaud --rawaud-channels 2 --rawaud-samplerate 12000 --play-and-exit -')