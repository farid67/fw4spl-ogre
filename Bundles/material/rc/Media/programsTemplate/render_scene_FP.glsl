#version 330


uniform float w;
uniform float h;
uniform sampler2D scene;

in vec2 uv;

out vec4 FragColor;

void main()
{
    vec2 texCoord = gl_FragCoord.xy / vec2(w,h);
    texCoord.y = abs(1.0 - texCoord.y);
    FragColor = texture(scene,texCoord) ;
}
