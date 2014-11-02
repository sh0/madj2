attribute vec2 vec_pos;
varying vec2 tex_pos;

void main()
{
    gl_Position = vec4(
        vec_pos.x * 2.0 - 1.0,
        vec_pos.y * 2.0 - 1.0,
        0.0, 1.0
    );
    tex_pos = vec_pos;
}
