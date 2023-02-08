#include "main.h"



#include "particles.h"
#include "words.h"

ALLEGRO_VOICE *voice = NULL;
ALLEGRO_MIXER *mixer = NULL;


ALLEGRO_DISPLAY *g_dsp = NULL;
ALLEGRO_EVENT_QUEUE *g_queue = NULL;
ALLEGRO_TIMER   *g_timer = NULL;
ALLEGRO_BITMAP  *g_screen = NULL;



ALLEGRO_SHADER *swirl_shader = NULL;
ALLEGRO_FONT *title_font = NULL;
ALLEGRO_FONT *title_font_40 = NULL;

ALLEGRO_SHADER *lavalamp_shader = NULL;
ALLEGRO_BITMAP *mj = NULL;


static ALLEGRO_TEXTLOG *window_log = NULL;

static ALLEGRO_COLOR text_color;

struct window_status_t {
    char message[255];
    int error;
};





ALLEGRO_BITMAP *bg_gameplay = NULL;
ALLEGRO_BITMAP *background_bitmap = NULL;


particle_t *particle_mj = NULL;
mouse_t g_mouse = {0};

static void window_status_init(struct window_status_t * status);
static void window_status_set_message(struct window_status_t *status, const char *src);


static int32_t g_score = 0;

enum e_gamestate  {
    E_GAMESTATE_MENU,
    E_GAMESTATE_PLAY,
    E_GAMESTATE_ROUND_SCREEN,
    E_GAMESTATE_GAMEOVER
};

enum e_gameplay_state {
    E_GAMEPLAY_START,
    E_GAMEPLAY_WAIT_KEY,
    E_GAMEPLAY_HIT,
    E_GAMEPLAY_RESET,
    E_GAMEPLAY_MISS
};


static int g_gamestate = E_GAMESTATE_PLAY;
static int g_gameplay = E_GAMEPLAY_START;
static int g_actual_word = 0;
static int32_t g_round = 1;



const char title_text[][1023] = { {" Allegro TRIPPY TYPING\0"},
                                  {"Entry for Krampus hack 2020-2020\0"},
                                  {"Originally made with Raylib, converted to liballegro\0"},
                                  {0}
                                };


static int key_buffer[255] = {0};
static char hit_buffer[255] = {0};

static int g_remaining_words = 0;


static wordlist_t wordlist;
static wordlist_t *sort_words= NULL;


const char credit_name[] = {"Created by Archdark (a.k.a Ryonagana)"};

typedef struct round_modifier_t {
    float speed;
    int max_words;
    int score_min;
} round_modifier_t;



static round_modifier_t g_round_modifier[9] = {
        {1.0f,10,9},
        {1.2f,15,7},
        {2.6f,20, 5},
        {2.8f,25, 4},
        {2.99f,55,1},
        {3.5f,55,1},
        {3.9f,55,1},
        {6.3f,55,1},
        {0,0,0}
};


typedef struct sfx_t {
    ALLEGRO_SAMPLE *sample;
    ALLEGRO_SAMPLE_INSTANCE* inst;
}sfx_t;

sfx_t title_bgm;
sfx_t hippie_bgm;
sfx_t round_bgm;
sfx_t hit_sfx;
sfx_t miss_sfx;
sfx_t lost_sfx;
sfx_t reset_sfx;

static round_modifier_t actual_modifier;


void main_game_reset(void);


static int g_life = 3;


//sounds
void main_game_init_sounds(void);
void main_game_unload_sounds(void);



void sfx_play(const sfx_t *sfx, float vol, float pan, float speed, ALLEGRO_PLAYMODE mode);
void sfx_stop(sfx_t *sfx);
void sfx_destroy(sfx_t *sfx);
int sfx_load(sfx_t *sfx, const  char* filepath);
void sfx_stop_all(void);

