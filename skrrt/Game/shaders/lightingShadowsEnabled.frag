#version 420 core

in vec4 position; // raw position in the model coord
in vec3 normal;   // raw normal in the model coord
in vec2 TexCoord; // texture coordinates

uniform mat4 modelview; // from model coord to eye coord
uniform mat4 view;      // from world coord to eye coord

/////////////////////////
// Material parameters //
/////////////////////////

struct Material {
    float shininess;
};
uniform Material material;

uniform sampler2D texture_id;
uniform sampler2D specular_id;
uniform sampler2D emission_id;

/////////////////////////
// Lighting parameters //
/////////////////////////
struct DirectionalLight {
    // The direction vector the light shines in
    vec3 direction;

    //How the light looks (color, shininess, ...)
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

uniform DirectionalLight sun;

struct PointLight {
    // Position of the light in world space
    vec4 position;

    // Attenuation (How the light fades with distance)
    // for no attenuation, set to (1.0f, 0.0f, 0.0f)
    float constant;
    float linear;
    float quadradic;

    // How the light looks (color, shininess, ...)
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

const int maxNumPointLights = 10;
uniform int numPointLights;
uniform PointLight pointLights[maxNumPointLights];

struct SpotLight {
    // Position of the light in world space
    vec4 position;

    //vector that points from position in the direction of the SpotLight
    vec3 direction; 

    //Full brightness if inside inner cutoff, lerp decreasing brightness between inner and outer, no brightness outside of outer.
    // Write as cos(angle), inner less than outer. i.e glm::cos(glm::radians(angle in degrees));
    float innerCutoff;
    float outerCutoff;


    // Attenuation (How the light fades with distance)
    // for no attenuation, set to (1.0f, 0.0f, 0.0f)
    float constant;
    float linear;
    float quadradic;

