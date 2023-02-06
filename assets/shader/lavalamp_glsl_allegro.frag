#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D al_tex;


uniform vec2 u_resolution;
uniform float u_time;
varying vec2 varying_texcoord;



void main(){

    vec2 coord = gl_FragCoord.xy/u_resolution.xy;

    float angle = atan(-coord.y + 0.25, coord.y - 0.50 ) * 0.1;
    float len = length(coord - vec2(0.5 + u_time/25.0,0.1 + u_time));

    vec3 col = vec3(0.0);

    col.r += cos(len * 10.0 + angle * 100.016 + u_time);
    col.g += sin(len * 10.0 + angle * 60.576 - u_time);
    col.b += cos(len * 10.0 + angle * 90.0 + u_time);


    gl_FragColor =  vec4(col,0.01);
}
