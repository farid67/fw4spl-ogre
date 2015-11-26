#version 150

uniform mat4 MVP;
in vec4 vertex;
out vec4 pos;

void main()
{
    gl_Position = MVP * vertex;
    pos = gl_Position;
}

