#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 tex_coord;
layout(location = 2) in vec4 color;

out vec4 v_color;
out vec2 v_tex_coord;

void main()
{
    gl_Position = vec4(position, 1.0f);
    v_color = color;
    v_tex_coord = tex_coord;
}
