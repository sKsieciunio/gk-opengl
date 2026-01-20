#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec3 FragPos;
in vec3 Normal;
in vec3 Albedo;

uniform bool useObjectColor;
uniform vec3 objectColor;

void main()
{
    // Store the fragment position vector in the first gbuffer texture
    gPosition = FragPos;
    
    // Also store the per-fragment normals into the gbuffer
    gNormal = normalize(Normal);
    
    // And the color per object
    gAlbedoSpec.rgb = useObjectColor ? objectColor : Albedo;
    
    // Store specular intensity in gAlbedoSpec's alpha component
    gAlbedoSpec.a = 1.0; 
}
