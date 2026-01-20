#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;
out vec3 Albedo;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    vec4 viewPos4 = view * model * vec4(aPos, 1.0);
    FragPos = viewPos4.xyz; 
    
    Albedo = aColor; // Or sample from texture if enabled
    
    // Normal Matrix for View Space
    Normal = mat3(view * model) * aNormal; 
    
    gl_Position = projection * viewPos4;
}
