#version 330 core
out vec4 FragColor;
in vec2 outtextureCoordinate;

uniform sampler2D floortexture;
void main()
{
FragColor = texture(floortexture, outtextureCoordinate);
}