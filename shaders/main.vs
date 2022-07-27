#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 tex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
out vec2 TexCoords;
out vec3 FragPos;
out vec3 Normal;

void main(){
    TexCoords = tex;
    FragPos = vec3(model * vec4(vertexPosition_modelspace, 1.0));
    gl_Position = projection * view * vec4(FragPos, 1.0);
    Normal = mat3(transpose(inverse(model))) * normal; 
}