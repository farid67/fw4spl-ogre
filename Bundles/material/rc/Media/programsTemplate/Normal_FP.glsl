#version 330

//uniform sampler2D MipMap_tex; // the MipMap texture get from the cpp
//in vec2 oUv0;
out vec4 fragColour;
in float zc;
in vec3 pos;

void main()
{

     vec3 n = normalize(cross(dFdy(pos.xyz),dFdx(pos.xyz)));


    fragColour.rgb = (n *0.5)+0.5;

//     fragColour =


}
