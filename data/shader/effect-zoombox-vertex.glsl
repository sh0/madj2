#version 150

uniform vec4 u_framing;
in vec2 m_vecpos;
out vec2 m_texpos;

void main()
{
    gl_Position = vec4(m_vecpos, 0.0f, 1.0f);
    vec2 n = (m_vecpos + 1.0f) * 0.5f;
    m_texpos = vec2(
        u_framing.x + (u_framing.z - u_framing.x) * n.x,
        u_framing.y + (u_framing.w - u_framing.y) * n.y
    );
}
