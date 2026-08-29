/*
 * ==============================================================================
 * SampleDeck - TUI Audio Waveform Inspector & Hardstyle Kick Analyzer
 * Vim-Style Navigation (hjkl) • Sub-pixel Braille • Zoom • Loop Playback
 * ==============================================================================
 */

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <math.h>
#include <pthread.h>
#include <sndfile.h>
#include <pulse/simple.h>
#include <pulse/error.h>

#define PI 3.14159265358979323846f

// Color Themes
typedef struct {
    const char *name;
    uint8_t bg_r, bg_g, bg_b;
    uint8_t wave_r, wave_g, wave_b;
    uint8_t peak_r, peak_g, peak_b;
    uint8_t cursor_r, cursor_g, cursor_b;
    uint8_t hud_r, hud_g, hud_b;
} ColorTheme;

static const ColorTheme THEMES[] = {
    // 0: Defqon.1 Flame (Warrior Red & Amber)
    { "Defqon.1 Flame",  20, 22, 28,   243, 141, 65,  224, 85, 97,   255, 255, 255,  86, 182, 194 },
    // 1: Cyberpunk Neon
    { "Cyberpunk Neon",  24, 18, 42,   86, 182, 194,  211, 54, 130,  255, 255, 80,   180, 140, 255 },
    // 2: Studio Amber
    { "Studio Amber",    22, 20, 16,   230, 160, 50,  255, 200, 80,  255, 255, 255,  180, 180, 180 },
    // 3: Ice Cyan
    { "Ice Cyan",        14, 24, 32,   72, 202, 228,  202, 240, 248, 255, 100, 100,  80, 220, 160 },
    // 4: Industrial Gray
    { "Industrial Gray", 26, 28, 34,   171, 178, 191, 228, 229, 232, 255, 85, 95,    140, 145, 155 }
};
#define NUM_THEMES (sizeof(THEMES) / sizeof(THEMES[0]))

static const char *NOTE_NAMES[] = {
    "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B"
};

typedef struct {
    char filename[512];
    float *samples;         // Mono downmixed normalized float samples [-1.0, 1.0]
    int64_t total_samples;
    int sample_rate;
    int channels;
    double duration_ms;

    // Analysis Metrics
    float peak_amp;
    float peak_db;
    float rms_amp;
    float rms_db;
    float dc_offset;
    float tok_time_ms;      // Attack peak time
    float punch_freq_hz;    // Transient / Punch fundamental frequency
    float tail_freq_hz;     // Sub-bass tail fundamental frequency
    char tail_note[32];     // e.g. "G1", "F#1"
    int zero_crossings;
} AudioSample;

typedef struct {
    int64_t view_start;
    int64_t view_len;
    double zoom;            // 1.0 = full sample, 100.0 = ultra zoom
    int64_t sel_start;
    int64_t sel_end;

    volatile bool is_playing;
    volatile int64_t play_pos;
    volatile bool loop_mode;
    pthread_t play_thread;
    pthread_mutex_t play_mutex;

    int theme_idx;
    bool filled_mode;
} ViewerState;

static volatile bool g_running = true;
static volatile bool g_resized = true;
static struct termios g_orig_termios;
static int g_term_cols = 80;
static int g_term_rows = 24;

static void restore_terminal(void) {
    printf("\033[?25h\033[0m\033[?1049l");
    fflush(stdout);
    tcsetattr(STDIN_FILENO, TCSANOW, &g_orig_termios);
}

static void sig_handler(int sig) {
    if (sig == SIGINT || sig == SIGTERM) {
        g_running = false;
    } else if (sig == SIGWINCH) {
        g_resized = true;
    }
}

static void setup_terminal(void) {
    tcgetattr(STDIN_FILENO, &g_orig_termios);
    atexit(restore_terminal);

    struct termios raw = g_orig_termios;
    raw.c_lflag &= ~(ECHO | ICANON);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);

    printf("\033[?1049h\033[?25l\033[2J\033[H");
    fflush(stdout);
}

static void update_terminal_size(void) {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0) {
        g_term_cols = ws.ws_col > 20 ? ws.ws_col : 80;
        g_term_rows = ws.ws_row > 8 ? ws.ws_row : 24;
    }
    g_resized = false;
}

