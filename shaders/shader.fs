#version 330 core
out vec4 FragColor;

in vec3 fragPos;
in vec3 normal;
in vec2 TexCoords;

struct PointLight{
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
const int NUM_POINT_LIGHT = 3;
uniform PointLight pointLight[NUM_POINT_LIGHT];

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    int shininess;
};
uniform Material material;

uniform vec3 viewPos;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir);

void main()
{    
    vec3 viewDir = normalize(viewPos - fragPos);
    vec3 result = vec3(0);
    for(int i = 0;i<NUM_POINT_LIGHT;i++){
       result += CalcPointLight(pointLight[i], normalize(normal), viewDir); 
    } 

    FragColor = vec4(result, 1.0);
    // FragColor = texture(material.diffuse,TexCoords);
}

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 viewDir){
    // 环境光
    vec3 ambient = light.ambient * texture(material.diffuse,TexCoords).rgb;

    // 漫反射
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(lightDir, normal),0);
    vec3 diffuse = diff * light.diffuse * texture(material.diffuse, TexCoords).rgb;

    // 镜面反射
    vec3 reflectDir = reflect(-lightDir,normal);
    float spec = pow(max(dot(reflectDir, viewDir),0),material.shininess);
    vec3 specular = spec * light.specular * texture(material.specular,TexCoords).rgb;

    // 光强衰减
    float distance = length(light.position - fragPos);
    float attenuation = 1.0/(light.constant + light.linear * distance + light.quadratic * (distance * distance));

    return attenuation * (ambient + diffuse + specular);
}