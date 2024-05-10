//
//  Created by Song Jungeun on 2024/1/4
//

#version 150 core

uniform vec3 cameraPosition;
uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 ambientLight;
//uniform float shininess;

uniform sampler2D diffTex;
uniform sampler2D normTex;
uniform sampler2D roughTex;
uniform sampler2D specAOTex;

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

float Fd_Lambert(){
	return 1.0 / PI;
}

mat3 getTBN(vec3 N){
	vec3 Q1 = dFdx(worldPosition), Q2 = dFdy(worldPosition); // wourldpos 미분 (compute tangent, bitangent)
	vec2 st1 = dFdx(texCoords), st2 = dFdy(texCoords);		 // texCoord 미분 
	float D = st1.s*st2.t - st1.t*st2.s;
	return mat3(normalize((Q1*st2.t - Q2*st1.t)*D), 
				normalize((-Q1*st2.s + Q2*st1.s)*D), 
				N);
}

void main(void)
{
	vec3 L = lightPosition - worldPosition;				// light unit vector
	vec3 l = normalize(L);							    // light unit vector
	vec3 n = normalize(normal);							// normal unit vector
	vec3 v = normalize(cameraPosition - worldPosition); // view unit vector
	vec3 h = normalize(l+v);							// half unit vector

	// normal mapping
	mat3 TBN = getTBN(n);
	vec3 normVec = texture(normTex,texCoords).rgb*2-1; // [0, 1] -> [-1, 1]
	//n = normalize(TBN * normVec);


	float NoV = abs(dot(n, v)) + 1e-5;
	float NoL = clamp(dot(n, l), 0.0, 1.0);
	float NoH = clamp(dot(n, h), 0.0, 1.0);
	float LoH = clamp(dot(l, h), 0.0, 1.0);
	

	vec3 f0 = vec3(0.028, 0.028, 0.028); // 'skin' specular reflectance at normal incidnece angle
	float roughness = texture(roughTex, texCoords).r;
	roughness *= roughness; // remapping roughness (alpha)

	// 1. specular BRDF
	float D = D_GGX(NoH, NoH * roughness);
	float V = V_SmithGGXCorrelated(NoV, NoL, roughness);
	vec3 F = F_Schlick(LoH, f0);

	vec3 Fr = (D * V) * F;

	// 2. diffuse BRDF
	vec4 diffColor = texture(diffTex, texCoords);
	diffColor.rgb = pow(diffColor.rgb, vec3(2.2)); // gamma correction (to linear space)
	vec3 Fd = diffColor.rgb * Fd_Lambert();
	
	vec4 spec = texture(specAOTex, texCoords);

	// final
	vec3 c = (Fd + Fr) * (lightColor/dot(L, L)) * NoL;
	out_Color = vec4(pow(c, vec3(1/2.2)), diffColor.a); // gamma correction (to srgb)
}
