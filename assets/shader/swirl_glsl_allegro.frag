#ifdef GL_ES
precision mediump float;
#endif

uniform sampler2D al_tex;


uniform vec2 u_resolution;
uniform float u_time;

varying vec2 varying_texcoord;
void main(){

    vec2 coord = (gl_FragCoord/u_resolution.xy/2.0);
    vec3 color = vec3(.1);

    float angle = atan(-coord.y + 0.25, coord.x - 0.50) * 0.1;
    float len = length(coord - vec2(0.5, 0.25));


    vec4 actual_texture = texture2D(al_tex, varying_texcoord);

    color.r += sin(len * 40.0 + angle * 40.0 + u_time);
    //color.g += cos(len * 30.0 + angle * 120.0 - u_time);
    color.b += sin(len * 50.0 + angle * 100.0 + 3.0);

    //gl_FragColor = texture *  actual_texture * vec4(color,1);
    gl_FragColor  =  vec4(color, .5);
}
