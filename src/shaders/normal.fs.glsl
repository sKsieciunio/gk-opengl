#version 330 core
out vec4 FragColor;

in vec3 Normal;

void main()
{
    vec3 norm = normalize(Normal);
    FragColor = vec4(norm * 0.5 + 0.5, 1.0);
}
