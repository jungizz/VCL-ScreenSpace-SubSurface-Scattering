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
	n = normalize(TBN * normVec);;


	float NoV = abs(dot(n, v)) + 1e-5;
	float NoL = clamp(dot(n, l), 0.0, 1.0);
	float NoH = clamp(dot(n, h), 0.0, 1.0);
	float LoH = clamp(dot(l, h), 0.0, 1.0);
	

	// diffuse BRDF
	vec4 diffColor = vec4(1); // color는 diffuse texture가 아닌 vec4(1) 사용
	vec3 Fd = diffColor.rgb * Fd_Lambert();
	
	//vec4 spec = texture(specAOTex, texCoords);

	// final
	vec3 c = Fd * (lightColor/dot(L, L)) * NoL;
	out_Color = vec4(c, 1);
}
