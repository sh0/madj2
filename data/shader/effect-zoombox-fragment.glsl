#version 150

uniform sampler2D u_texture;
in vec2 m_texpos;
out vec4 m_color;

void main()
{
    m_color = texture(u_texture, m_texpos);
}
