#include "main.h"

#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_opengl.h>


#include "particles.h"
#include "words.h"



ALLEGRO_DISPLAY *g_dsp = NULL;
ALLEGRO_EVENT_QUEUE *g_queue = NULL;
ALLEGRO_TIMER   *g_timer = NULL;
ALLEGRO_BITMAP  *g_screen = NULL;



ALLEGRO_SHADER *swirl_shader = NULL;
ALLEGRO_FONT *title_font = NULL;
ALLEGRO_FONT *title_font_40 = NULL;

ALLEGRO_SHADER *lavalamp_shader = NULL;


ALLEGRO_BITMAP *mj = NULL;

struct window_status_t {
    char message[255];
    int error;
};


particle_t *particle_mj = NULL;
mouse_t g_mouse = {0};

static void window_status_init(struct window_status_t * status);
static void window_status_set_message(struct window_status_t *status, const char *src);


static int32_t g_score = 0;

enum e_gamestate  {
    E_GAMESTATE_MENU,
    E_GAMESTATE_PLAY,
    E_GAMESTATE_GAMEOVER
};




enum e_gameplay_state {
    E_GAMEPLAY_START,
    E_GAMEPLAY_WAIT_KEY,
    E_GAMEPLAY_HIT,
    E_GAMEPLAY_RESET,
    E_GAMEPLAY_MISS,
    E_GAMEPLAY_RESTART
};


static int g_gamestate = E_GAMESTATE_MENU;
static int g_gameplay = E_GAMEPLAY_START;
static int g_actual_word = 0;




const char title_text[][1023] = { {"TRIPPY TYPING\0"},
                                  {"Entry for Krampus hack 2020\0"},
                                  {"Originally made with Raylib, converted to liballegro\0"},
                                  {0}
                                };


static int key_buffer[255] = {0};
static char hit_buffer[255] = {0};

static int g_remaining_words = 0;


static wordlist_t wordlist;
static wordlist_t *sort_words= NULL;


void main_game_reset(void);

static
void window_status_set_message(struct window_status_t *status, const char *src){

    int len = strnlen(src,sizeof(*src));
    strncpy(status->message, src,len);
    return;

}

static
void window_status_init(struct window_status_t * status){
    memset(status->message,0, sizeof(status->message));
    status->error = 0;
}

static
int init_allegro(struct window_status_t *status){
    if(!al_init()){
        window_status_set_message(status,"Allegro Failed to Init!");
        status->error = 1;
        return -1;
    }

    if(!al_install_keyboard()){
        window_status_set_message(status,"Allegro Keyboard Failed to Init!");
        status->error = 2;
        return -1;
    }

    if(!al_install_mouse()){
       window_status_set_message(status,"Allegro Mouse Failed to Init!");
        status->error = 2;
        return -1;
    }

    if(!al_install_audio()){
       window_status_set_message(status,"Allegro Audio Failed to Init!");
        status->error = 2;
        return -1;
    }


    if(!al_init_image_addon()){
       window_status_set_message(status,"Allegro Images Failed to Init!");
        status->error = 2;
        return -1;
    }

    if(!al_init_acodec_addon()){
       window_status_set_message(status,"Allegro Acodecs failed to Init!");
        status->error = 2;
        return -1;
    }

    if(!al_init_ttf_addon()){
       window_status_set_message(status,"Allegro TTF Failed to Init!");
        status->error = 2;
        return -1;
    }


    if(!al_init_native_dialog_addon()){
       window_status_set_message(status,"Allegro Dialogs Failed to Init!");
        status->error = 2;
        return -1;
    }

    if(!al_init_primitives_addon()){
        window_status_set_message(status,"Allegro Primitives  Failed to Init!");
        status->error = 2;
        return -1;
    }

    return 0;
}

