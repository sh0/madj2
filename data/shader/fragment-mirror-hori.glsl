uniform sampler2D uni_tex;
uniform float uni_flip;
varying vec2 tex_pos;

void main()
{
    float pos_y = abs(tex_pos.y - 0.5);
    gl_FragColor = texture2D(
        uni_tex,
        vec2(tex_pos.x, 0.5 - (uni_flip * pos_y))
    );
}
