# HarryIA Python Version

This is a Python port of the HarryIA project, an AI assistant with voice capabilities.

## Features

- Text-based conversation with AI model
- Voice input using speech recognition
- Voice output using Piper TTS
- Memory persistence
- Modular architecture

## Installation

1. Install Python dependencies:
   ```
   pip install -r requirements.txt
   ```

2. Ensure Piper and VLC are installed and configured in config.ini.

3. Copy config.ini from the parent directory or adjust paths.

## Obtención del archivo Llamafile

Para obtener el archivo Llamafile necesario para el funcionamiento del proyecto, sigue estos pasos:

1. **Visita el repositorio oficial**: Dirígete al [repositorio de Llamafile en GitHub](https://github.com/tu_usuario/llamafile) (reemplaza con la URL correcta).

2. **Descarga el archivo**: Puedes clonar el repositorio usando Git o descargar el archivo directamente como un ZIP.
   - Para clonar el repositorio, usa el siguiente comando:
     ```bash
     git clone https://github.com/tu_usuario/llamafile.git
     ```
   - Si prefieres descargarlo como ZIP, haz clic en el botón "Code" y selecciona "Download ZIP".

3. **Ubicación del archivo**: Asegúrate de colocar el archivo `llamafile` en el directorio correcto, que es el mismo donde se encuentra el script principal `main.py`.

4. **Configuración**: Revisa el archivo `config.ini` para asegurarte de que las rutas y configuraciones sean correctas para tu entorno.

5. **Ejecuta el servidor**: Una vez que tengas el archivo, puedes iniciar el servidor de Llamafile con el siguiente comando (reemplaza `modelo.gguf` con el nombre de tu modelo descargado):

   ```bash
   .\llamafile-0.9.3.exe --server --ctx-size 2048 -m modelo.gguf --n
   ```

   Nota: Los modelos se pueden descargar fácilmente desde [Smiling Face](https://smilingface.co) sin problemas.

Asegúrate de seguir estos pasos para que el proyecto funcione correctamente.

## Usage

Run the main script:
```
python main.py
```

Commands:
- Type text to chat
- Type 'listen' to use voice input
- Type 'salir' to exit

## Modules

- `config.py`: Configuration management
- `utils.py`: Utility functions for AI interaction and text processing
- `listen.py`: Speech recognition
- `speak.py`: Text-to-speech
- `server.py`: Llamafile server management
- `main.py`: Main application loop