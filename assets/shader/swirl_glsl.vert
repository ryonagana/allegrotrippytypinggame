#ifdef GL_ES
precision mediump float;
#endif


void main()
{
    gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;
}