static
int window_create_window(const char * title, int width, int height, int fullscreen, int vsync, int force_opengl)
{
    int flags = 0;

    if(fullscreen){
        flags |= ALLEGRO_FULLSCREEN_WINDOW;
    }

    if(vsync){
        al_set_new_display_option(ALLEGRO_VSYNC, vsync, ALLEGRO_SUGGEST);
    }

    /*
    if(force_opengl){
        flags |= ALLEGRO_OPENGL_3_0;
    }else {
#if defined(__WIN32__) || defined(__WIN64__)
        flags |= ALLEGRO_DIRECT3D;
#else
        flags |= ALLEGRO_OPENGL_3_0;
#endif
    }
*/
    (void)force_opengl;



    flags |= ALLEGRO_PROGRAMMABLE_PIPELINE | ALLEGRO_GENERATE_EXPOSE_EVENTS;
    flags |= ALLEGRO_OPENGL;

    al_set_new_display_flags(flags);
    al_set_new_window_title(title);
    g_dsp = al_create_display(width, height);
    if(!g_dsp) return -1;
    assert(g_dsp != NULL);

    g_queue = al_create_event_queue();
    if(!g_queue) return -1;
    assert(g_queue != NULL);

    g_timer = al_create_timer(1.0/60.0);
    if(!g_queue) return -1;
    assert(g_timer != NULL);

    g_screen = al_create_bitmap(width,height);
    al_set_target_bitmap(g_screen);
    al_clear_to_color(al_map_rgb_f(0,0,0));
    //al_lock_bitmap(g_screen, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_READWRITE);
    al_set_target_backbuffer(g_dsp);

    al_register_event_source(g_queue, al_get_display_event_source(g_dsp));
    al_register_event_source(g_queue, al_get_timer_event_source(g_timer));
    al_register_event_source(g_queue, al_get_keyboard_event_source());
    al_register_event_source(g_queue, al_get_mouse_event_source());


    title_font = al_load_ttf_font("res//hippie.ttf", 25,0); // al_create_builtin_font();
    title_font_40 =  al_load_ttf_font("res//hippie.ttf", 40,0);

    mj = al_load_bitmap("res//mj.png");

    al_start_timer(g_timer);
    return 0;

}

static
void unload_window(void){






    if(g_timer){
        al_destroy_timer(g_timer);
    }

    if(g_queue){
        al_destroy_event_queue(g_queue);
    }

    if(g_screen){
        al_unlock_bitmap(g_screen);
        al_destroy_bitmap(g_screen);
    }

    if(g_dsp){
        al_destroy_display(g_dsp);
    }
}




static
int shader_build(ALLEGRO_SHADER *shader){

    if(!al_build_shader(shader)){
        LOG("Build Shader Error!: %s", al_get_shader_log(shader));
        return -1;
    }

    return 0;
}



static
void render_mainmenu(ALLEGRO_BITMAP *bg, float res[] ){


    al_set_target_bitmap(bg);
    al_clear_to_color(al_map_rgb(255,255,255));
    al_set_target_backbuffer(g_dsp);

    al_set_target_bitmap(g_screen);
    al_clear_to_color(al_map_rgb(0,0,0));


    if(!al_use_shader(swirl_shader)){
        LOG("INVALID SHADER background");
    }

    if(!al_set_shader_float_vector("u_resolution", 2, (float*)res,1)){
         LOG_ERROR("Error: u_resolution not existent in Lavalamp\n");
    }



    if(!al_set_shader_float("u_time", (float)al_get_timer_count(g_timer) / 120.0 )){
         LOG_ERROR("Error: u_time  not existent in lavalamp\n");
    }



    al_draw_bitmap(bg,0,0,0);
    al_use_shader(NULL);



    for(int i=0; i < 3; i++){
        int tx,ty,tw,th;

         al_get_text_dimensions(title_font, title_text[i], &tx,&ty,&tw,&th);

         const  int title_x = (al_get_display_width(g_dsp) / 2) -  tx - 300;
         const  int title_y = (al_get_display_height(g_dsp) / 2) -  ty - 200;

         if(i == 0){

             al_draw_text(title_font_40, al_map_rgb(0,0,0), title_x+1, title_y + (i * 50) + 5,0,title_text[i]);

             if(!al_use_shader(lavalamp_shader)){
                 LOG("INVALID SHADER background");
             }

             if(!al_set_shader_float_vector("u_resolution", 2, (float*)res,1)){
                  LOG_ERROR("Error: u_resolution not existent\n");
             }




             if(!al_set_shader_float("u_time", (float)al_get_timer_count(g_timer) / 120.0 )){
                  LOG_ERROR("Error: u_resolution not existent\n");
             }


             al_draw_text(title_font_40, al_map_rgb(0,0,0), title_x+1, title_y + (i * 50) + 5,0,title_text[i]);
             al_use_shader(NULL);
             continue;

         }

          al_draw_text(title_font, al_map_rgb(0,255,0), title_x, title_y + (i * 50),0,title_text[i]);
    }

    al_set_target_backbuffer(g_dsp);



}

