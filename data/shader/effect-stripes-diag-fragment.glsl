uniform sampler2D uni_tex;
uniform float uni_time;
uniform vec2 uni_width;
varying vec2 tex_pos;

void main()
{
    vec2 norm = tex_pos / (uni_width * 2.0);
    float value = mod(norm.x + norm.y + uni_time, 1.0);
    if (value < 0.5)
        gl_FragColor = vec4(0.95, 0.95, 0.95, 1.0);
    else
        gl_FragColor = texture2D(uni_tex, tex_pos);
}
