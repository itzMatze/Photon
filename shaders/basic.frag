#version 330 core

layout(location = 0) out vec4 f_color;

in vec4 v_color;
in vec2 v_tex_coord;
uniform sampler2D u_texture;


void main()
{
    vec4 tex_color = texture(u_texture, v_tex_coord);
    f_color = tex_color;// + v_color;
}