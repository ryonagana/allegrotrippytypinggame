#ifndef MAIN_H
#define MAIN_H

#include <allegro5/allegro.h>
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

#define LOG_ERROR(...) fprintf(stderr, __VA_ARGS__)
#define LOG(...) fprintf(stdout, __VA_ARGS__)

extern mouse_t g_mouse;

#endif // MAIN_H
