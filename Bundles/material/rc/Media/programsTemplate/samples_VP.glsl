#version 330

// PI value
const float PI = 3.141592653589793238462643383;
in vec4 vertex;
uniform sampler2D MipMap_tex; // MIP MAP texture
uniform mat4 MVP; // modelViewProj matrix
uniform float h; // height
uniform float w; // width
//uniform int s; // samples number
//uniform float r; // radius (spiral)
//uniform int gamma;
//uniform int MAX_MIP; // max mip level


//out vec2 c; // needed by the FP
//out vec2 nc; // normal
//out vec2 qi[s]; // samples point to compute the occlusion factor of c

// test
out float zc;


void main()
{
    // get the pixel position of the vertex
    gl_Position = MVP * vertex;

    // get the texture coordinate
    vec2 text_coord = gl_Position.xy/vec2(w,h);

    // we first compute the depth of the camera point
//    zc = textureLod(MipMap_tex,text_coord,0).r;

    zc = 1.0f;
}
