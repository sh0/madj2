uniform vec3 view_pos;
uniform vec2 view_size;
attribute vec2 vec_pos;
varying vec2 tex_pos;

void main()
{
    gl_Position = vec4(
        (vec_pos.x * view_size.x) + view_pos.x,
        (vec_pos.y * view_size.y) + view_pos.y,
        view_pos.z, 1.0
    );
    tex_pos = vec_pos;
}