static
void window_status_set_message(struct window_status_t *status, const char *src){

    strncpy(status->message, src,255);
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

    /*
    if(!al_init_primitives_addon()){
        window_status_set_message(status,"Allegro Primitives  Failed to Init!");
        status->error = 2;
        return -1;
    }
    */

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



    if(window_log){
        al_close_native_text_log(window_log);
    }


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
void main_render_mainmenu(ALLEGRO_BITMAP *bg, float res[] ){


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

             //al_draw_text(title_font_40, al_map_rgb(0,0,0), title_x+1, title_y + (i * 50) + 5,0,title_text[i]);

             if(!al_use_shader(swirl_shader)){
                 LOG("INVALID SHADER background");
             }

             if(!al_set_shader_float_vector("u_resolution", 2, (float*)res,1)){
                  LOG_ERROR("Error: u_resolution not existent\n");
             }




             if(!al_set_shader_float("u_time", (float)al_get_timer_count(g_timer)/60)){
                  LOG_ERROR("Error: u_resolution not existent\n");
             }


             al_draw_text(title_font_40, al_map_rgb(0,0,0), title_x+1, title_y + (i * 50) + 5,0,title_text[i]);
             al_use_shader(NULL);
             continue;

         }

          al_draw_text(title_font, al_map_rgb(0,255,0), title_x, title_y + (i * 50),0,title_text[i]);
    }


    const int w = al_get_display_width(g_dsp) / 2;

    int tx,ty,tw,th;
    al_get_text_dimensions(title_font,credit_name, &tx,&ty,&tw,&th);

    const int h = al_get_display_height(g_dsp);
    al_draw_textf(title_font, al_map_rgb(0,255,0), w/2,h -th,0, "%s", credit_name);

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

     word_t *word = &sort->words[g_actual_word];

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


            if(word->x >=  -al_get_text_width(title_font,word->word) ){
                float round_speed = 0;
                if((g_round < 2 )){
                    round_speed = (g_round/2);
                }else {
                    round_speed = 1.5f;
                }

                word->x -= 1.0f * actual_modifier.speed + round_speed;
            }else {

                if(g_life <= 0){
                    main_game_reset();
                    g_gameplay = E_GAMEPLAY_RESET;
                    g_gamestate = E_GAMESTATE_GAMEOVER;

                }
                sfx_play(&lost_sfx,1.0f,0.5f,1.0f, ALLEGRO_PLAYMODE_ONCE);
                g_life--;
                g_score -= 10 * abs((g_round / 2) - 10);
                g_gameplay = E_GAMEPLAY_RESET;
            }


        }
        break;

        case E_GAMEPLAY_HIT:
                g_score += 10;
                particle_generate_rain(particle_mj,  word->x,word->y,12, rand() % 350, 0.001f);
                sfx_play(&hit_sfx,1.0,.5f, 1.0,ALLEGRO_PLAYMODE_ONCE);
                g_gameplay = E_GAMEPLAY_WAIT_KEY;

                //particle_generate_explosion(particle_mj, word->x,word->y, rand() % 360, 12, rand() % 250);

        break;

        case E_GAMEPLAY_RESET:
        {
            g_actual_word++;
            g_remaining_words++;
            memset(key_buffer,0,sizeof(key_buffer));
            memset(hit_buffer,0,sizeof(hit_buffer));
            al_flush_event_queue(g_queue);
            sort->words[g_actual_word].x = al_get_display_width(g_dsp) - al_get_text_width(title_font, sort->words[g_actual_word].word);
            text_color = al_map_rgba(rand()%255,rand()%255,rand()%255,255);
            sfx_play(&hit_sfx,1.0,.5f, 1.0,ALLEGRO_PLAYMODE_ONCE);
            g_gameplay = E_GAMEPLAY_WAIT_KEY;
        }

        break;

        case E_GAMEPLAY_MISS:

            if(key_buffer[word->hit] >= 97 && key_buffer[word->hit] <=  122){
                sfx_play(&miss_sfx,1.0f,0.5f,1.0f, ALLEGRO_PLAYMODE_ONCE);
                g_score -= 10;

            }
            g_gameplay = E_GAMEPLAY_WAIT_KEY;
        break;
    }

    return;
}

