// Light Structures

struct directional {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct point {
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 attenuation;
};

struct spot {
    vec3 position;
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 attenuation;
    float cutoff;
    float exponent;
};

// Light Variables

uniform directional dlights[10];
uniform int num_dlights;

uniform point plights[10];
uniform int num_plights;

uniform spot slights[10];
uniform int num_slights;

// Directional shading

void directional_shade(directional light, inout vec3 ambient, inout vec3 diffuse, inout vec3 specular, float shininess, vec3 vertex, vec3 normal){
    
    float pf = 0.0; // power factor
    vec3 eye = -vertex;
    vec3 VP = normalize(light.direction);
    vec3 halfVector = normalize(VP + eye);
    float nDotVP = max(0.0, dot(normal, VP));
    float nDotHV = max(0.0, dot(normal, halfVector));
    if (nDotVP > 0.0)
        pf = pow(nDotVP, shininess);
    ambient  += light.ambient;
    diffuse  += light.diffuse * nDotVP;
    specular += light.specular * pf;
}

// Point Shading

void point_shade(point light, inout vec3 ambient, inout vec3 diffuse, inout vec3 specular, float shininess , vec3 vertex, vec3 normal) {
    
    float pf = 0.0;                     // power factor
    vec3 eye = -vertex;
    vec3 VP = light.position - vertex;  // direction from surface to light position
    float d = length(VP);               // distance from surface to light source
    VP = normalize(VP);                 // Normalize the vector from surface to light position
    attenuation = 1.0 / (light.attenuation[0] +
                         light.attenuation[1] * d +
                         light.attenuation[2] * d * d);
    float halfVector = normalize(VP + eye);             // direction of maximum highlights
    float nDotVP = max(0.0, dot(normal, VP));           // normal . light direction
    float nDotHV = max(0.0, dot(normal, halfVector));   // normal . light half vector
    if (nDotVP > 0.0) {
        pf = pow(nDotHV, shininess);
    }
    ambient += light.ambient * attenuation;
    diffuse += light.diffuse * nDotVP * attenuation;
    specular += light.specular * pf * attenuation;
}

// Spot Shading

void spot_shade(spot light, inout vec3 ambient, inout vec3 diffuse, inout vec3 specular, float shininess, vec3 vertex, vec3 normal) {
    
    float pf = 0.0; // power factor
    float spotAttenuation = 0.0; // spotlight attenuation factor
    vec3 eye = -vertex;
    vec3 VP = light.position - vertex;  // direction from surface to light position
    float d = length(VP);               // distance from surface to light source
    
    VP = normalize(VP);     // Normalize the vector from surface to light position
    float attenuation = 1.0 / (light.attenuation[0] +
                               light.attenuation[1] * d +
                               light.attenuation[2] * d * d);     // Compute attenuation
    
    // See if point on surface is inside cone of illumination
    float spotDot = dot(-VP, normalize(light.direction)); // cosine of angle between spotlight
    if (spotDot >= light.cutoff)
        spotAttenuation = pow(spotDot, light.exponent);
    // Combine the spotlight and distance attenuation.
    attenuation *= spotAttenuation;
    vec3 halfVector = normalize(VP + eye); // direction of maximum highlights
    float nDotVP = max(0.0, dot(normal, VP));
    float nDotHV = max(0.0, dot(normal, halfVector));
    if (nDotVP > 0.0)
        pf = pow(nDotHV, shininess);
    ambient += light.ambient * attenuation;
    diffuse += light.diffuse * nDotVP * attenuation;
    specular += light.specular * pf * attenuation;
}

// Iterate through lighting
vec4 lighting(vec3 emission, vec3 ambient, vec3 diffuse, vec3 specular, float shininess, vec3 vertex, vec3 normal) {
    
    vec3 light_ambient = vec3(0.0), light_diffuse = vec3(0.0), light_specular = vec3(0.0);
    
    for(int i = 0; i < num_plights; i++)
        point_shade(plights[i], light_ambient, light_diffuse, light_specular, shininess, vertex, normal);
    for(int i = 0; i < num_slights; i++)
        spot_shade(slights[i], light_ambient, light_diffuse, light_specular, shininess, vertex, normal);
    for(int i = 0; i < num_dlights; i++)
        directional_shade(dlights[i], light_ambient, light_diffuse, light_specular, shininess, vertex, normal);
    
    return vec4(clamp(emission + light_ambient*ambient + light_diffuse*diffuse + light_specular*specular, 0.0, 1.0), 1.0);
}



