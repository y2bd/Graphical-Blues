#version 330 core
out vec4 color;

in VS_OUT {
    vec3 position;
    vec3 normal;
    vec2 texCoord;
    vec4 positionLightSpace;
} fs_in;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;

    sampler2D texture_diffuse1;
}; 

struct Light {
    vec3 position;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform vec3 cameraPos;
uniform sampler2DShadow shadowMap;

uniform Material material;
uniform Light light;

float calculateIfShadow(vec4 positionLightSpace)
{

    // dehomogonize
    vec3 projCoords = positionLightSpace.xyz / positionLightSpace.w;

    // clip into zbuf range
    projCoords = projCoords * 0.5 + 0.5;

    /*
    // get saved depth from light space
    float closestDepth = texture(shadowMap, projCoords.xy).r; 

    // get current depth from light space
    float currentDepth = projCoords.z;

    // check if we're at the right space

    vec3 lightDir = normalize(-light.position);
    float bias = max(0.05 * (1.0 - dot(fs_in.normal, lightDir)), 0.005);  
    float shadow = currentDepth - bias > closestDepth  ? 1.0 : 0.0;
    */

    vec3 lightDir = normalize(-light.position);
    float shadowBias = max(0.05 * (1.0 - dot(fs_in.normal, lightDir)), 0.005); ;
    
    //vec3 projCoordsBiased = vec3(projCoords.xy, projCoords.z - shadowBias);
    //float shadow = 1.0f - texture(shadowMap, projCoordsBiased);

    // pcf
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            vec3 samplePos = vec3(projCoords.xy + vec2(x, y) * texelSize, projCoords.z - shadowBias);
            float pcfDepth = texture(shadowMap, samplePos); 
            shadow += (pcfDepth);     
        }    
    }

    shadow = 1.0f - (0.5f + (shadow / 18.0f));

    if(projCoords.z > 1.0f)
        shadow = 0.0f;

    return shadow;
}  

void main()
{
    vec3 tex = texture(material.texture_diffuse1, fs_in.texCoord).rgb;

    // calc ambient
    float ambientStrength = 0.1f;
    vec3 ambient = light.ambient * tex;

    // get angle
    vec3 norm = normalize(fs_in.normal);
    vec3 lightDir = normalize(-light.position);

    // get diffuse from angle
    float diffAmt = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diffAmt * tex);

    // setup spec
    float specularStrength = 0.8f;
    vec3 viewDir = normalize(cameraPos - fs_in.position);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    // calc spec
    float shininess = 64;
    float specAmt = pow(max(dot(norm, halfwayDir), 0.0), material.shininess);
    vec3 specular = light.specular * (specAmt * material.specular);

    float shadow = calculateIfShadow(fs_in.positionLightSpace);
    vec3 lighting = (ambient + (1.0 - shadow) * (diffuse + specular)) * tex;

	color = vec4(lighting, 1.0f);
}