void main_render_gameplay(wordlist_t *sort)
{
    word_t *w = &sort->words[g_actual_word];

    al_set_target_bitmap(g_screen);
    al_clear_to_color(al_map_rgb(0,0,0));
    ALLEGRO_USTR *hit_buffer_utf8 = al_ustr_newf("%s",hit_buffer);

    const float res[][2] = {
                                {al_get_display_width(g_dsp), al_get_display_height(g_dsp)},
                                {0}
    };







    if(!al_use_shader(lavalamp_shader)){
        LOG("INVALID SHADER background");
    }

    if(!al_set_shader_float_vector("u_resolution", 2, (float*)res,1)){
         LOG_ERROR("Error: u_resolution not existent\n");
    }

    if(!al_set_shader_float("u_time", (float)al_get_timer_count(g_timer)/60)){
         LOG_ERROR("Error: u_resolution not existent\n");
    }

    al_draw_bitmap(bg_gameplay,0,0,0);
    al_use_shader(NULL);




    al_draw_textf(title_font_40, al_map_rgb(220,220,220), w->x+1,w->y+1,0, "%s", w->word);
    al_draw_textf(title_font_40, text_color, w->x,w->y,0, "%s", w->word);
    //al_draw_ustr(title_font_40, al_map_rgb(220,220,220),w->x+1, w->y+1, 0, w->word_utf8);
    //al_draw_ustr(title_font_40, text_color,w->x, w->y, 0, w->word_utf8);
    al_draw_ustr(title_font_40, al_map_rgb(255,0,0),w->x, w->y,0,hit_buffer_utf8);

    al_ustr_free(hit_buffer_utf8);






    al_draw_textf(title_font_40, al_map_rgb(0,255,0),0,5,0, "Score: %.2d", g_score);
    al_draw_textf(title_font_40, al_map_rgb(0,255,0),0,50,0, "Words: %.2d / %.2d", sort_words->total_words, sort_words->total_words - g_remaining_words);
    al_draw_textf(title_font_40, al_map_rgb(0,255,0),0,100,0, "Lives: %.2d", g_life);

    al_set_target_backbuffer(g_dsp);

}


void main_game_reset(void){
    int sort_size = rand() % 10 + actual_modifier.max_words;
    sort_size = sort_size <= 1 ? rand() % (1 + actual_modifier.max_words) : sort_size;

    wordlist_unset(&sort_words);
    sort_words  = NULL;

    sort_words = wordlist_sort(&wordlist, sort_size);


    for(int i = 0; i < sort_words->total_words;i++){
        sort_words->words[i].x = al_get_display_width(g_dsp) - al_get_text_width(title_font_40, sort_words->words[i].word);
        sort_words->words[i].y = rand() % (al_get_display_height(g_dsp) / 2) - 100;

        if(sort_words->words[i].y < al_get_display_height(g_dsp)){
            sort_words->words[i].y =  (al_get_display_height(g_dsp) / 2);
        }

        if(sort_words->words[i].y < 0){
            sort_words->words[i].y =  (al_get_display_height(g_dsp) / 2);
        }


    }

    //clean buffers
    memset(key_buffer,0,sizeof(key_buffer));
    memset(hit_buffer,0,sizeof(hit_buffer));
    g_remaining_words = 0;
    g_actual_word = 0;
    //g_gamestate = E_GAMESTATE_MENU;
    //g_gameplay = E_GAMEPLAY_START;
}

void round_start(int round){

    const int64_t now = al_get_timer_count(g_timer) / 60;
    static int64_t time  = 0;
    static int round_secs = 0;
    size_t modifiers_size = sizeof(g_round_modifier)/sizeof(g_round_modifier[0]);
    int w,h;

    if(now > time + 1){
        time = now;
        round_secs++;
    }

    if(round <= 0){
          actual_modifier = g_round_modifier[0];
    }else if((size_t)round > modifiers_size-1){
        actual_modifier = g_round_modifier[modifiers_size-1];
    }

    if(round < 2){
        actual_modifier = g_round_modifier[0];

    }

    if(round > 2 && round <= 4){
        actual_modifier = g_round_modifier[1];
    }


    if(round > 4 && round <= 6){
        actual_modifier = g_round_modifier[2];
    }

    if(round > 6 && round <= 8){
        actual_modifier = g_round_modifier[3];
    }


    if(round > 8 && round <= 10){
        actual_modifier = g_round_modifier[3];
    }

    if(round > 10){
        actual_modifier = g_round_modifier[4];
    }


    if(round > 15){
        actual_modifier = g_round_modifier[5];
    }

    if(round > 20){
        actual_modifier = g_round_modifier[6];
    }

    if(round > 25){
        actual_modifier = g_round_modifier[7];
    }




    w = al_get_display_width(g_dsp);
    h = al_get_display_height(g_dsp);

    al_set_target_bitmap(g_screen);
    al_clear_to_color(al_map_rgb(0,0,0));

    int tw;
    char buf[56] = {0};

    if(round_secs == 1){

        snprintf(buf,sizeof(buf), "ROUND %.2d", round);

        tw = al_get_text_width(title_font_40, buf);
        al_draw_textf(title_font_40, al_map_rgb(255,255,0), (w/2)-tw,h/2,0,"%s", buf);
    }

    if(round_secs == 2){
        tw = al_get_text_width(title_font_40, "TYPE!");
        al_draw_textf(title_font_40, al_map_rgb(255,255,0), (w/2)-tw,h/2,0, "TYPE!");
    }


    if(round_secs >= 3) {
        g_gamestate = E_GAMESTATE_PLAY;
        time = 0;
        round_secs = 0;
    }

    al_set_target_backbuffer(g_dsp);



}