static
void main_load_shader(ALLEGRO_SHADER *shader, const char *vert_shader_path, const char *pixel_shader_path){

    if(!vert_shader_path){
        if(!al_attach_shader_source(shader,ALLEGRO_VERTEX_SHADER, al_get_default_shader_source(ALLEGRO_SHADER_GLSL, ALLEGRO_VERTEX_SHADER))){
            LOG("Vertex Shader error: %s", al_get_shader_log(shader));
        }
    }else {
        if(!al_attach_shader_source(shader,ALLEGRO_VERTEX_SHADER, vert_shader_path)){
            LOG("Vertex Shader error: %s", al_get_shader_log(shader));
        }
    }

    if(!pixel_shader_path){
        if(!al_attach_shader_source_file(shader, ALLEGRO_PIXEL_SHADER, al_get_default_shader_source(ALLEGRO_SHADER_GLSL, ALLEGRO_PIXEL_SHADER))){
            LOG_ERROR("Shader Error: %s\n", al_get_shader_log(shader));

        }
    }else {
        if(!al_attach_shader_source_file(shader, ALLEGRO_PIXEL_SHADER, pixel_shader_path )){
            LOG_ERROR("Shader Error: %s\n", al_get_shader_log(shader));

        }
    }


    if(shader_build(shader) < 0){
        LOG_ERROR("Shader Error: %s\n", al_get_shader_log(shader));
    }

}


void main_update_gameplay(wordlist_t *sort){

    switch(g_gameplay){
    case E_GAMEPLAY_START:
        {
          for(int i = 0; i < sort->total_words;i++){
              sort->words[i].x = al_get_display_width(g_dsp) - al_get_text_width(title_font, sort->words[i].word);
              sort->words[i].y = rand() % (al_get_display_height(g_dsp) / 2);
          }

          g_gameplay = E_GAMEPLAY_WAIT_KEY;
        }
        break;

        case E_GAMEPLAY_WAIT_KEY:
        {
            word_t *word = &sort->words[g_actual_word];

            if(word->x >=  -al_get_text_width(title_font,word->word) ){
                word->x--;
            }else {
                g_gameplay = E_GAMEPLAY_RESET;
            }


        }
        break;

        case E_GAMEPLAY_HIT:
                g_score += 10;
                g_gameplay = E_GAMEPLAY_WAIT_KEY;

        break;

        case E_GAMEPLAY_RESET:
        {
            g_actual_word++;
            g_remaining_words++;
            memset(key_buffer,0,sizeof(key_buffer));
            memset(hit_buffer,0,sizeof(hit_buffer));
            al_flush_event_queue(g_queue);
            sort->words[g_actual_word].x = al_get_display_width(g_dsp) - al_get_text_width(title_font, sort->words[g_actual_word].word);
            g_gameplay = E_GAMEPLAY_WAIT_KEY;
        }

        break;

        case E_GAMEPLAY_MISS:
            g_score -= 10;
            g_gameplay = E_GAMEPLAY_WAIT_KEY;
        break;

        case E_GAMEPLAY_RESTART:
             main_game_reset();
             g_gamestate = E_GAMESTATE_MENU;
        break;
    }

    return;
}

