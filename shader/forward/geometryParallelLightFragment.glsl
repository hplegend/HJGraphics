#version 330 core
#define BLINN
#define PCF_SHADOW
const float gamma = 2.2;
out vec4 FragColor;
in vec3 tangentPos;
in vec3 worldPos;
in vec3 normal;
in vec2 texCoord;
in vec3 tangentCameraPos;
in vec3 tangentLightPos;
in vec3 tangentLightDirection;

struct Material{
    float ambientStrength;
    float diffuseStrength;
    float specularStrength;

    float shininess;
    float alpha;
    float reflective;
    float refractive;

    int diffuseMapNum;
    int specularMapNum;
    int normalMapNum;
    int heightMapNum;

    sampler2D diffuseMap;
    sampler2D specularMap;
    sampler2D normalMap;
    sampler2D heightMap;
};
//rendered object info
uniform mat4 model;
uniform Material material;

//light Info
uniform vec3 lightPosition;
uniform vec3 lightDirection;
uniform vec3 lightColor;
uniform mat4 lightSpaceMatrix;

uniform sampler2D shadowMap;
float parallelShadowCalculation(vec4 fragPosLightSpace);
vec3 parallelLight();
void main()
{

    vec3 Color=parallelLight();
    FragColor=vec4(Color,1.0f);
    FragColor.rgb = pow(FragColor.rgb, vec3(1.0/gamma));
}
//////////////////////////////////////////////////////////////////
float parallelShadowCalculation(vec4 fragPosLightSpace)
{
    // projection division
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1]
    projCoords = projCoords * 0.5 + 0.5;
    float currentDepth = projCoords.z;
    float bias=0.0005f/ fragPosLightSpace.w;// if we don't do divide to bias, then spotlight bias=0.0005f parallels bias = 0.005f
    #ifndef PCF_SHADOW
    //Original Version without PCF shadow
        float closestDepth = texture(shadowMap,projCoords.xy).r;
        float shadow = currentDepth - bias > closestDepth  ? 0.0 : 1.0;
        return shadow;
    #else
    //PCF shadow
        float shadow = 0.0f;
        vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
        for(int x = -2; x <= 2; ++x){
            for(int y = -2; y <= 2; ++y){
                float closestDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
                shadow += currentDepth - bias > closestDepth ? 0.0 : 1.0;
            }
        }
        shadow/= 25.0f;
        return shadow;
    #endif
}
vec3 parallelLight(){
    //default color
    vec3 diffuseSampler=vec3(1.0,1.0,1.0);
    vec3 specularSampler=vec3(1.0,1.0,1.0);
    if(material.diffuseMapNum>0){
        diffuseSampler=texture(material.diffuseMap,texCoord).rgb;
        diffuseSampler = pow(diffuseSampler, vec3(gamma));
    }
    if(material.specularMapNum>0){
        specularSampler=texture(material.specularMap,texCoord).rgb;
    }
    vec3 normalSampler=normal;
    if(material.normalMapNum>0){
        normalSampler=texture(material.normalMap,texCoord).rgb;
        normalSampler=normalize(2*normalSampler-1);
    }
    //calculate shadow
    vec4 lightSpacePos=lightSpaceMatrix*vec4(worldPos,1.0f);//replaced original code
    float shadowFactor=parallelShadowCalculation(lightSpacePos);
    //Diffuse
    vec3 lightDir=normalize(tangentLightDirection);
    float diff=max(dot(-lightDir,normalSampler),0.0);
    vec3 diffuse=diff * diffuseSampler * material.diffuseStrength * lightColor;
    //Specular
    vec3 viewDir=normalize(tangentCameraPos-tangentPos);
    #ifdef BLINN
    //blinn-phong
        vec3 halfwayDir=normalize(viewDir-lightDir);
        float spec=pow(max(dot(normalSampler,halfwayDir),0.0),material.shininess);
        vec3 specular=spec * specularSampler * material.specularStrength * lightColor;
    #else
    //phong
        vec3 reflectDir=reflect(lightDir,normalSampler);
        float spec=pow(max(dot(viewDir,reflectDir),0.0),material.shininess);
        vec3 specular=spec * specularSampler * material.specularStrength * lightColor;
    #endif
    return (diffuse+specular)*shadowFactor;
}
