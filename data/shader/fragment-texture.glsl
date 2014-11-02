uniform sampler2D uni_tex;
varying vec2 tex_pos;

void main()
{
    gl_FragColor = texture2D(uni_tex, tex_pos);
}