void main_render_gameplay(wordlist_t *sort)
{
    word_t *w = &sort->words[g_actual_word];
    ALLEGRO_BITMAP *bmp_text = NULL;
    int width = al_get_text_width(title_font_40, w->word);



    bmp_text = al_create_bitmap(width,30);

    al_set_target_bitmap(g_screen);
    al_clear_to_color(al_map_rgb(0,0,0));



    al_set_target_bitmap(bmp_text);
    //al_clear_to_color(al_map_rgb(0,0,0));
    al_draw_textf(title_font_40, al_map_rgb(255,0,0), 0,0,0, "%s", hit_buffer);

    al_set_target_bitmap(g_screen);



    al_draw_textf(title_font_40, al_map_rgb(220,220,220), w->x+1,w->y+1,0, "%s", w->word);
    al_draw_textf(title_font_40, al_map_rgb(0,255,0), w->x,w->y,0, "%s", w->word);
    al_draw_bitmap(bmp_text,w->x,w->y, 0);
    al_set_target_backbuffer(g_dsp);

    al_destroy_bitmap(bmp_text);




}


void main_game_reset(void){
    int sort_size = rand() % 10 -1;
    sort_size = sort_size <= 0 ? rand() % 10 -1 : sort_size;

    wordlist_unset(sort_words);
    sort_words  = NULL;

    sort_words = wordlist_sort(&wordlist, sort_size); //sort new words


    for(int i = 0; i < sort_words->total_words;i++){
        sort_words->words[i].x = al_get_display_width(g_dsp) - al_get_text_width(title_font_40, sort_words->words[i].word);
        sort_words->words[i].y = rand() % al_get_display_height(g_dsp) / 2;
    }

    //clean buffers
    memset(key_buffer,0,sizeof(key_buffer));
    memset(hit_buffer,0,sizeof(hit_buffer));
    g_remaining_words = sort_words->total_words;
    g_actual_word = 0;
    g_gameplay = E_GAMEPLAY_START;
}


