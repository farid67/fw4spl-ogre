#version 330

//uniform sampler2D MipMap_tex; // the MipMap texture get from the cpp
//in vec2 oUv0;
out vec4 fragColour;
in float zc;


void main()
{

    // MipMap version -> method to get the z value, depending on the mip level
//    fragColour.rgb = vec3(textureLod(MipMap_tex,oUv0,1).r);

    // normal Texture version -> use when getting mip0 from the last compositor
//    if (texture(RT,oUv0).r >  0.8 && texture(RT,oUv0).g > 0.8 )
//        fragColour = vec4 (0.5,0.1,0.1,1.0);

//    else
//        fragColour = texture(RT,oUv0);

    fragColour.rgb = vec3(zc);


}