static void hz_to_note_str(float hz, char *out, size_t out_sz) {
    if (hz < 15.0f || hz > 20000.0f) {
        snprintf(out, out_sz, "N/A");
        return;
    }
    float midi = 69.0f + 12.0f * (logf(hz / 440.0f) / logf(2.0f));
    int midi_round = (int)roundf(midi);
    int note_idx = (midi_round % 12 + 12) % 12;
    int octave = (midi_round / 12) - 1;

    float exact_hz = 440.0f * powf(2.0f, (float)(midi_round - 69) / 12.0f);
    float cents = 1200.0f * (logf(hz / exact_hz) / logf(2.0f));

    if (fabsf(cents) < 1.0f) {
        snprintf(out, out_sz, "%s%d (%.1fHz)", NOTE_NAMES[note_idx], octave, hz);
    } else {
        snprintf(out, out_sz, "%s%d %+.0fc (%.1fHz)", NOTE_NAMES[note_idx], octave, cents, hz);
    }
}

static bool load_audio_file(const char *path, AudioSample *sample) {
    memset(sample, 0, sizeof(AudioSample));
    strncpy(sample->filename, path, sizeof(sample->filename) - 1);

    SF_INFO sf_info;
    memset(&sf_info, 0, sizeof(sf_info));
    SNDFILE *snd_file = sf_open(path, SFM_READ, &sf_info);

    if (snd_file) {
        sample->sample_rate = sf_info.samplerate;
        sample->channels = sf_info.channels;
        sample->total_samples = sf_info.frames;

        float *raw_buf = (float *)malloc(sizeof(float) * sf_info.frames * sf_info.channels);
        if (!raw_buf) {
            sf_close(snd_file);
            return false;
        }

        sf_readf_float(snd_file, raw_buf, sf_info.frames);
        sf_close(snd_file);

        sample->samples = (float *)malloc(sizeof(float) * sf_info.frames);
        for (int64_t i = 0; i < sf_info.frames; i++) {
            float sum = 0.0f;
            for (int c = 0; c < sf_info.channels; c++) {
                sum += raw_buf[i * sf_info.channels + c];
            }
            sample->samples[i] = sum / (float)sf_info.channels;
        }
        free(raw_buf);
    } else {
        char cmd[1024];
        snprintf(cmd, sizeof(cmd), "ffmpeg -v error -i \"%s\" -f f32le -ac 1 -ar 44100 - 2>/dev/null", path);
        FILE *fp = popen(cmd, "r");
        if (!fp) return false;

        size_t cap = 44100 * 10;
        sample->samples = (float *)malloc(sizeof(float) * cap);
        sample->sample_rate = 44100;
        sample->channels = 1;
        sample->total_samples = 0;

        float chunk[1024];
        size_t n;
        while ((n = fread(chunk, sizeof(float), 1024, fp)) > 0) {
            if (sample->total_samples + (int64_t)n >= (int64_t)cap) {
                cap *= 2;
                sample->samples = (float *)realloc(sample->samples, sizeof(float) * cap);
            }
            memcpy(sample->samples + sample->total_samples, chunk, sizeof(float) * n);
            sample->total_samples += n;
        }
        pclose(fp);

        if (sample->total_samples <= 0) {
            if (sample->samples) free(sample->samples);
            return false;
        }
    }

    sample->duration_ms = ((double)sample->total_samples / (double)sample->sample_rate) * 1000.0;

    double sum_sq = 0.0;
    double sum_dc = 0.0;
    float max_peak = 0.0f;
    int64_t max_peak_idx = 0;
    int zc_count = 0;

    for (int64_t i = 0; i < sample->total_samples; i++) {
        float s = sample->samples[i];
        float abs_s = fabsf(s);
        if (abs_s > max_peak) {
            max_peak = abs_s;
            max_peak_idx = i;
        }
        sum_sq += (double)(s * s);
        sum_dc += (double)s;

        if (i > 0) {
            if ((sample->samples[i - 1] <= 0.0f && s > 0.0f) ||
                (sample->samples[i - 1] >= 0.0f && s < 0.0f)) {
                zc_count++;
            }
        }
    }

    sample->peak_amp = max_peak;
    sample->peak_db = (max_peak > 0.00001f) ? (20.0f * log10f(max_peak)) : -96.0f;
    sample->rms_amp = (float)sqrt(sum_sq / (double)sample->total_samples);
    sample->rms_db = (sample->rms_amp > 0.00001f) ? (20.0f * log10f(sample->rms_amp)) : -96.0f;
    sample->dc_offset = (float)(sum_dc / (double)sample->total_samples) * 100.0f;
    sample->zero_crossings = zc_count;
    sample->tok_time_ms = ((float)max_peak_idx / (float)sample->sample_rate) * 1000.0f;

    // Hardstyle Kick Transient & Tail Pitch Tracking
    int64_t punch_start = (int64_t)(sample->sample_rate * 0.010);
    int64_t punch_end = (int64_t)(sample->sample_rate * 0.060);
    if (punch_end > sample->total_samples) punch_end = sample->total_samples;

    int punch_zc = 0;
    for (int64_t i = punch_start; i < punch_end; i++) {
        if (i > 0 && sample->samples[i - 1] <= 0.0f && sample->samples[i] > 0.0f) {
            punch_zc++;
        }
    }
    double punch_dur_s = (double)(punch_end - punch_start) / (double)sample->sample_rate;
    sample->punch_freq_hz = punch_dur_s > 0.001 ? (float)(punch_zc / punch_dur_s) : 0.0f;

    int64_t tail_start = (int64_t)(sample->sample_rate * 0.080);
    int64_t tail_end = (int64_t)(sample->sample_rate * 0.350);
    if (tail_end > sample->total_samples) tail_end = sample->total_samples;

    int tail_zc = 0;
    for (int64_t i = tail_start; i < tail_end; i++) {
        if (i > 0 && sample->samples[i - 1] <= 0.0f && sample->samples[i] > 0.0f) {
            tail_zc++;
        }
    }
    double tail_dur_s = (double)(tail_end - tail_start) / (double)sample->sample_rate;
    sample->tail_freq_hz = tail_dur_s > 0.01 ? (float)(tail_zc / tail_dur_s) : 0.0f;
    hz_to_note_str(sample->tail_freq_hz, sample->tail_note, sizeof(sample->tail_note));

    return true;
}

