#version 150 core

out vec4 out_Color;
//uniform vec3 lightPosition;
//uniform vec3 lightColor;
//uniform float shininess;
//uniform vec3 cameraPosition;
//uniform vec3 ambientLight;

uniform sampler2D diffTex;
uniform sampler2D normTex;
uniform sampler2D roughTex;
uniform sampler2D specTex;

in vec3 normal;
in vec3 worldPosition;
in vec2 texCoords;


const float PI = 3.14159265358979f;

void main(void)
{
	//vec3 l = lightPosition - worldPosition; //vector to light
	//vec3 L = normalize(l); //light direction
	//vec3 N = normalize(normal);
	//vec3 V = normalize(cameraPosition - worldPosition); // to camera
	//vec3 R = 2 * dot(L, N) * N - L;
	//vec3 I = lightColor / dot(l, l);
	//vec3 c = color.rgb * max(0, dot(L,N)) * I + color.rgb * ambientLight;
	//c+= pow(max(0, dot(R, V)), shininess) * I;

	vec4 c4 = texture(diffTex, texCoords);
	//c4 *= 1.0/PI;

	out_Color = c4;
}
