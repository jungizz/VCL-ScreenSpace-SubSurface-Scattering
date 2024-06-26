//
//  Created by Song Jungeun on 2024/1/4
//

#version 150 core

uniform vec3 cameraPosition;
uniform vec3 lightPosition;

uniform sampler2D diffTex;
uniform sampler2D roughTex;

uniform sampler2D gaussianDiffTex;
uniform sampler2D pass1Tex;

uniform vec3 specReflectance;
uniform vec2 size;
uniform int selectScene;

in vec3 normal;
in vec3 worldPosition;
in vec2 texCoords;

out vec4 out_Color;

const float PI = 3.14159265358979f;


float V_SmithGGXCorrelated(float NoV, float NoL, float roughness) {
    float a2 = roughness * roughness;
    float GGXL = NoV * sqrt((-NoL * a2 + NoL) * NoL + a2);
    float GGXV = NoL * sqrt((-NoV * a2 + NoV) * NoV + a2);
    return 0.5 / (GGXV + GGXL);
}

float D_GGX(float NoH, float roughness) {
    float a2 = roughness * roughness;
    float f = (NoH * a2 - NoH) * NoH + 1.0;
    return a2 / (PI * f * f);
}

vec3 F_Schlick(float u, vec3 f0) {
    return f0 + (vec3(1.0) - f0) * pow(1.0 - u, 5.0);
}



void main(void)
{
	vec3 L = lightPosition - worldPosition;				// light unit vector
	vec3 l = normalize(L);							    // light unit vector
	vec3 n = normalize(normal);							// normal unit vector
	vec3 v = normalize(cameraPosition - worldPosition); // view unit vector
	vec3 h = normalize(l+v);							// half unit vector


	float NoV = abs(dot(n, v)) + 1e-5;
	float NoL = clamp(dot(n, l), 0.0, 1.0);
	float NoH = clamp(dot(n, h), 0.0, 1.0);
	float LoH = clamp(dot(l, h), 0.0, 1.0);
	

	//vec3 f0 = vec3(0.028, 0.028, 0.028); // 'skin' specular reflectance at normal incidnece angle
	vec3 f0 = specReflectance;
	float roughness = texture(roughTex, texCoords).r;
	roughness *= roughness; // remapping roughness (alpha)

	// specular BRDF
	float D = D_GGX(NoH, roughness);
	float V = V_SmithGGXCorrelated(NoV, NoL, roughness);
	vec3 F = F_Schlick(LoH, f0);

	vec3 Fr = (D * V) * F;

	// diffuse BRDF with gaussian blur
	vec3 Fd = texture(gaussianDiffTex, gl_FragCoord.xy / size).rgb;

	// color
	vec4 color = texture(diffTex, texCoords);
	color.rgb = pow(color.rgb, vec3(2.2)); // gamma correction (to linear space)

	
	// final
	vec3 c;

	// 1. default (pass1 result)
	if(selectScene == 0){
		vec3 pass1diffTex = texture(pass1Tex, gl_FragCoord.xy / size).rgb;
		c = pow( pass1diffTex * color.rgb + Fr * NoL, vec3(1/2.2));
		//c = pass1diffTex;
	} 

	// 2. diffuse + specular (sssss result)
	else if(selectScene == 1){
		c = pow(Fd * color.rgb + Fr * NoL, vec3(1/2.2));
	}

	// 3. Gaussian on diffuse texture
	else if(selectScene == 2) c = Fd;

	out_Color = vec4(c, 1);
}
