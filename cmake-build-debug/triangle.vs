#version 120

attribute vec3 a_position;
attribute vec3 a_color;

varying vec4 v_color;

uniform float u_time;

uniform mat4 u_rotationMatrix;
uniform mat4 u_projectionMatrix;

void main(void)
{
    gl_Position = u_projectionMatrix *
                    u_rotationMatrix *
                    vec4(a_position, 1.0);
    v_color = vec4(a_color, 1.0);
}