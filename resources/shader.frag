#version 430 core

#define N_POINT_LIGHTS 3  

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
    float transparency;
}; 
  
uniform Material material;

struct Point_Light {
        vec3 position;
        vec3 diffuse;
        vec3 specular;
        float constant;
        float linear;
        float quadratic;
};

struct DirLight {
    vec3 direction;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
uniform DirLight dirLight;
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);  
vec3 CalcPointLight(Point_Light light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcAmbient();

struct Camera
{
    vec3 position;
    vec3 direction;
};
uniform Camera camera;

uniform Point_Light pointLights[N_POINT_LIGHTS];

out vec4 FragColor;
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

uniform sampler2D tex0;

void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(camera.position - FragPos);

    vec3 result = CalcDirLight(dirLight, norm, viewDir);

    for(int i = 0; i < N_POINT_LIGHTS; i++)
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);    

    result += CalcAmbient();

    FragColor = vec4(result, material.transparency);
}

vec3 CalcAmbient(){
    return 0.01 * vec3(texture(tex0, TexCoords));
}

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // combine results
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(tex0, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(tex0, TexCoords));
    return (diffuse + specular);
}

vec3 CalcPointLight(Point_Light light, vec3 normal, vec3 fragPos, vec3 viewDir)
{   
    float constant=light.constant;
    float linear=light.linear;
    float quadratic=light.quadratic;
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (constant + linear * distance + 
  			     quadratic * (distance * distance));    
    // combine results
    vec3 diffuse  = light.diffuse * diff * vec3(texture(tex0, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(tex0, TexCoords));
    diffuse  *= attenuation;
    specular *= attenuation;
    return (diffuse + specular);
}