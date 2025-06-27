#ifndef LISTEN_H
#define LISTEN_H

#include <string>

// Inicializa Whisper con el modelo especificado
void initWhisper(const std::string &model_path);

// Captura audio del micrófono hasta detectar una pausa y devuelve el texto transcrito
std::string listen();

// Libera la memoria usada por Whisper
void cleanupWhisper();

#endif // LISTEN_H