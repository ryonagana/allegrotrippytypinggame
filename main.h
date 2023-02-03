#ifndef MAIN_H
#define MAIN_H

#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_opengl.h>
#include <allegro5/allegro_physfs.h>
#include <physfs.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

extern ALLEGRO_DISPLAY *g_dsp;
extern ALLEGRO_EVENT_QUEUE *g_queue;
extern ALLEGRO_TIMER   *g_timer;
extern ALLEGRO_BITMAP  *g_screen;


enum MOUSE_BUTTON {
    MOUSE_BT_LEFT=0x2,
    MOUSE_BT_RIGHT=0x4
};


typedef struct mouse_t {
    int x;
    int y;
    uint8_t button;
    float mwheel;

}mouse_t;


#define GAME_PI  3.14159265358979323846
#define GAME_PI2 6.28318530717958647692

#define RAD2DEG 57.295779513082320876846364344191
#define DEG2RAD 0.0174532925199432957692222222222

#ifdef DEBUG
#define LOG_ERROR(...) al_append_native_text_log(window_log, __VA_ARGS__)  //fprintf(stderr, __VA_ARGS__)
#define LOG(...)       al_append_native_text_log(window_log, __VA_ARGS__)
#else
#define LOG_ERROR(...)
#define LOG(...)
#endif

extern mouse_t g_mouse;

#endif // MAIN_H
