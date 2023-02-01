#ifdef GL_ES
precision mediump float;
#endif

uniform  float  u_time;
uniform vec2   u_resolution;
uniform int u_frame;


vec3 lavalamp(vec2 coord){

    float angle = atan(-coord.y + 0.25, coord.y - 0.50 ) * 0.1;
    float len = length(coord - vec2(0.5 + u_time/100.0,0.25 + u_time));

    vec3 col = vec3(1.0);

    col.r += cos(len * 40 + angle * 70 + u_time);
    col.g += sin(len * 10 + angle * 30 + u_time);
    col.b += cos(len * 60 + angle * 90 + u_time);

    return col;
}


void main(){
    vec2 coord = (gl_FragCoord.xy / u_resolution);



    gl_FragColor = vec4(lavalamp(coord),0.01);
}
