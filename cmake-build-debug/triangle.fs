#version 120

varying vec4 v_color;
varying vec2 v_texcoords;

uniform float u_time;
uniform sampler2D u_sampler;

void main(void)
{
    vec4 color = texture2D(u_sampler, v_texcoords);
    gl_FragColor = color;
}