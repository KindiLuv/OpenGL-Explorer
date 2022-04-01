#version 120

varying vec4 v_color;

uniform float u_time;

void main(void)
{

    gl_FragColor = v_color + vec4(mod(u_time, 1.0), vec3(0.0));
}