#version 330 core

in vec4 position; // raw position in the model coord
in vec3 normal;   // raw normal in the model coord
in vec2 TexCoord; // texture coordinates

uniform mat4 modelview; // from model coord to eye coord
uniform mat4 view;      // from world coord to eye coord

// Material parameters
uniform vec4 ambient;
uniform vec4 diffuse;
uniform vec4 specular;
uniform vec4 emision;
uniform float shininess;

// Light source parameters
const int maximal_allowed_lights = 10;
uniform bool enablelighting;
uniform int nlights;
uniform vec4 lightpositions[ maximal_allowed_lights ];
uniform vec4 lightcolors[ maximal_allowed_lights ];

// Texture sampler 
uniform int texture_id;
//uniform sampler2D ourTexture;
uniform sampler2D texture0;
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;

uniform int is_particle;

// Output the frag color
out vec4 fragColor;


void main (void){
    if (!enablelighting){
        // Default normal coloring (you don't need to modify anything here)
        vec3 N = normalize(normal);
        //fragColor = vec4(0.5f*N + 0.5f , 1.0f);

        if (is_particle == 1) {
            fragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
        } else if (texture_id == 0) {
			fragColor = texture(texture0, TexCoord);
        } else if (texture_id == 1) {
			fragColor = texture(texture1, TexCoord);
        } else if (texture_id == 2) {
			fragColor = texture(texture2, TexCoord);
        } else {
            // test for UI elements 
			fragColor = texture(texture3, TexCoord);
            if (fragColor.w < 0.01) {
                discard;
            }
        }

        //fragColor = vec4(TexCoord.x, TexCoord.y, 0.0f, 1.0f);
    } else {
        
        // HW3: You will compute the lighting here.
        //fragColor = vec4(0.0f,0.0f,0.0f,0.0f);
        //fragColor = texture(ourTexture, TexCoord);
        if (texture_id == 0) {
			fragColor = texture(texture1, TexCoord);
        } else {
			fragColor = texture(texture2, TexCoord);
        }

        //make 3x3 matrix to transform normal
        mat3 a_modelview = mat3(modelview);
        a_modelview = transpose(inverse(a_modelview));
        vec3 n_eye_norm = normalize(a_modelview * normal);

        vec4 pos_eye = modelview * position;

        // avoid divide by zero
        vec3 v_eye_norm = normalize((pos_eye.w * vec3(0.0f,0.0f,0.0f)) - (1.0f * pos_eye.xyz));

        for(int i = 0; i < nlights; i++) {
            vec4 lightpos_eye = view * lightpositions[i];
            // avoid divide by zero
            vec3 l_eye_norm = normalize((pos_eye.w * lightpos_eye.xyz) - (lightpos_eye.w * pos_eye.xyz));
            float nl = dot(n_eye_norm, l_eye_norm);

            vec3 h_eye_norm = normalize(v_eye_norm + l_eye_norm);
            float nh = dot(n_eye_norm, h_eye_norm);

            vec4 coef = ambient + diffuse * max(nl,0) + specular * pow(max(nh,0), shininess);
            //component wise multiply lightcolors[i] and coef
            vec4 acum = vec4(coef[0] * lightcolors[i][0], coef[1] * lightcolors[i][1], coef[2] * lightcolors[i][2], coef[3] * lightcolors[i][3]);
            fragColor += acum;
        }
        fragColor += emision; 
    }
}
