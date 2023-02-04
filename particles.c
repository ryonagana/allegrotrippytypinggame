#include "particles.h"
#include "main.h"

void particle_init(particle_t **p, int max)
{

    particle_t *tmp = NULL;

    tmp = calloc(max,sizeof(particle_t));


    for(int i = 0; i < max;i++){
        tmp[i].x = 0;
        tmp[i].y = 0;
        tmp[i].dx = 0;
        tmp[i].dy = 0;
        tmp[i].color = al_map_rgba(255,255,255,255);
        tmp[i].size = (rand() % 30) / 20.0f;
        tmp[i].rot = (rand() % 3600) / 10.0f;
        tmp[i].ttl = 0;
        tmp[i].alive = 0;
        tmp[i].fade = 0.0f;
        tmp[i].fade_speed = 0.0;
    }

    *p = tmp;

    return;

}

void particle_set_single(particle_t *p, float x, float y, float dx, float dy, ALLEGRO_COLOR color, float size, float rot, int ttl, int alive, float fade_speed)
{

    if(!p ) return;

    p->x = x;
    p->y = y;
    p->dx = dx;
    p->dy = dy;
    p->color  = color;
    p->size = size;
    p->rot = rot;
    p->ttl = ttl;
    p->alive = alive;
    p->fade = 1.0;
    p->fade_speed = fade_speed;


}

void particle_update(particle_t *p, const int max)
{

    for(int i = 0; i < max; i++){
        if(p[i].alive) continue;
        p[i].x +=  p->dx * 1.5;
        p[i].y +=  p->dy * 1.5;
        p[i].ttl--;
        p[i].rot += 0.09f;
        p[i].alive = p->ttl > 0 ? 1: 0;
        p[i].fade -= p[i].fade_speed;
        p[i].color.a -= p[i].fade;


    }

    /*
    for(int i = 0; i < max;i++){
        if(!p->alive && p->ttl <= 0) continue;

        const int64_t now = al_get_timer_count(g_timer);
        static int64_t time = 0;





        if(now >= time + 50){

            p[i].x +=  p->dx * 0.5;
            p[i].y +=  p->dy * 0.5;
            p[i].rot += 0.9;
            p[i].ttl--;
            p[i].alive = p->ttl > 0 ? 1: 0;

            time = now;
            //LOG("Time: %lld", time);

        }


    }
    */
}

void particle_draw(const particle_t *p, ALLEGRO_BITMAP *bmp)
{

    ALLEGRO_STATE state;

    al_store_state(&state, ALLEGRO_STATE_BLENDER);

    if(!bmp){
        return;
    }

    if(p->alive){

        al_set_blender(ALLEGRO_ADD, ALLEGRO_ONE, ALLEGRO_INVERSE_ALPHA);
        al_draw_tinted_scaled_rotated_bitmap_region(bmp,
                                                    0,
                                                    0,
                                                    al_get_bitmap_width(bmp),
                                                    al_get_bitmap_height(bmp),
                                                    p->color,
                                                    p->x / 2,
                                                    p->y / 2,
                                                    p->dx + p->x,
                                                    p->dy + p->y,
                                                    p->size,
                                                    p->size,
                                                    p->rot,
                                                    0
                                                    );

    }

    al_restore_state(&state);
}

particle_t *particle_free_slot(particle_t *p, int max)
{
    int i = 0;
    while(p[i].ttl > 0 && i < max) i++;
    if(i == max) return NULL;
    return &p[i];
}


void particle_generate_rain(particle_t *p, int x, int y, int count, int life, float fade){

    (void)p;(void)x;(void)y;(void)count;(void)life;(void)fade;
    particle_t *tmp =  NULL;

    for(int i = 0;i < count;i++){
        tmp = particle_free_slot(p,PARTICLES_MAX);

        //int spread = (rand() % 3600 / 10) / 4;
        //int spread_val_x = rand() % spread - (spread / 2);  //GetRandomValue(0, spread) - spread / 2;
        //int spread_val_y = rand() % spread - (spread / 2); //GetRandomValue(0, spread) - spread / 2;
        ALLEGRO_COLOR c = al_map_rgba(rand() % 255, rand() % 255, rand() % 255,rand() % 128);

        //Color c = (Color){ GetRandomValue(0,255), GetRandomValue(0,255), GetRandomValue(0,255), GetRandomValue(0,255) };
        float size = rand() % 30 / 20.0f;
        //float sx =   x + rand() % 100 / 100 - 0.5;
        //float sy =   y + rand() % 100 / 100 - 0.5;
        float rot =  rand() % 3600 / 10;
        int ttl = life ? life : rand() % 450;

        particle_set_single(tmp,x,y,0,-1.0f,c, size, rot,ttl,1,0.08f);
    }

}


void particle_generate_explosion(particle_t *p, int x, int y, int spread, int count, int life, float fade)
{
    int i;
    particle_t *tmp;



    for(i = 0; i < count; i++){
        tmp =  particle_free_slot(p, PARTICLES_MAX);  //Particle_GetFreeSlot(p, PARTICLES_MAX);

        if(spread <= 0) spread = 1;
        if(tmp != NULL){
            int spread_val_x = rand() % spread - (spread / 2);  //GetRandomValue(0, spread) - spread / 2;
            int spread_val_y = rand() % spread - (spread / 2); //GetRandomValue(0, spread) - spread / 2;
            ALLEGRO_COLOR c = al_map_rgba(rand() % 255, rand() % 255, rand() % 255,rand() % 128);

            //Color c = (Color){ GetRandomValue(0,255), GetRandomValue(0,255), GetRandomValue(0,255), GetRandomValue(0,255) };
            float size = rand() % 30 / 20.0f;
            float sx =   x + rand() % 100 / 100 - 0.5;
            float sy =   y + rand() % 100 / 100 - 0.5;
            float rot =  rand() % 3600 / 10;
            int ttl = life ? life : rand() % 450;

            particle_set_single(tmp,spread_val_x, spread_val_y, sx,sy, c, size, rot,ttl,1, fade);
        }
    }

}


void particle_unset(particle_t **p)
{
    if(*p){
        free((void*)*p);
        p = NULL;
    }
}
