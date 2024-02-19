//
//  Created by Song Jungeun on 2024/1/4
//

#version 150 core

uniform sampler2D colorTex;
uniform sampler2D depthTex;
uniform vec2 size;

out vec4 out_Color;

vec2 kernel = {15, 15}; 
const float sigma = 5;
const float alpha = 11; // glbal SSS level
const float beta = 800; // how SSS varies with depth gradient

float n = 0.01;   // near
float f = 1.0; // far 

float LinearizeDepth(float dPri)
{
	float d = dPri * 2 - 1; // [0,1] -> [-1, 1]
	return (2 * n * f) / (n + f + d * (n - f));
}


void main(void)
{
	float depth = texture(depthTex, gl_FragCoord.xy / size).r; // [0,1] (가까울수록 0)
	float z = LinearizeDepth(depth); // camera coord depth

	vec2 texelSize = 1.0/size;
	vec3 resColor = vec3(0.0);

	int wy = (int(kernel.y)-1)/2;
	
	float wSum = 0;

	for(int dy=-wy; dy<=wy; dy++)
	{
		// texCoords 대신 gl_FragCoord.xy/size 사용
		float yy =  gl_FragCoord.y / size.y + (dy * texelSize.y);
	
		float w = exp(-(dy*dy)/(2.0 * sigma * sigma));
		wSum += w;
		resColor += w*texture(colorTex, vec2(gl_FragCoord.x/size.x, yy)).rgb;
	}
	resColor /= wSum;
	 
	out_Color = vec4(resColor, 1.0);
	//out_Color = vec4(pow(vec3(z), vec3(4)), 1.0); // camera coord depth test
	
}
