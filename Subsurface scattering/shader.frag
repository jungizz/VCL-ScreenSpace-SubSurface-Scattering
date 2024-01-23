#version 150 core

uniform vec3 cameraPosition;
uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 ambientLight;
//uniform float shininess;

uniform sampler2D diffTex;
uniform sampler2D normTex;
uniform sampler2D roughTex;
uniform sampler2D specTex;

in vec3 normal;
in vec3 worldPosition;
in vec2 texCoords;

out vec4 out_Color;

const float PI = 3.14159265358979f;

void main(void)
{
	vec3 l = lightPosition - worldPosition; // light vector
	vec3 L = normalize(l); // light unit vector
	vec3 N = normalize(normal); // normal unit vector

	vec3 V = normalize(cameraPosition - worldPosition); // view unit vector
	vec3 R = 2 * dot(L, N) * N - L;
	vec3 I = lightColor / dot(l, l);

	vec4 c4 = texture(diffTex, texCoords);
	c4 *= 1.0/PI; // diffuse BRDF
	vec3 c = c4.rgb * max(0, dot(L,N)) * I + c4.rgb * ambientLight;
	c += pow(max(0, dot(R, V)), 1000) * I;
	
	out_Color = vec4(pow(c, vec3(1 / 2.2)), 1);
}