typedef struct {
    AudioSample *sample;
    ViewerState *viewer;
} PlaybackContext;

static void *playback_thread_func(void *arg) {
    PlaybackContext *ctx = (PlaybackContext *)arg;
    AudioSample *s = ctx->sample;
    ViewerState *v = ctx->viewer;

    pa_sample_spec ss;
    ss.format = PA_SAMPLE_S16LE;
    ss.rate = s->sample_rate;
    ss.channels = 1;

    int error;
    pa_simple *pa = pa_simple_new(NULL, "SampleDeck", PA_STREAM_PLAYBACK, NULL, "Sample Playback", &ss, NULL, NULL, &error);
    if (!pa) {
        v->is_playing = false;
        free(ctx);
        return NULL;
    }

    while (v->is_playing && g_running) {
        int64_t start_idx = v->sel_start >= 0 ? v->sel_start : v->view_start;
        int64_t end_idx = v->sel_end > start_idx ? v->sel_end : (v->view_start + v->view_len);
        if (end_idx > s->total_samples) end_idx = s->total_samples;

        v->play_pos = start_idx;

        int16_t pcm_buf[1024];
        for (int64_t pos = start_idx; pos < end_idx && v->is_playing && g_running; pos += 1024) {
            int chunk_sz = (int)((end_idx - pos) < 1024 ? (end_idx - pos) : 1024);
            for (int k = 0; k < chunk_sz; k++) {
                float val = s->samples[pos + k];
                if (val > 1.0f) val = 1.0f;
                if (val < -1.0f) val = -1.0f;
                pcm_buf[k] = (int16_t)(val * 32767.0f);
            }

            v->play_pos = pos;
            if (pa_simple_write(pa, pcm_buf, chunk_sz * sizeof(int16_t), &error) < 0) {
                break;
            }
        }

        if (!v->loop_mode) {
            break;
        }
    }

    pa_simple_drain(pa, &error);
    pa_simple_free(pa);
    v->is_playing = false;
    free(ctx);
    return NULL;
}

