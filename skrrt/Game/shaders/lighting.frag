#version 330 core

in vec4 position; // raw position in the model coord
in vec3 normal;   // raw normal in the model coord
in vec2 TexCoord; // texture coordinates

uniform mat4 modelview; // from model coord to eye coord
uniform mat4 view;      // from world coord to eye coord

const float levels = 8.0f; // How many color levels when cel shading

/////////////////////////
// Material parameters //
/////////////////////////

struct Material {
    vec4 emission;
    float shininess;
};
uniform Material material;

// Texture sampler 
//uniform int texture_id;
//uniform sampler2D ourTexture;
/*
uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;
*/

uniform int is_particle;
uniform sampler2D texture_id;
uniform sampler2D specular_id;

/////////////////////////
// Lighting parameters //
/////////////////////////

uniform bool enablelighting; // if false shade everything black

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

// Output the frag color
out vec4 fragColor;

void main (void){
    //Shades everything in black.
    if (!enablelighting){
        vec3 N = normalize(normal);

        if (is_particle == 1) {
            fragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
        /*
        } else if (texture_id == 0) {
			fragColor = texture(texture0, TexCoord);
        } else if (texture_id == 1) {
			fragColor = texture(texture1, TexCoord);
        } else if (texture_id == 2) {
			fragColor = texture(texture2, TexCoord);
        */
        } else {
            // test for UI elements 
			fragColor = texture(texture_id, TexCoord);
            if (fragColor.w < 0.01) {
                discard;
            }
        }

    //Shades everything normally.
    } else {
        fragColor = vec4(0.0f,0.0f,0.0f,0.0f);

        vec4 texColor = texture(texture_id, TexCoord);
        vec4 specColor = texture(specular_id, TexCoord);

		if (texColor.w < 0.01) {
			discard;
		}

        //make 3x3 matrix to transform normal (needed for non-uniform transforms)
        mat3 a_modelview = mat3(modelview);
        a_modelview = transpose(inverse(a_modelview));
        vec3 n_eye_norm = normalize(a_modelview * normal);

        vec4 pos_eye = modelview * position;

        // avoid divide by zero
        vec3 v_eye_norm = normalize((pos_eye.w * vec3(0.0f,0.0f,0.0f)) - (1.0f * pos_eye.xyz));

		////////////////////////////
		// Calc Directional Light //
		////////////////////////////

        vec3 sunDirNorm = normalize(view * vec4(sun.direction,0.0f)).xyz; // Transform sun to eye coord

		// Calculate ambient
		vec4 acum = sun.ambient * texColor;

		// Calculate diffuse
        float sunNL = dot(n_eye_norm, sunDirNorm);
		acum += sun.diffuse * max(sunNL,0) * texColor;

		// Calculate specular
        float sunNH = dot(n_eye_norm, normalize(v_eye_norm + sunDirNorm));
		acum += sun.specular * pow(max(sunNH,0), material.shininess) * specColor;

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
            float lenFragLight = length(pointLights[i].position - (inverse(view) * pos_eye)); // inverse view matrix to get fragment world pos
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
            float lenFragLight = length(spotLights[i].position - (inverse(view) * pos_eye)); // inverse view matrix to get fragment world pos
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

        fragColor += material.emission; 

        // idk if this is needed
        //fragColor.w = 1.0f;

		// Apply cel shading
		//fragColor = floor(fragColor * levels) / levels;
    }
}
