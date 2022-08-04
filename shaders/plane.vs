#version 330 core
layout(location = 0) in vec3 vertexPosition_modelspace;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec3 FragPos;

void main()
{
    FragPos = vec3(model * vec4(vertexPosition_modelspace, 1.0));
    gl_Position = projection * view * vec4(FragPos, 1.0);
}