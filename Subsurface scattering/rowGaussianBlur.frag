//
//  Created by Song Jungeun on 2024/1/4
//

#version 150 core

uniform sampler2D colorTex;
uniform sampler2D depthTex;
uniform vec2 size;
uniform float screenWidth; // screen width in world coord

out vec4 out_Color;

const float variances[6] = {0.0064, 0.0484, 0.187, 0.567, 1.99, 7.41}; // m단위
const vec3  weights[6] = { vec3(0.233, 0.455, 0.649),
						   vec3(0.100, 0.336, 0.344), 
						   vec3(0.118, 0.198, 0.000),
					       vec3(0.113, 0.007, 0.007), 
						   vec3(0.358, 0.004, 0.000),
						   vec3(0.078, 0.000, 0.000) };

const float alpha = 11; // glbal SSS level
const float beta = 800; // how SSS varies with depth gradient

float n = 0.01; // near
float f = 10.0;  // far 

float LinearizeDepth(float dPri)
{
	float d = dPri * 2 - 1; // d[0,1] -> d[-1, 1]
	return (2 * n * f) / (n + f + d * (n - f)); // z[n, f]
}


void main(void)
{
	float depth = texture(depthTex, gl_FragCoord.xy / size).r; // d[0,1] (가까울수록 0)
	float z = LinearizeDepth(depth); // camera coord depth z[n, f]
	z = (z - n) / (f - n); // z[0,1]

	//kernel.x = 17;
	//kernel.x = mix(17, 3, z);

	vec2 texelSize = 1.0/size;
	
	vec3 wSum = vec3(0);
	vec3 resColor = vec3(0.0);

	for(int i=0; i<6; i++)
	{
		//float variance = variances[i];	
		float sigma = sqrt(variances[i]) * .001; // kernel size (mm단위를 m단위로 바꿈)
		float sigmaPri = (n * sigma) / z; // kernel size at screen in world coord
		sigmaPri /= screenWidth; // kernel size at screen in normal coord [0,1]
		float variance = sigmaPri * sigmaPri;
		
		int kernel = int(sigmaPri * size.x); // kernel size in pixel (sigma*2 -> kernel size)
		//int kernel = 7;

		vec3 weight = weights[i];


		//for(int dx=-sigma; dx<=sigma; dx++)
		for(int dx=-kernel; dx<=kernel; dx++)
		{
			float xx =  gl_FragCoord.x / size.x + (dx * texelSize.x);	
			vec3 w = weight * exp(-(dx*dx*.0000001)/(2.0 * variance)); // variance가 너무 작아서 분모에 *.0000001
			wSum += w;

			resColor += w * texture(colorTex, vec2(xx, gl_FragCoord.y/size.y)).rgb;
		}
	}
	resColor /= wSum;
	 
	out_Color = vec4(resColor, 1.0);
	//out_Color = vec4(vec3(z), 1.0); // camera coord depth test	
}