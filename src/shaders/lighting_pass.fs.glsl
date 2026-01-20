#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct Light {
    int type; // 0=Dir, 1=Point, 2=Spot
    vec3 position;  // VIEW SPACE
    vec3 direction; // VIEW SPACE
    vec3 color;
    
    float constant;
    float linear;
    float quadratic;
    
    float cutOff;
    float outerCutOff;
};

#define MAX_LIGHTS 8
uniform Light lights[MAX_LIGHTS];
uniform int numLights;

uniform bool fogEnabled;
uniform vec3 fogColor;
uniform float fogStart;
uniform float fogEnd;

vec3 CalcDirLight(Light light, vec3 normal, vec3 viewDir);
vec3 CalcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcSpotLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir);

void main()
{
    // Retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;
    
    // Optimization: Discard if normal is zero (background) - optional
    if(length(Normal) == 0.0) {
       // discard; // Or handle background
    }

    // In View Space, the viewer is at (0,0,0)
    vec3 viewPos = vec3(0.0);
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 norm = normalize(Normal);
    
    vec3 result = vec3(0.0);

    for(int i = 0; i < numLights; i++)
    {
        // Light positions/directions must be in View Space!
        if(lights[i].type == 0) // Directional
            result += CalcDirLight(lights[i], norm, viewDir);
        else if(lights[i].type == 1) // Point
            result += CalcPointLight(lights[i], norm, FragPos, viewDir);
        else if(lights[i].type == 2) // Spot
            result += CalcSpotLight(lights[i], norm, FragPos, viewDir);
    }

    vec3 lighting = result * Diffuse; 
    // Specular should be added separately if we want true Phong where spec is white, not tinted by albedo usually
    // But existing shader did: result * Color. The existing shader mixed ambient/diffuse/spec terms multiplied by light color, then multiplied SUM by ObjectColor.
    // Let's stick to the previous implementation logic: (ambient + diffuse + specular) * ObjectColor.
    // Wait, check previous logic: 
    // vec3 finalColor = result * (useObjectColor ? objectColor : FragColor);
    // And result was sum of (ambient + diffuse + specular).
    // So yes, specular color IS multiplied by Albedo. That's fine.

    // Apply Fog
    if (fogEnabled) {
        float distance = length(FragPos); // In View Space, distance is length of position vector (camera at 0,0,0)
        float fogFactor = (fogEnd - distance) / (fogEnd - fogStart);
        fogFactor = clamp(fogFactor, 0.0, 1.0);
        lighting = mix(fogColor, lighting, fogFactor);
    }
    
    FragColor = vec4(lighting, 1.0);
}

vec3 CalcDirLight(Light light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32); 
    
    vec3 ambient  = light.color * 0.1; 
    vec3 diffuse  = light.color * diff * 0.8;
    vec3 specular = light.color * spec * 1.0;
    return (ambient + diffuse + specular);
}

vec3 CalcPointLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
  			     light.quadratic * (distance * distance));    
    
    vec3 ambient  = light.color * 0.1;
    vec3 diffuse  = light.color * diff * 0.8;
    vec3 specular = light.color * spec * 1.0;
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

vec3 CalcSpotLight(Light light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse
    float diff = max(dot(normal, lightDir), 0.0);
    // specular
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutOff - light.outerCutOff;
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    
    vec3 ambient = light.color * 0.1;
    vec3 diffuse = light.color * diff * 0.8;
    vec3 specular = light.color * spec * 1.0;
    
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    
    return (ambient + diffuse + specular);
}