static void trigger_playback(AudioSample *s, ViewerState *v) {
    if (v->is_playing) {
        v->is_playing = false;
        pthread_join(v->play_thread, NULL);
    }
    v->is_playing = true;
    PlaybackContext *ctx = (PlaybackContext *)malloc(sizeof(PlaybackContext));
    ctx->sample = s;
    ctx->viewer = v;
    pthread_create(&v->play_thread, NULL, playback_thread_func, ctx);
}

static inline uint8_t get_braille_bit(int sub_x, int sub_y) {
    if (sub_x == 0) {
        if (sub_y == 0) return 0x01;
        if (sub_y == 1) return 0x02;
        if (sub_y == 2) return 0x04;
        if (sub_y == 3) return 0x40;
    } else {
        if (sub_y == 0) return 0x08;
        if (sub_y == 1) return 0x10;
        if (sub_y == 2) return 0x20;
        if (sub_y == 3) return 0x80;
    }
    return 0;
}

static void draw_dot(uint8_t *grid, int cols, int rows, int x, int y) {
    if (x < 0 || x >= cols * 2 || y < 0 || y >= rows * 4) return;
    int cx = x / 2;
    int cy = y / 4;
    grid[cy * cols + cx] |= get_braille_bit(x % 2, y % 4);
}

static void draw_line(uint8_t *grid, int cols, int rows, int x0, int y0, int x1, int y1) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2;
    int e2;

    while (1) {
        draw_dot(grid, cols, rows, x0, y0);
        if (x0 == x1 && y0 == y1) break;
        e2 = err;
        if (e2 > -dx) { err -= dy; x0 += sx; }
        if (e2 < dy) { err += dx; y0 += sy; }
    }
}

