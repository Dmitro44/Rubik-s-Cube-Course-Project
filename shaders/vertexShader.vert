#version 330 core
layout (location = 0) in vec4 a_pos;
layout (location = 1) in vec3 a_color;
out vec3 our_color;
uniform mat4 mvp_matrix;
void main()
{
   gl_Position = mvp_matrix * a_pos;
   our_color = a_color;
};