    // How the light looks (color, shininess, ...)
    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

const int maxNumSpotLights = 10;
uniform int numSpotLights;
uniform SpotLight spotLights[maxNumSpotLights];

//Camera
uniform vec3 viewPos;
uniform float farPlane;

//Shadow maps
layout (std140, binding = 0) uniform LightSpaceMatrices
{
    mat4 lightSpaceMatrices[16];
};
uniform float cascadePlaneDistances[16];
uniform int cascadeCount;

uniform sampler2DArray directionalDepthMap;
uniform sampler2D spotDepthMaps[maxNumSpotLights];
uniform sampler2D pointDepthMaps[maxNumPointLights];

const float minShadowBias = 0.005f;
const float maxShadowBias = 0.01f;
const float biasModifier = 0.5f;

const int radius = 1;

// Output the frag color
out vec4 fragColor;

float ShadowCalculation(vec4 fragPosViewSpace, vec4 fragPosWorldSpace, float shadowBias) {
    // select cascade layer
    float depthValue = abs(fragPosViewSpace.z);

    int layer = cascadeCount;
    for (int i = 0; i < cascadeCount; ++i) {
        if (depthValue < cascadePlaneDistances[i]) {
            layer = i;
            break;
        }
    }

    vec4 fragPosLightSpace = lightSpaceMatrices[layer] * fragPosWorldSpace;
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5f + 0.5f;

    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;

    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    if (currentDepth > 1.0f) {
		// Invert shadow for easier math
        return 1.0f - 0.0f;
    }
    // calculate bias (based on depth map resolution and slope)
    if (layer == cascadeCount) {
        shadowBias *= 1 / (farPlane * biasModifier);
    } else {
        shadowBias *= 1 / (cascadePlaneDistances[layer] * biasModifier);
    }

    // PCF
    float shadow = 0.0f;
    vec2 texelSize = 1.0f / vec2(textureSize(directionalDepthMap, 0));
    for(int x = -radius; x <= radius; ++x) {
        for(int y = -radius; y <= radius; ++y) {
            float pcfDepth = texture(directionalDepthMap, vec3(projCoords.xy + vec2(x, y) * texelSize, layer)).r;
            shadow += (currentDepth - shadowBias) > pcfDepth ? 1.0f : 0.0f;        
        }    
    }
	shadow /= (2.0f * radius + 1.0f) * (2.0f * radius + 1.0f); 
        
	// Invert shadow for easier math
    return 1.0f - shadow;
}


float OLDShadowCalculation(vec4 fragPosLightSpace, float bias, sampler2D shadowMap) {
    // transform to [0,1] range
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5f + 0.5f;
	// In light space, xy corresponds to the depth map, z is the depth
    float currentDepth = projCoords.z;

	// 0 means no shadow, 1 means full shadow
	float shadow = 0.0;
	//average shadow over a group of pixels
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(int x = -radius; x <= radius; ++x) {
		for(int y = -radius; y <= radius; ++y) {
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;        
		}    
	}
	// number of pixels we average over, must be a float
	shadow /= (2.0f * radius + 1.0f) * (2.0f * radius + 1.0f); 


	// Don't change color if out of range
	if(currentDepth > 1.0f) {
		shadow = 0.0f;
	}

	// Need to inverse shadow
    return 1.0f - shadow;
}
void main (void){
	fragColor = vec4(0.0f,0.0f,0.0f,0.0f);

	vec4 texColor = texture(texture_id, TexCoord);
	vec4 specColor = texture(specular_id, TexCoord);
	vec4 emisColor = texture(emission_id, TexCoord);

	//make 3x3 matrix to transform normal (needed for non-uniform transforms)
	mat3 a_modelview = mat3(modelview);
	a_modelview = transpose(inverse(a_modelview));
	vec3 n_eye_norm = normalize(a_modelview * normal);

	vec4 pos_eye = modelview * position;
	vec4 pos_world = inverse(view) * pos_eye;

	// avoid divide by zero
	vec3 v_eye_norm = normalize((pos_eye.w * vec3(0.0f,0.0f,0.0f)) - (1.0f * pos_eye.xyz));

	////////////////////////////
	// Calc Directional Light //
	////////////////////////////
	vec3 sunDirNorm = normalize(view * vec4(sun.direction,0.0f)).xyz; // Transform sun to eye coord
	float shadowBias = max(maxShadowBias * (1.0f - dot(sunDirNorm, n_eye_norm)), minShadowBias);
	float shadow = ShadowCalculation(pos_eye, pos_world, shadowBias);

	// Calculate ambient
	vec4 acum = sun.ambient * texColor;

	// Calculate diffuse
	float sunNL = dot(n_eye_norm, sunDirNorm);
	acum += shadow * (sun.diffuse * max(sunNL,0) * texColor);

	// Calculate specular
	float sunNH = dot(n_eye_norm, normalize(v_eye_norm + sunDirNorm));
	acum += shadow * (sun.specular * pow(max(sunNH,0), material.shininess) * specColor);

	fragColor += acum;

	///////////////////////
	// Calc Point Lights //
	///////////////////////
	for(int i = 0; i < numPointLights; i++) {
		vec4 lightpos_eye = view * pointLights[i].position;
		// avoid divide by zero
		vec3 l_eye_norm = normalize((pos_eye.w * lightpos_eye.xyz) - (lightpos_eye.w * pos_eye.xyz));
		float nl = dot(n_eye_norm, l_eye_norm);

		vec3 h_eye_norm = normalize(v_eye_norm + l_eye_norm);
		float nh = dot(n_eye_norm, h_eye_norm);

		// Calculate ambient
		vec4 acum = pointLights[i].ambient * texColor;

		// Calculate diffuse
		acum += pointLights[i].diffuse * max(nl,0) * texColor;

		// Calculate specular
		acum += (pointLights[i].specular * specColor) * pow(max(nh,0), material.shininess);

		// Calculate attenuation
		float lenFragLight = length(pointLights[i].position - pos_world); // inverse view matrix to get fragment world pos
		float attenuation = 1.0f / (pointLights[i].constant + pointLights[i].linear * lenFragLight + pointLights[i].quadradic * lenFragLight * lenFragLight);
		acum *= attenuation;

		fragColor += acum;
	}

	//////////////////////
	// Calc Spot Lights //
	//////////////////////
	for(int i = 0; i < numSpotLights; i++) {
		vec4 lightpos_eye = view * spotLights[i].position;
		// avoid divide by zero
		vec3 l_eye_norm = normalize((pos_eye.w * lightpos_eye.xyz) - (lightpos_eye.w * pos_eye.xyz));
		float nl = dot(n_eye_norm, l_eye_norm);

		vec3 h_eye_norm = normalize(v_eye_norm + l_eye_norm);
		float nh = dot(n_eye_norm, h_eye_norm);

		// Calculate ambient
		vec4 acum = spotLights[i].ambient * texColor;

		// Calculate diffuse
		acum += spotLights[i].diffuse * max(nl,0) * texColor;

		// Calculate specular
		acum += (spotLights[i].specular * specColor) * pow(max(nh,0), material.shininess);

		// Calculate attenuation
		float lenFragLight = length(spotLights[i].position - pos_world); // inverse view matrix to get fragment world pos
		float attenuation = 1.0f / (spotLights[i].constant + spotLights[i].linear * lenFragLight + spotLights[i].quadradic * lenFragLight * lenFragLight);
		acum *= attenuation;

		// Check if frag is in the spotlight
		vec3 spotlightDirView = normalize((view * vec4(-1.0f * spotLights[i].direction,0.0f)).xyz);
		float angleLightFrag = dot(l_eye_norm, spotlightDirView);
		float intensity = (angleLightFrag - spotLights[i].outerCutoff) / (spotLights[i].innerCutoff - spotLights[i].outerCutoff);
		intensity = clamp(intensity, 0.0f, 1.0f);
		acum *= intensity;

		fragColor += acum;
	}

	fragColor += emisColor; 

	fragColor.w = texColor.w;
}
