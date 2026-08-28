/*
 * ==============================================================================
 * DotWave - Minimalist High-Performance TUI Audio Oscilloscope Visualizer
 * Defqon.1 Weekend Warrior Edition • Sub-pixel Braille Dot Matrix
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
#include <sys/select.h>
#include <math.h>
#include <pulse/simple.h>
#include <pulse/error.h>

#define SAMPLE_RATE 44100
#define BUFFER_SAMPLES 2048
#define DEFAULT_GAIN 1.0f

// Color Themes (RGB 24-bit TrueColor)
typedef struct {
    const char *name;
    uint8_t base_r, base_g, base_b;     // Center/low amplitude
    uint8_t mid_r, mid_g, mid_b;       // Mid amplitude
    uint8_t peak_r, peak_g, peak_b;    // High peaks
} ColorTheme;

static const ColorTheme THEMES[] = {
    // 0: Defqon.1 Flame (Amber to Warrior Red / Fire)
    { "Defqon.1 Flame", 243, 141, 65,   224, 85, 97,    255, 71, 87 },
    // 1: Laser Cyan (Teal to Electric Cyan)
    { "Laser Cyan",     86, 182, 194,   97, 175, 239,   82, 222, 240 },
    // 2: Acid Green (Industrial CRT / Oscilloscope Green)
    { "Acid Green",     142, 189, 107,  152, 195, 121,  180, 255, 100 },
    // 3: Steel Monochrome (Chalk White & Steel)
    { "Steel Chalk",    171, 178, 191,  228, 229, 232,  255, 255, 255 }
};
#define NUM_THEMES (sizeof(THEMES) / sizeof(THEMES[0]))

// Modes
enum WaveMode {
    MODE_LINE = 0,      // Pure continuous oscilloscope line
    MODE_FILLED = 1,    // Filled envelope from center
    MODE_STEREO = 2     // Dual split stereo
};

// Global State
static volatile bool g_running = true;
static volatile bool g_resized = true;
static struct termios g_orig_termios;
static int g_term_cols = 80;
static int g_term_rows = 24;

static int g_theme_idx = 0;
static int g_mode = MODE_LINE;
static float g_gain = DEFAULT_GAIN;
static bool g_trigger_sync = true;
static bool g_paused = false;
static bool g_show_help = false;

// Cleanup terminal state on exit
static void restore_terminal(void) {
    printf("\033[?25h\033[0m\033[2J\033[H");
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

    // Hide cursor & clear
    printf("\033[?25l\033[2J");
    fflush(stdout);
}

static void update_terminal_size(void) {
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == 0) {
        g_term_cols = ws.ws_col > 10 ? ws.ws_col : 80;
        g_term_rows = ws.ws_row > 5 ? ws.ws_row : 24;
    }
    g_resized = false;
}

// Braille Dot Mapping:
// Unicode braille offset = U+2800 + bitmask
// Bit 0: (0,0), Bit 1: (0,1), Bit 2: (0,2), Bit 6: (0,3)
// Bit 3: (1,0), Bit 4: (1,1), Bit 5: (1,2), Bit 7: (1,3)
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

// Draw a line on sub-pixel grid using Bresenham algorithm
static void draw_dot(uint8_t *grid, int v_width, int v_height, int x, int y) {
    if (x < 0 || x >= v_width || y < 0 || y >= v_height) return;
    
    int cell_x = x / 2;
    int cell_y = y / 4;
    int sub_x = x % 2;
    int sub_y = y % 4;

    int cell_cols = v_width / 2;
    int cell_rows = v_height / 4;

    if (cell_x < cell_cols && cell_y < cell_rows) {
        grid[cell_y * cell_cols + cell_x] |= get_braille_bit(sub_x, sub_y);
    }
}

static void draw_line(uint8_t *grid, int v_width, int v_height, int x0, int y0, int x1, int y1) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = x0 < x1 ? 1 : -1;
    int sy = y0 < y1 ? 1 : -1;
    int err = (dx > dy ? dx : -dy) / 2;
    int e2;

    while (1) {
        draw_dot(grid, v_width, v_height, x0, y0);
        if (x0 == x1 && y0 == y1) break;
        e2 = err;
        if (e2 > -dx) { err -= dy; x0 += sx; }
        if (e2 < dy) { err += dx; y0 += sy; }
    }
}

// Zero-crossing trigger: find start index of ascending zero-cross
static int find_trigger_offset(const int16_t *samples, int count) {
    int threshold = 0;
    for (int i = 1; i < count - 200; i++) {
        if (samples[i - 1] <= threshold && samples[i] > threshold) {
            return i;
        }
    }
    return 0;
}

// Process keyboard input
static void handle_input(void) {
    char ch;
    while (read(STDIN_FILENO, &ch, 1) > 0) {
        if (ch == 'q' || ch == 'Q' || ch == 27) { // 27 = Esc
            g_running = false;
        } else if (ch == 'c' || ch == 'C') {
            g_theme_idx = (g_theme_idx + 1) % NUM_THEMES;
        } else if (ch == 'm' || ch == 'M') {
            g_mode = (g_mode + 1) % 3;
        } else if (ch == '+' || ch == '=' || ch == 'k' || ch == 'K') {
            g_gain *= 1.25f;
            if (g_gain > 20.0f) g_gain = 20.0f;
        } else if (ch == '-' || ch == '_' || ch == 'j' || ch == 'J') {
            g_gain *= 0.8f;
            if (g_gain < 0.1f) g_gain = 0.1f;
        } else if (ch == 't' || ch == 'T') {
            g_trigger_sync = !g_trigger_sync;
        } else if (ch == ' ') {
            g_paused = !g_paused;
        } else if (ch == 'h' || ch == 'H') {
            g_show_help = !g_show_help;
        }
    }
}

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    signal(SIGINT, sig_handler);
    signal(SIGTERM, sig_handler);
    signal(SIGWINCH, sig_handler);

    setup_terminal();
    update_terminal_size();

    // PulseAudio setup
    pa_sample_spec ss;
    ss.format = PA_SAMPLE_S16LE;
    ss.rate = SAMPLE_RATE;
    ss.channels = 2; // Stereo

    int error;
    pa_simple *pa_stream = pa_simple_new(NULL, "dotwave", PA_STREAM_RECORD, NULL, "record", &ss, NULL, NULL, &error);
    if (!pa_stream) {
        restore_terminal();
        fprintf(stderr, "Error conectando a PulseAudio/PipeWire: %s\n", pa_strerror(error));
        return 1;
    }

    int16_t audio_buf[BUFFER_SAMPLES * 2];
    int16_t mono_buf[BUFFER_SAMPLES];

    // Output buffer
    char out_buf[128 * 1024];

    while (g_running) {
        handle_input();

        if (g_resized) {
            update_terminal_size();
            printf("\033[2J"); // Clear on resize
        }

        // Read audio if not paused
        if (!g_paused) {
            if (pa_simple_read(pa_stream, audio_buf, sizeof(audio_buf), &error) < 0) {
                usleep(10000);
                continue;
            }

            // Downmix to mono buffer
            for (int i = 0; i < BUFFER_SAMPLES; i++) {
                mono_buf[i] = (int16_t)(((int32_t)audio_buf[i * 2] + (int32_t)audio_buf[i * 2 + 1]) / 2);
            }
        }

        int v_width = g_term_cols * 2;
        int v_height = (g_show_help ? g_term_rows - 1 : g_term_rows) * 4;
        if (v_height < 4) v_height = 4;

        int cell_cols = g_term_cols;
        int cell_rows = v_height / 4;
        size_t grid_size = cell_cols * cell_rows;
        uint8_t *grid = (uint8_t *)calloc(grid_size, sizeof(uint8_t));
        if (!grid) continue;

        int start_offset = 0;
        if (g_trigger_sync) {
            start_offset = find_trigger_offset(mono_buf, BUFFER_SAMPLES);
        }

        int available_samples = BUFFER_SAMPLES - start_offset;
        if (available_samples <= 0) available_samples = BUFFER_SAMPLES;

        int prev_x = 0;
        int prev_y = v_height / 2;

        for (int vx = 0; vx < v_width; vx++) {
            float sample_idx = (float)vx / (float)v_width * (float)(available_samples - 1) + start_offset;
            int idx = (int)sample_idx;
            if (idx >= BUFFER_SAMPLES - 1) idx = BUFFER_SAMPLES - 2;

            float frac = sample_idx - idx;
            float raw_val = (float)mono_buf[idx] * (1.0f - frac) + (float)mono_buf[idx + 1] * frac;

            // Normalized [-1.0, 1.0] with gain
            float norm = (raw_val / 32768.0f) * g_gain;
            if (norm > 1.0f) norm = 1.0f;
            if (norm < -1.0f) norm = -1.0f;

            // Invert Y so positive is up
            int vy = (int)((1.0f - norm) * 0.5f * (v_height - 1));
            if (vy < 0) vy = 0;
            if (vy >= v_height) vy = v_height - 1;

            if (g_mode == MODE_FILLED) {
                int center_y = v_height / 2;
                draw_line(grid, v_width, v_height, vx, center_y, vx, vy);
            } else {
                if (vx == 0) {
                    draw_dot(grid, v_width, v_height, vx, vy);
                } else {
                    draw_line(grid, v_width, v_height, prev_x, prev_y, vx, vy);
                }
            }

            prev_x = vx;
            prev_y = vy;
        }

        // Render to stdout buffer
        size_t out_len = 0;
        out_len += snprintf(out_buf + out_len, sizeof(out_buf) - out_len, "\033[H");

        const ColorTheme *theme = &THEMES[g_theme_idx];

        for (int r = 0; r < cell_rows; r++) {
            // Calculate gradient ratio based on distance from vertical center
            float dist_from_center = fabsf((float)r - (float)cell_rows / 2.0f) / ((float)cell_rows / 2.0f);
            if (dist_from_center > 1.0f) dist_from_center = 1.0f;

            uint8_t cr, cg, cb;
            if (dist_from_center < 0.5f) {
                float t = dist_from_center * 2.0f;
                cr = (uint8_t)(theme->base_r * (1.0f - t) + theme->mid_r * t);
                cg = (uint8_t)(theme->base_g * (1.0f - t) + theme->mid_g * t);
                cb = (uint8_t)(theme->base_b * (1.0f - t) + theme->mid_b * t);
            } else {
                float t = (dist_from_center - 0.5f) * 2.0f;
                cr = (uint8_t)(theme->mid_r * (1.0f - t) + theme->peak_r * t);
                cg = (uint8_t)(theme->mid_g * (1.0f - t) + theme->peak_g * t);
                cb = (uint8_t)(theme->mid_b * (1.0f - t) + theme->peak_b * t);
            }

            out_len += snprintf(out_buf + out_len, sizeof(out_buf) - out_len, "\033[38;2;%d;%d;%dm", cr, cg, cb);

            for (int c = 0; c < cell_cols; c++) {
                uint8_t mask = grid[r * cell_cols + c];
                if (mask == 0) {
                    out_buf[out_len++] = ' ';
                } else {
                    uint32_t codepoint = 0x2800 | mask;
                    // UTF-8 3-byte encoding for U+2800..U+28FF
                    out_buf[out_len++] = (char)(0xE0 | ((codepoint >> 12) & 0x0F));
                    out_buf[out_len++] = (char)(0x80 | ((codepoint >> 6) & 0x3F));
                    out_buf[out_len++] = (char)(0x80 | (codepoint & 0x3F));
                }
            }
            out_len += snprintf(out_buf + out_len, sizeof(out_buf) - out_len, "\n");
        }

        // Help bar footer if toggled
        if (g_show_help) {
            out_len += snprintf(out_buf + out_len, sizeof(out_buf) - out_len,
                "\033[38;2;100;105;115m [q] Salir  [c] Tema (%s)  [m] Modo  [+/-] Ganancia (%.1fx)  [t] Trigger (%s)  [Space] %s\033[0m",
                theme->name, g_gain, g_trigger_sync ? "ON" : "OFF", g_paused ? "PAUSADO" : "PLAY");
        }

        write(STDOUT_FILENO, out_buf, out_len);
        free(grid);

        usleep(16666); // ~60 FPS
    }

    pa_simple_free(pa_stream);
    return 0;
}