int main(int argc, char **argv)
{
    srand(time(0));

    struct window_status_t status;
    window_status_init(&status);

    init_allegro(&status);

#ifdef GAME_DATA_PACK
    ALLEGRO_FILE *gamedata = al_fopen("game.pack","rb");

    if(!gamedata){
        LOG("game.pack file not found... exiting\n");
        unload_window();
        exit(1);
    }

    PHYSFS_init(NULL);

    if(!PHYSFS_mount("game.pack","//",1)){
        LOG("game.pack failed to mount.. exiting\n");
        unload_window();
        exit(1);
    }

    LOG("PhysFS Mounted at: /");
    al_set_physfs_file_interface();
    LOG("PhysFS Started");
#endif

    if(status.error < 0){
        LOG_ERROR("Error: %s", status.message);
        exit(1);
    }





    if(window_create_window("Trippy Typing", 1330,768,0,1,1) < 0){
       LOG_ERROR("Error! Game Cannot be Initialize");
       exit(1);
    }

    int redraw = 0;
    int close = 0;



#ifdef DEBUG
    window_log = al_open_native_text_log("DEBUG",ALLEGRO_TEXTLOG_MONOSPACE | ALLEGRO_TEXTLOG_NO_CLOSE);
#endif


    background_bitmap = al_create_bitmap(al_get_display_width(g_dsp),al_get_display_height(g_dsp));
    al_set_target_bitmap(background_bitmap);
    al_clear_to_color(al_map_rgb(255,255,255));
    //al_draw_filled_circle(400,300,200,al_map_rgb(255,0,0));
   //al_draw_filled_rectangle(0,0,al_get_bitmap_width(bg) ,al_get_bitmap_height(bg), al_map_rgb(255,255,255));
    al_set_target_backbuffer(g_dsp);


    bg_gameplay = al_create_bitmap(al_get_display_width(g_dsp),al_get_display_height(g_dsp));
    al_set_target_bitmap(bg_gameplay);
    al_clear_to_color(al_map_rgb(255,255,255));
    //al_draw_filled_circle(400,300,200,al_map_rgb(255,0,0));
   //al_draw_filled_rectangle(0,0,al_get_bitmap_width(bg) ,al_get_bitmap_height(bg), al_map_rgb(255,255,255));
    al_set_target_backbuffer(g_dsp);



    main_game_init_sounds();


    swirl_shader = al_create_shader(ALLEGRO_SHADER_GLSL);
    assert(swirl_shader != NULL);
    lavalamp_shader = al_create_shader(ALLEGRO_SHADER_GLSL);
    assert(lavalamp_shader != NULL);


    main_load_shader(swirl_shader, NULL,"assets//shader//swirl_glsl_allegro.frag");
    main_load_shader(lavalamp_shader, NULL,"assets//shader//lavalamp_glsl_allegro.frag");




    al_use_shader(NULL);





    al_set_standard_file_interface();

    if(argc < 2){
        if(words_load_file(&wordlist, "words") < 0){
            LOG("Failed to Load Words File!");
            unload_window();
            exit(1);
        }
    }else {
        char buf[127] = {0};
        strncpy(buf,argv[1],sizeof(buf)-1);
        if(words_load_file(&wordlist, buf) < 0){
            LOG("Failed to Load Words File!");
            unload_window();
            exit(1);
        }
    }

    al_set_physfs_file_interface();


    int sort_size = rand() % 10 -1;
    sort_size = sort_size <= 0 ? rand() % 10 -1 : sort_size;
    (void)sort_size;

    sort_words = wordlist_sort(&wordlist, sort_size);



    particle_init(&particle_mj, PARTICLES_MAX);
    text_color = al_map_rgba(rand()%255,rand()%255,rand()%255,255);
    al_clear_to_color(al_map_rgb(0,0,0));

    actual_modifier = g_round_modifier[0];
    main_game_reset();



    sfx_play(&hippie_bgm, 1.0f,.5f,1.0f, ALLEGRO_PLAYMODE_LOOP);

    const float res[][2] = {
                                {al_get_display_width(g_dsp), al_get_display_height(g_dsp)},
                                {0}
    };


    while(!close){

        if(redraw){
            redraw = 0;


            al_clear_to_color(al_map_rgb(0,0,0));


            if(g_gamestate == E_GAMESTATE_MENU){

                 main_render_mainmenu(background_bitmap, (float*)res);
                 al_clear_to_color(al_map_rgba(0,0,0,0));
                 al_draw_bitmap(g_screen,0,0,0);


            }

            if(g_gamestate == E_GAMESTATE_ROUND_SCREEN){
                round_start(g_round);
                al_draw_bitmap(g_screen,0,0,0);
            }


            if(g_gamestate == E_GAMESTATE_PLAY){


                main_render_gameplay(sort_words);
                al_draw_bitmap(g_screen,0,0,0);




                if((sort_words->total_words - g_remaining_words) <= 0){
                    main_game_reset();
                    g_gameplay = E_GAMEPLAY_START;
                    g_gamestate = E_GAMESTATE_ROUND_SCREEN;
                    g_round++;
                }


            }

            if(g_gamestate == E_GAMESTATE_GAMEOVER){
                sfx_stop_all();
                char buf[25] = {0};
                snprintf(buf,sizeof(buf),"GAME OVER!");

                int w = (al_get_display_width(g_dsp) / 2)-al_get_text_width(title_font_40,buf);
                int h = al_get_display_height(g_dsp) / 2;

                al_set_target_bitmap(g_screen);
                al_clear_to_color(al_map_rgb(0,0,0));
                al_draw_text(title_font_40, al_map_rgb(0,255,0),w,h,0, buf);
                al_set_target_backbuffer(g_dsp);

                al_draw_bitmap(g_screen,0,0,0);
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
                        sfx_stop_all();
                        sfx_play(&hippie_bgm, .4f,.5f,1.0f, ALLEGRO_PLAYMODE_ONCE);
                        main_update_gameplay(sort_words);

                 }


                 if(g_gamestate == E_GAMESTATE_GAMEOVER){
                    main_game_reset();
                    g_round = 1;
                    g_life = 3;
                    g_score = 0;
                    actual_modifier = g_round_modifier[0];
                    g_gamestate = E_GAMESTATE_GAMEOVER;
                    g_gameplay  = E_GAMEPLAY_RESET;
                 }


                 if(g_gamestate == E_GAMESTATE_ROUND_SCREEN){
                     sfx_stop_all();
                     sfx_play(&round_bgm,1.0f, 0.5f, 1.0f, ALLEGRO_PLAYMODE_ONCE);
                 }


                if(g_mouse.button & MOUSE_BT_LEFT){

                    /*
                    particle_generate_explosion(
                                particle_mj,
                                g_mouse.x,
                                g_mouse.y,
                                rand() % 300 / 2, 6,1);
                    */
                }


                word_t *w = &sort_words->words[g_actual_word];


                if(key_buffer[w->hit%255] == w->word[w->hit%255] && w->hit <= w->len && key_buffer[w->hit] != ' ' ){
                    hit_buffer[w->hit%255] = w->word[w->hit%255];
                    LOG("Hit %c\n", w->word[w->hit%255]);
                     w->hit++;
                }else  if(w->hit == w->len && (sort_words->total_words - g_remaining_words) > 0){
                    g_gameplay = E_GAMEPLAY_RESET;
                    sfx_play(&reset_sfx,1.0f,0.5,1.0f, ALLEGRO_PLAYMODE_ONCE);
                }


                redraw = 1;
            }

            ALLEGRO_KEYBOARD_STATE kbd_state;

            al_get_keyboard_state(&kbd_state);


#ifdef DEBUG
            if(al_key_down(&kbd_state,ALLEGRO_KEY_LCTRL) && al_key_down(&kbd_state,ALLEGRO_KEY_R)){
                main_game_reset();
                g_round++;
                g_gameplay = E_GAMEPLAY_START;
                g_gamestate = E_GAMESTATE_ROUND_SCREEN;
            }
#endif


            if(e.type == ALLEGRO_EVENT_KEY_UP){

                if(e.keyboard.keycode == ALLEGRO_KEY_ESCAPE && g_gamestate == E_GAMESTATE_MENU){
                        main_game_reset();
                        g_gamestate = E_GAMESTATE_MENU;
                }

            }

            if(e.type == ALLEGRO_EVENT_KEY_CHAR){

                if(g_gamestate == E_GAMESTATE_MENU){
                    sfx_stop(&hippie_bgm);
                    g_gamestate = E_GAMESTATE_ROUND_SCREEN;
                }

                if(g_gamestate == E_GAMESTATE_GAMEOVER){
                    sfx_play(&round_bgm,1.0,0.5,0.3,ALLEGRO_PLAYMODE_ONCE);
                    g_gameplay = E_GAMEPLAY_RESET;
                    g_gamestate = E_GAMESTATE_MENU;
                }

                if(g_gameplay == E_GAMESTATE_PLAY){
                    sfx_stop(&round_bgm);
                    const int key = e.keyboard.unichar;


                    word_t *w = &sort_words->words[g_actual_word];


                    if(key  >= 97  && key <= 122){
                        key_buffer[w->hit%255] = key;

                        if(key_buffer[w->hit%255] != w->word[w->hit%255]){
                            g_gameplay = E_GAMEPLAY_MISS;
                        }else {
                            g_gameplay = E_GAMEPLAY_HIT;
                        }

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

    main_game_unload_sounds();

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

    if(bg_gameplay){
        al_destroy_bitmap(bg_gameplay);
    }

    particle_unset(&particle_mj);

    wordlist_unset(&sort_words);

#ifdef GAME_DATA_PACK
    PHYSFS_deinit();
#endif


    unload_window();

    return 0;
}


int sfx_load(sfx_t *sfx, const  char* filepath){
     sfx->sample = al_load_sample(filepath);

    if(!hippie_bgm.sample){
        return -1;
    }

    sfx->inst = al_create_sample_instance(sfx->sample);

    if(!al_attach_sample_instance_to_mixer(sfx->inst,mixer)){
        LOG("error: failed to  attach sfx instance to mixer");
    }
    return 0;

}

void main_game_init_sounds(void){

    voice = al_create_voice(44100, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_2);
    if(!voice){
        LOG("Error: Allegro Voice Failed");
    }

    mixer = al_create_mixer(44100, ALLEGRO_AUDIO_DEPTH_FLOAT32, ALLEGRO_CHANNEL_CONF_2);

    if(!mixer){
        LOG("Error: Allegro Voice Failed");
    }

    if(!al_attach_mixer_to_voice(mixer,voice)){
         LOG("Error: Allegro trying to attach mixer to voice");
    }

    al_set_default_mixer(mixer);
    al_reserve_samples(4);


    if(sfx_load(&hippie_bgm, "assets//bgm//hippie_loop.ogg") < 0){
        LOG("BGM hippie loop not loaded");
    }

    if(sfx_load(&round_bgm, "assets//bgm//hippie_cue.ogg") < 0){
        LOG("BGM hippie cue not loaded");
    }


    if(sfx_load(&hit_sfx, "assets//sfx//hit_word.wav") < 0){
        LOG("BGM Hit not loaded");
    }


    if(sfx_load(&miss_sfx, "assets//sfx//miss_word.wav") < 0){
        LOG("BGM Miss Word not loaded");
    }

    if(sfx_load(&lost_sfx,"assets//sfx//lost.wav") < 0 ){
         LOG("BGM Lost not loaded");
    }

    if(sfx_load(&reset_sfx,"assets//sfx//reset.wav") < 0 ){
         LOG("BGM Lost not loaded");
    }


}
void main_game_unload_sounds(void){

        sfx_destroy(&hippie_bgm);
        sfx_destroy(&round_bgm);
        sfx_destroy(&hit_sfx);
}


void sfx_play(const sfx_t *sfx, float vol, float pan, float speed, ALLEGRO_PLAYMODE mode){

    al_set_sample_instance_gain(sfx->inst, vol);
    al_set_sample_instance_pan(sfx->inst, pan);
    al_set_sample_instance_speed(sfx->inst, speed);
    al_set_sample_instance_playmode(sfx->inst, mode);
    al_play_sample_instance(sfx->inst);
}
void sfx_destroy(sfx_t *sfx){

    if(sfx){
        if(sfx->sample)  al_destroy_sample(sfx->sample);
        if(sfx->inst) al_destroy_sample_instance(sfx->inst);
    }

}

void sfx_stop_all(void){
    al_stop_samples();
}

void sfx_stop(sfx_t *sfx){
    if(sfx->inst){
        al_stop_sample_instance(sfx->inst);
    }
}
