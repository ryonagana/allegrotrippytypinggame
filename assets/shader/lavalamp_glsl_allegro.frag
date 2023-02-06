#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D al_tex;


uniform vec2 u_resolution;
uniform float u_time;
varying vec2 varying_texcoord;



void main(){

    vec2 coord = (gl_FragCoord.xy/u_resolution.xy);
    vec4 actual_texture = texture2D(al_tex, varying_texcoord);

    float angle = atan(-coord.y + 0.25, coord.y - 0.50 ) * 0.1;
    float len = length(coord - vec2(0.5 + u_time/100.0,0.25 + u_time));

    vec4 col = vec4(0.0);

    col.r += cos(len * 40 + angle * 70 + u_time);
    col.g += sin(len * 10 + angle * 30 - u_time);
    col.b += cos(len * 60 + angle * 90 + u_time);


    gl_FragColor =  col;
}