static void render_sampledeck(const AudioSample *s, const ViewerState *v) {
    const ColorTheme *theme = &THEMES[v->theme_idx];

    // Header Row 0 & 1: Metadata & Kick Analysis
    printf("\033[H\033[38;2;%d;%d;%dm", theme->hud_r, theme->hud_g, theme->hud_b);
    printf(" ⚡ SAMPLEDECK // %s  [ %d Hz | %s | %.2f ms | %ld samples ]\033[K\n",
           s->filename, s->sample_rate, s->channels == 2 ? "Stereo" : "Mono", s->duration_ms, s->total_samples);

    printf("\033[38;2;%d;%d;%dm", theme->peak_r, theme->peak_g, theme->peak_b);
    printf(" 📊 Peak: %+.1fdBFS (%.0f%%) | RMS: %+.1fdBFS | DC: %+.2f%% | 🎯 Tok: %.1fms | 🥊 Punch: %.0fHz | 🔊 Sub/Tail: %s\033[K\n",
           s->peak_db, s->peak_amp * 100.0f, s->rms_db, s->dc_offset, s->tok_time_ms, s->punch_freq_hz, s->tail_note);

    // Section 1: Minimap (2 rows)
    int minimap_rows = 2;
    int minimap_cols = g_term_cols;
    int minimap_vx = minimap_cols * 2;
    int minimap_vy = minimap_rows * 4;

    uint8_t *minimap_grid = (uint8_t *)calloc(minimap_cols * minimap_rows, sizeof(uint8_t));
    if (minimap_grid) {
        int prev_mx = 0, prev_my = minimap_vy / 2;
        for (int mx = 0; mx < minimap_vx; mx++) {
            int64_t s_idx = (mx * s->total_samples) / minimap_vx;
            float val = s->samples[s_idx];
            int my = (int)(((1.0f - val) * 0.5f) * (float)(minimap_vy - 1));
            if (my < 0) my = 0;
            if (my >= minimap_vy) my = minimap_vy - 1;

            if (mx == 0) draw_dot(minimap_grid, minimap_cols, minimap_rows, mx, my);
            else draw_line(minimap_grid, minimap_cols, minimap_rows, prev_mx, prev_my, mx, my);
            prev_mx = mx;
            prev_my = my;
        }

        int view_box_start = (int)((v->view_start * minimap_cols) / s->total_samples);
        int view_box_end = (int)(((v->view_start + v->view_len) * minimap_cols) / s->total_samples);
        if (view_box_end <= view_box_start) view_box_end = view_box_start + 1;

        for (int r = 0; r < minimap_rows; r++) {
            printf("\033[38;2;%d;%d;%dm", theme->wave_r / 2, theme->wave_g / 2, theme->wave_b / 2);
            for (int c = 0; c < minimap_cols; c++) {
                uint8_t mask = minimap_grid[r * minimap_cols + c];
                bool in_view = (c >= view_box_start && c <= view_box_end);

                if (in_view) {
                    printf("\033[38;2;%d;%d;%dm", theme->cursor_r, theme->cursor_g, theme->cursor_b);
                } else {
                    printf("\033[38;2;%d;%d;%dm", theme->wave_r / 2, theme->wave_g / 2, theme->wave_b / 2);
                }

                if (mask == 0) {
                    putchar(in_view && r == 0 ? '-' : ' ');
                } else {
                    uint32_t codepoint = 0x2800 | mask;
                    putchar(0xE0 | ((codepoint >> 12) & 0x0F));
                    putchar(0x80 | ((codepoint >> 6) & 0x3F));
                    putchar(0x80 | (codepoint & 0x3F));
                }
            }
            printf("\033[K\n");
        }
        free(minimap_grid);
    }

    // Section 2: Main Zoomed Waveform Canvas
    int main_rows = g_term_rows - 7;
    if (main_rows < 4) main_rows = 4;
    int main_cols = g_term_cols;
    int main_vx = main_cols * 2;
    int main_vy = main_rows * 4;

    uint8_t *main_grid = (uint8_t *)calloc(main_cols * main_rows, sizeof(uint8_t));
    if (main_grid) {
        int prev_x = 0, prev_y = main_vy / 2;

        for (int vx = 0; vx < main_vx; vx++) {
            int64_t s_idx = v->view_start + (vx * v->view_len) / main_vx;
            if (s_idx >= s->total_samples) s_idx = s->total_samples - 1;
            if (s_idx < 0) s_idx = 0;

            float val = s->samples[s_idx];
            int vy = (int)(((1.0f - val) * 0.5f) * (float)(main_vy - 1));
            if (vy < 0) vy = 0;
            if (vy >= main_vy) vy = main_vy - 1;

            if (v->filled_mode) {
                int center_y = main_vy / 2;
                draw_line(main_grid, main_cols, main_rows, vx, center_y, vx, vy);
            } else {
                if (vx == 0) draw_dot(main_grid, main_cols, main_rows, vx, vy);
                else draw_line(main_grid, main_cols, main_rows, prev_x, prev_y, vx, vy);
            }
            prev_x = vx;
            prev_y = vy;
        }

        // Draw Center Zero-dB Axis
        int center_vy = main_vy / 2;
        for (int vx = 0; vx < main_vx; vx += 6) {
            draw_dot(main_grid, main_cols, main_rows, vx, center_vy);
        }

        // Playback cursor column
        int cursor_col = -1;
        if (v->is_playing && v->play_pos >= v->view_start && v->play_pos < (v->view_start + v->view_len)) {
            cursor_col = (int)(((v->play_pos - v->view_start) * main_cols) / v->view_len);
        }

        for (int r = 0; r < main_rows; r++) {
            float dist = fabsf((float)r - (float)main_rows / 2.0f) / ((float)main_rows / 2.0f);
            uint8_t cr = (uint8_t)(theme->wave_r * (1.0f - dist) + theme->peak_r * dist);
            uint8_t cg = (uint8_t)(theme->wave_g * (1.0f - dist) + theme->peak_g * dist);
            uint8_t cb = (uint8_t)(theme->wave_b * (1.0f - dist) + theme->peak_b * dist);

            printf("\033[38;2;%d;%d;%dm", cr, cg, cb);

            for (int c = 0; c < main_cols; c++) {
                if (c == cursor_col) {
                    printf("\033[38;2;%d;%d;%dm|\033[38;2;%d;%d;%dm", theme->cursor_r, theme->cursor_g, theme->cursor_b, cr, cg, cb);
                    continue;
                }

                uint8_t mask = main_grid[r * main_cols + c];
                if (mask == 0) {
                    putchar(' ');
                } else {
                    uint32_t codepoint = 0x2800 | mask;
                    putchar(0xE0 | ((codepoint >> 12) & 0x0F));
                    putchar(0x80 | ((codepoint >> 6) & 0x3F));
                    putchar(0x80 | (codepoint & 0x3F));
                }
            }
            printf("\033[K\n");
        }
        free(main_grid);
    }

    // Section 3: Time Axis Marker Bar
    double view_start_ms = ((double)v->view_start / (double)s->sample_rate) * 1000.0;
    double view_end_ms = ((double)(v->view_start + v->view_len) / (double)s->sample_rate) * 1000.0;
    double view_mid_ms = (view_start_ms + view_end_ms) / 2.0;

    printf("\033[38;2;%d;%d;%dm", theme->hud_r, theme->hud_g, theme->hud_b);
    printf(" ├─ %.2f ms %*s %.2f ms %*s %.2f ms ─┤\033[K\n",
           view_start_ms,
           g_term_cols / 2 - 18, "┼",
           view_mid_ms,
           g_term_cols / 2 - 18, "┤",
           view_end_ms);

    // Section 4: Vim-Style Navigation Footer
    printf("\033[38;2;%d;%d;%dm", theme->hud_r, theme->hud_g, theme->hud_b);
    printf(" [Space] %s  [o] Loop: %s  [h/l] Pan  [k/j] Zoom: %.1fx  [0/$] Start/End  [m] Modo  [c] Tema  [q] Salir\033[K",
           v->is_playing ? "PAUSE" : "PLAY",
           v->loop_mode ? "ON" : "OFF",
           v->zoom);

    fflush(stdout);
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <archivo_de_audio.wav|flac|mp3|ogg>\n", argv[0]);
        return 1;
    }

    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);
    signal(SIGWINCH, sig_handler);

    AudioSample sample;
    if (!load_audio_file(argv[1], &sample)) {
        fprintf(stderr, "Error: No se pudo cargar el archivo de audio '%s'.\n", argv[1]);
        return 1;
    }

    setup_terminal();
    update_terminal_size();

    ViewerState viewer;
    memset(&viewer, 0, sizeof(viewer));
    viewer.zoom = 1.0;
    viewer.view_start = 0;
    viewer.view_len = sample.total_samples;
    viewer.sel_start = -1;
    viewer.sel_end = -1;
    viewer.theme_idx = 0;
    viewer.filled_mode = false;
    viewer.loop_mode = false;
    pthread_mutex_init(&viewer.play_mutex, NULL);

    while (g_running) {
        char ch;
        while (read(STDIN_FILENO, &ch, 1) > 0) {
            if (ch == 'q' || ch == 'Q') {
                g_running = false;
                break;
            } else if (ch == 27) { // Esc or Escape sequence
                char next1, next2;
                if (read(STDIN_FILENO, &next1, 1) > 0 && next1 == '[') {
                    if (read(STDIN_FILENO, &next2, 1) > 0) {
                        if (next2 == 'D') ch = 'h'; // Left arrow -> h
                        else if (next2 == 'C') ch = 'l'; // Right arrow -> l
                        else if (next2 == 'A') ch = 'k'; // Up arrow -> k (Zoom in)
                        else if (next2 == 'B') ch = 'j'; // Down arrow -> j (Zoom out)
                        else if (next2 == 'H') ch = '0'; // Home -> 0
                        else if (next2 == 'F') ch = '$'; // End -> $
                    }
                } else {
                    g_running = false;
                    break;
                }
            }

            // --- Vim Movements (h / l / H / L / 0 / $ / w / b) ---
            if (ch == 'h') {
                // Pan Left (10% viewport)
                int64_t step = viewer.view_len / 10;
                if (step < 1) step = 1;
                viewer.view_start -= step;
                if (viewer.view_start < 0) viewer.view_start = 0;
            } else if (ch == 'l') {
                // Pan Right (10% viewport)
                int64_t step = viewer.view_len / 10;
                if (step < 1) step = 1;
                viewer.view_start += step;
                if (viewer.view_start + viewer.view_len > sample.total_samples) {
                    viewer.view_start = sample.total_samples - viewer.view_len;
                    if (viewer.view_start < 0) viewer.view_start = 0;
                }
            } else if (ch == 'H') {
                // Fast Pan Left (35% viewport)
                int64_t step = (viewer.view_len * 35) / 100;
                viewer.view_start -= step;
                if (viewer.view_start < 0) viewer.view_start = 0;
            } else if (ch == 'L') {
                // Fast Pan Right (35% viewport)
                int64_t step = (viewer.view_len * 35) / 100;
                viewer.view_start += step;
                if (viewer.view_start + viewer.view_len > sample.total_samples) {
                    viewer.view_start = sample.total_samples - viewer.view_len;
                    if (viewer.view_start < 0) viewer.view_start = 0;
                }
            } else if (ch == '0' || ch == '^') {
                // Jump to Start (t = 0.0 ms)
                viewer.view_start = 0;
            } else if (ch == '$') {
                // Jump to End
                viewer.view_start = sample.total_samples - viewer.view_len;
                if (viewer.view_start < 0) viewer.view_start = 0;
            } else if (ch == 'w') {
                // Word forward jump (25% viewport)
                int64_t step = viewer.view_len / 4;
                viewer.view_start += step;
                if (viewer.view_start + viewer.view_len > sample.total_samples) {
                    viewer.view_start = sample.total_samples - viewer.view_len;
                    if (viewer.view_start < 0) viewer.view_start = 0;
                }
            } else if (ch == 'b') {
                // Word backward jump (25% viewport)
                int64_t step = viewer.view_len / 4;
                viewer.view_start -= step;
                if (viewer.view_start < 0) viewer.view_start = 0;
            }

            // --- Vim Zoom (k = in, j = out, z / r = reset) ---
            else if (ch == 'k' || ch == '+' || ch == '=') {
                // Zoom in centered
                if (viewer.zoom < 300.0) {
                    viewer.zoom *= 1.35;
                    int64_t new_len = (int64_t)((double)sample.total_samples / viewer.zoom);
                    if (new_len < 16) new_len = 16;
                    int64_t center = viewer.view_start + viewer.view_len / 2;
                    viewer.view_len = new_len;
                    viewer.view_start = center - new_len / 2;
                    if (viewer.view_start < 0) viewer.view_start = 0;
                    if (viewer.view_start + viewer.view_len > sample.total_samples) {
                        viewer.view_start = sample.total_samples - viewer.view_len;
                        if (viewer.view_start < 0) viewer.view_start = 0;
                    }
                }
            } else if (ch == 'j' || ch == '-' || ch == '_') {
                // Zoom out centered
                if (viewer.zoom > 1.0) {
                    viewer.zoom /= 1.35;
                    if (viewer.zoom < 1.0) viewer.zoom = 1.0;
                    int64_t new_len = (int64_t)((double)sample.total_samples / viewer.zoom);
                    if (new_len > sample.total_samples) new_len = sample.total_samples;
                    int64_t center = viewer.view_start + viewer.view_len / 2;
                    viewer.view_len = new_len;
                    viewer.view_start = center - new_len / 2;
                    if (viewer.view_start < 0) viewer.view_start = 0;
                    if (viewer.view_start + viewer.view_len > sample.total_samples) {
                        viewer.view_start = sample.total_samples - viewer.view_len;
                        if (viewer.view_start < 0) viewer.view_start = 0;
                    }
                }
            } else if (ch == 'z' || ch == 'r' || ch == 'R') {
                // Reset zoom (1.0x full sample)
                viewer.zoom = 1.0;
                viewer.view_start = 0;
                viewer.view_len = sample.total_samples;
            }

            // --- Playback Controls ---
            else if (ch == ' ' || ch == '\n' || ch == '\r') {
                if (viewer.is_playing) {
                    viewer.is_playing = false;
                } else {
                    trigger_playback(&sample, &viewer);
                }
            } else if (ch == 'o' || ch == 'O') {
                // Toggle loop mode
                viewer.loop_mode = !viewer.loop_mode;
            } else if (ch == 's' || ch == 'S') {
                // Stop playback
                viewer.is_playing = false;
            }

            // --- Display & Styling ---
            else if (ch == 'm' || ch == 'M') {
                viewer.filled_mode = !viewer.filled_mode;
            } else if (ch == 'c' || ch == 'C') {
                viewer.theme_idx = (viewer.theme_idx + 1) % NUM_THEMES;
            }
        }

        if (g_resized) {
            update_terminal_size();
            printf("\033[2J");
        }

        render_sampledeck(&sample, &viewer);
        usleep(33333); // ~30 FPS UI refresh
    }

    if (viewer.is_playing) {
        viewer.is_playing = false;
        pthread_join(viewer.play_thread, NULL);
    }

    pthread_mutex_destroy(&viewer.play_mutex);
    if (sample.samples) free(sample.samples);

    return 0;
}
