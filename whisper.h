#ifndef WHISPER_H
#define WHISPER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct whisper_context whisper_context;

whisper_context *whisper_init_from_file(const char *path);
void whisper_free(whisper_context *ctx);

typedef struct {
    int print_progress;
    int print_timestamps;
    const char *language;
} whisper_full_params;

whisper_full_params whisper_full_default_params(int strategy);
int whisper_full(whisper_context *ctx, whisper_full_params params, const float *samples, int n_samples);
int whisper_full_n_segments(whisper_context *ctx);
const char *whisper_full_get_segment_text(whisper_context *ctx, int i);

// 🔥 Agrega esta línea para definir WHISPER_SAMPLING_GREEDY
#define WHISPER_SAMPLING_GREEDY 0  // Asegúrate de que el valor sea correcto según Whisper

#ifdef __cplusplus
}
#endif

#endif  // WHISPER_H
