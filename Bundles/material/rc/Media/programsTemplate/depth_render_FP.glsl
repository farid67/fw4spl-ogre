#version 330

uniform float u_vpWidth;
uniform float u_vpHeight;

uniform sampler2D Mip_current;
in vec4 pos;
out vec4 fragColour;

void main()
{
    // normalisation
    vec2 texCoord = gl_FragCoord.xy/vec2(u_vpWidth,u_vpHeight);

    fragColour.rgb = vec3(texture (Mip_current,texCoord).r);


}