int main(void)
{
    srand(time(0));

    struct window_status_t status;
    window_status_init(&status);

    init_allegro(&status);

    if(status.error < 0){
        LOG_ERROR("Error: %s", status.message);
        exit(1);
    }

    if(window_create_window("Trippy Typing", 800,600,0,1,1) < 0){
       LOG_ERROR("Error! Game Cannot be Initialize");
       exit(1);
    }

    int redraw = 0;
    int close = 0;


    ALLEGRO_BITMAP *background_bitmap = NULL;

    background_bitmap = al_create_bitmap(800,600);
    al_set_target_bitmap(background_bitmap);
    al_clear_to_color(al_map_rgb(255,255,255));
    //al_draw_filled_circle(400,300,200,al_map_rgb(255,0,0));
   //al_draw_filled_rectangle(0,0,al_get_bitmap_width(bg) ,al_get_bitmap_height(bg), al_map_rgb(255,255,255));
    al_set_target_backbuffer(g_dsp);


    swirl_shader = al_create_shader(ALLEGRO_SHADER_GLSL);
    assert(swirl_shader != NULL);
    lavalamp_shader = al_create_shader(ALLEGRO_SHADER_GLSL);
    assert(lavalamp_shader != NULL);


    main_load_shader(swirl_shader, NULL,"assets//shader//swirl_glsl_allegro.frag");
    main_load_shader(lavalamp_shader, NULL,"assets//shader//lavalamp_glsl_allegro.frag");



    const float res[][2] = {
                                {al_get_display_width(g_dsp), al_get_display_height(g_dsp)},
                                {0}
                           };


    al_use_shader(NULL);


    words_load_file(&wordlist, "words");


    int sort_size = rand() % 10 -1;
    sort_size = sort_size <= 0 ? rand() % 10 -1 : sort_size;
    (void)sort_size;

    sort_words = wordlist_sort(&wordlist, sort_size);



    particle_init(&particle_mj, PARTICLES_MAX);


    while(!close){

        if(redraw){
            redraw = 0;

            al_clear_to_color(al_map_rgb(0,0,0));


            if(g_gamestate == E_GAMESTATE_MENU){
                render_mainmenu(background_bitmap, (float*)res);
                al_clear_to_color(al_map_rgba(0,0,0,0));
                 al_draw_bitmap(g_screen,0,0,0);


            }

            if(g_gamestate == E_GAMESTATE_PLAY){
                main_render_gameplay(sort_words);
                al_draw_bitmap(g_screen,0,0,0);
                al_draw_textf(title_font_40, al_map_rgb(0,255,0),0,50,0, "Score: %.2d", g_score);
                al_draw_textf(title_font_40, al_map_rgb(0,255,0),0,80,0, "Words: %.2d/%.2d", sort_words->total_words, sort_words->total_words - g_remaining_words);

                if((sort_words->total_words - g_remaining_words) <= 0 ){
                    g_gameplay = E_GAMEPLAY_RESTART;
                }

            }



            particle_draw(particle_mj, mj);


            al_flip_display();
        }

        do {
            ALLEGRO_EVENT e;
            al_wait_for_event(g_queue, &e);

            if(e.type == ALLEGRO_EVENT_DISPLAY_CLOSE){
                close = 1;
            }


            if(e.type == ALLEGRO_EVENT_TIMER){
                 particle_update(particle_mj, PARTICLES_MAX);

                 if(g_gamestate == E_GAMESTATE_PLAY){
                        main_update_gameplay(sort_words);



                 }

                if(g_mouse.button & MOUSE_BT_LEFT){
                    particle_generate_explosion(
                                particle_mj,
                                g_mouse.x,
                                g_mouse.y,
                                rand() % 300 / 2, 6,1);
                }




                word_t *w = &sort_words->words[g_actual_word];


                if(key_buffer[w->hit] == w->word[w->hit] && w->hit <= w->len){
                    hit_buffer[w->hit] = w->word[w->hit];
                    LOG("Hit %c\n", w->word[w->hit]);
                     w->hit++;
                }else  if(w->hit >= w->len && (sort_words->total_words - g_remaining_words) > 0   ){
                    g_gameplay = E_GAMEPLAY_RESET;
                }



                redraw = 1;
            }


            if(e.type == ALLEGRO_EVENT_KEY_UP){

                if(e.keyboard.keycode == ALLEGRO_KEY_ESCAPE){
                    main_game_reset();
                    g_gamestate = E_GAMESTATE_MENU;
                }

            }

            if(e.type == ALLEGRO_EVENT_KEY_CHAR){

                if(g_gameplay != E_GAMESTATE_PLAY){
                    g_gamestate = E_GAMESTATE_PLAY;
                }

                if(g_gameplay == E_GAMESTATE_PLAY){
                    const int key = e.keyboard.unichar;


                    word_t *w = &sort_words->words[g_actual_word];




                    if(key  >= 97  && key <= 122){
                        key_buffer[w->hit] = key;

                        if(key_buffer[w->hit] != w->word[w->hit]){
                            g_gameplay = E_GAMEPLAY_MISS;
                        }else {
                            g_gameplay = E_GAMEPLAY_HIT;
                        }






                        LOG("%c\n", key);
                    }
                }

            }




            if(e.type == ALLEGRO_EVENT_MOUSE_AXES){
                g_mouse.x = e.mouse.x;
                g_mouse.y = e.mouse.y;
            }

            if(e.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN){
                if(e.mouse.button & 1){
                    g_mouse.button |= MOUSE_BT_LEFT;
                }

                if(e.mouse.button & 2){
                    g_mouse.button |= MOUSE_BT_RIGHT;
                }

            }

            if(e.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP){
                if(e.mouse.button & 1){
                    g_mouse.button &= ~ MOUSE_BT_LEFT;
                }

                if(e.mouse.button & 2){
                    g_mouse.button &= ~MOUSE_BT_RIGHT;
                }
            }


        }while(!al_event_queue_is_empty(g_queue));
    }

    if(swirl_shader){
        al_destroy_shader(swirl_shader);
    }

    if(lavalamp_shader){
        al_destroy_shader(lavalamp_shader);
    }

    if(title_font){
        al_destroy_font(title_font);
    }

    if(title_font_40){
        al_destroy_font(title_font_40);
    }

    if(background_bitmap){
        al_destroy_bitmap(background_bitmap);
    }

    particle_unset(&particle_mj);

    wordlist_unset(sort_words);
    unload_window();

    return 0;
}
