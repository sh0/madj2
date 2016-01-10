uniform sampler2D uni_tex;
uniform float uni_flip;
varying vec2 tex_pos;

void main()
{
    vec2 pos_xy = abs(tex_pos - 0.5);
    gl_FragColor = texture2D(uni_tex, 0.5 - (uni_flip * pos_xy));
}
