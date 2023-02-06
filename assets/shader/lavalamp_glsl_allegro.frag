#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D al_tex;


uniform vec2 u_resolution;
uniform float u_time;
varying vec2 varying_texcoord;


float rand(vec2 co){
    return fract(sin(dot(co, vec2(12.9898, 78.233))) * 43758.5453);
}


void main(){
    vec2 uv =  (2.0 * gl_FragCoord.xy - u_resolution.xy) / u_resolution.y;


    vec3 ball[5];

    ball[0] = vec3(sin(u_time) * 0.2 + 0.2 * rand(vec2(0.0, 0.9)), cos(u_time) * 0.1 + 0.4, 0.1 );
    ball[1] = vec3(sin(u_time) * 0.01 + 0.1, cos(u_time) * 0.07 + 0.4, 0.1 );
    ball[2] = vec3(1.0,0.6,0.03);
    ball[3] = vec3(sin(u_time) * 0.1 + 0.4 + rand(vec2(0.0, 0.5)) , cos(u_time) * 0.01 + 0.4 + rand(vec2(0.0, 1.0)), 0.1 );
    ball[4] = vec3(sin(u_time) * 0.1 + 0.2  *  rand(vec2(0.0, 0.9)),cos(u_time) * 1.0 *  rand(vec2(0.0, 0.9)) ,.2);


    float f = 0.0;

    for(int i = 0; i < 5;i++){
        f += ball[i].z  / distance(ball[i].xy, uv);
    }

    f = clamp(f,0.0,1.0);
    f = f == 1.0 ? 1.0 : 0.0;


    float r = rand(vec2(0.0,1.0)) / f + 0.1;

    vec3 col = vec3(f * 0.8,f * 0.3,f  * 0.2);


    gl_FragColor =  vec4(col,1.0);
}
