#version 150

uniform sampler2D ZBuf_input_tex;
in vec4 pos;
in vec2 oUv0;
out vec4 fragColour;

void main()
{
    // test first if the value is correctly sent by the FP

    //fragColour = vec4(0.5,0.1,0.1,1.0);

    // get the z composant
    fragColour = vec4 (vec3(pos.z),1.0);
}
