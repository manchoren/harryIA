#include "portaudio.h" 
#include "whisper.h"
#include <iostream>
#include <vector>
#include <string>
#include <windows.h>  // Para Sleep()
#include <cmath>      // Para analizar el nivel de audio

#define SAMPLE_RATE 16000
#define FRAMES_PER_BUFFER 512
#define SILENCE_THRESHOLD 0.01f  // Umbral de silencio
#define SILENCE_DURATION 1.0      // Tiempo en segundos para detectar pausa

static whisper_context *ctx;
static std::vector<float> audio_data;
static double silence_time = 0.0;

// Callback de PortAudio para capturar audio
static int recordCallback(const void *input, void *output, unsigned long frameCount,
                          const PaStreamCallbackTimeInfo *timeInfo, PaStreamCallbackFlags statusFlags, void *userData) {
    float *in = (float *)input;
    if (!in) return paContinue;

    bool has_voice = false;
    for (unsigned long i = 0; i < frameCount; i++) {
        if (std::abs(in[i]) > SILENCE_THRESHOLD) {
            has_voice = true;
            break;
        }
    }

    if (has_voice) {
        silence_time = 0.0;
        audio_data.insert(audio_data.end(), in, in + frameCount);
    } else {
        silence_time += (double)frameCount / SAMPLE_RATE;
    }

    return (silence_time >= SILENCE_DURATION) ? paComplete : paContinue;
}

// Inicializa Whisper con el modelo especificado
void initWhisper(const std::string &model_path) {
    ctx = whisper_init_from_file(model_path.c_str());
    if (!ctx) {
        std::cerr << "Error cargando el modelo de Whisper.\n";
        exit(1);
    }
}

// Transcribe el audio capturado con Whisper
std::string transcribeAudio() {
    if (audio_data.empty()) return "";

    whisper_full_params params = whisper_full_default_params(WHISPER_SAMPLING_GREEDY);
    params.print_progress = false;
    params.print_timestamps = false;
    params.language = "es";

    if (whisper_full(ctx, params, audio_data.data(), audio_data.size()) != 0) {
        std::cerr << "Error en la transcripción.\n";
        return "";
    }

    std::string transcript;
    int num_segments = whisper_full_n_segments(ctx);
    for (int i = 0; i < num_segments; i++) {
        transcript += whisper_full_get_segment_text(ctx, i);
    }

    return transcript;
}

// Captura audio del micrófono hasta detectar una pausa
std::string listen() {
    audio_data.clear();
    silence_time = 0.0;
    PaStream *stream;
    
    if (Pa_OpenDefaultStream(&stream, 1, 0, paFloat32, SAMPLE_RATE, FRAMES_PER_BUFFER, recordCallback, nullptr) != paNoError) {
        std::cerr << "Error abriendo el stream de audio.\n";
        return "";
    }

    Pa_StartStream(stream);
    while (Pa_IsStreamActive(stream) == 1) {
        Pa_Sleep(100);
    }
    Pa_StopStream(stream);
    Pa_CloseStream(stream);

    return transcribeAudio();
}

// Liberar memoria cuando se termine el uso de Whisper
void cleanupWhisper() {
    whisper_free(ctx);
}
