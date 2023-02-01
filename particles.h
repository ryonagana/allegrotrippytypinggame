#ifndef PARTICLES_H
#define PARTICLES_H

#define PARTICLES_MAX 255

#include <allegro5/allegro.h>

typedef struct particle_t {
    float x;
    float y;
    float dx;
    float dy;

    ALLEGRO_COLOR color;
    float alpha;
    float size;
    float rot;
    int ttl;
    int alive;
}particle_t;


void particle_init(particle_t **p, int max);
void particle_set_single(particle_t *p, float x, float y, float dx, float dy, ALLEGRO_COLOR color, float size, float rot, int ttl,int alive);
void particle_update(particle_t *p, const int max);
void particle_draw(const particle_t *p, ALLEGRO_BITMAP *bmp);
particle_t *particle_free_slot(particle_t *p, int max);
void particle_generate_explosion(particle_t *p, int x, int y, int spread, int count, int life);
void particle_unset(particle_t **p);

#endif // PARTICLES_H
