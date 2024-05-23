//
//  Created by Song Jungeun on 2024/1/4
//

#version 150 core

uniform sampler2D colorTex;
uniform sampler2D depthTex;
uniform vec2 size;
uniform float screenWidth; // screen width in world coord
uniform int isAdjKernel;
uniform int isEdgeDet;
uniform float kernelParam;

out vec4 out_Color;

const vec3 lum = vec3(0.2126, 0.7152, 0.0722);
const float edgeThreshold = 5.f;
const float variances[6] = {0.0064, 0.0484, 0.187, 0.567, 1.99, 7.41}; // mm단위
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
bool isEdge;

float LinearizeDepth(float dPri)
{
	float d = dPri * 2 - 1; // d[0,1] -> d[-1, 1]
	return (2 * n * f) / (n + f + d * (n - f)); // z[n, f]
}

float luminance(vec3 color)
{
	return dot(lum, color);
}

float edgeDetection()
{
	ivec2 pix = ivec2(gl_FragCoord.xy);

	float s00 = luminance(texelFetchOffset(colorTex, pix, 0, ivec2(-1,1)).rgb);
	float s10 = luminance(texelFetchOffset(colorTex, pix, 0, ivec2(-1,0)).rgb);
	float s20 = luminance(texelFetchOffset(colorTex, pix, 0, ivec2(-1,-1)).rgb);
	float s01 = luminance(texelFetchOffset(colorTex, pix, 0, ivec2(0,1)).rgb);
	float s21 = luminance(texelFetchOffset(colorTex, pix, 0, ivec2(0,-1)).rgb);
	float s02 = luminance(texelFetchOffset(colorTex, pix, 0, ivec2(1,1)).rgb);
	float s12 = luminance(texelFetchOffset(colorTex, pix, 0, ivec2(1,0)).rgb);
	float s22 = luminance(texelFetchOffset(colorTex, pix, 0, ivec2(1,-1)).rgb);

	float sx = s00 + 2 * s10 + s20 - (s02 + 2 * s12 + s22);
	float sy = s00 + 2 * s01 + s02 - (s20 + 2 * s21 + s22);

	float g = sx * sx + sy * sy;
	return g;
}


void main(void)
{
	if(edgeDetection() >= edgeThreshold) isEdge = true;

	float depth = texture(depthTex, gl_FragCoord.xy / size).r; // d[0,1] (가까울수록 0)
	float worldZ = LinearizeDepth(depth); // world coord linear depth z[n, f]
	float z = worldZ / f; //[0, 1]

	vec2 texelSize = 1.0/size;
	
	vec3 wSum = vec3(0);
	vec3 resColor = vec3(0.0);

	for(int i=0; i<weights.length(); i++)
	{
		//float variance = variances[i];	
		float sigma = sqrt(variances[i]) * .001; // kernel size (mm단위를 m단위로 바꿈)
		float sigmaPri = abs((n * sigma) / worldZ); // kernel size at screen in world coord
		sigmaPri /= screenWidth; // kernel size at screen in normal coord [0,1]
		sigmaPri *= size.x; //change to pixel coord
		float variance = sigmaPri * sigmaPri;
		
		float kernel = sigmaPri;
		//if(isAdjKernel==0) kernel = min(10,sigmaPri);
		//else if(isAdjKernel == 1) kernel = min(10, mix(sigmaPri*10, sigmaPri*0.1, z));
		kernel *= kernelParam;
		
 		//if(isEdgeDet==1 && isEdge) kernel = 1;

		vec3 weight = weights[i];
	
		for(float dx=-kernel; dx<=kernel; dx+=kernel/10.)
		{
			float xx =  gl_FragCoord.x / size.x + (dx * texelSize.x);	
			float depth = texture(depthTex, vec2(xx, gl_FragCoord.y / size.y)).r; // d[0,1] (가까울수록 0)
			float sampleZ = LinearizeDepth(depth); // world coord linear depth z[n, f]
			
			vec3 w = weight * exp(-(dx*dx)/(2.0 * variance)) * exp(-pow((sampleZ-worldZ), 2)/(0.01*0.01));
			wSum += w;
	
			resColor += w * texture(colorTex, vec2(xx, gl_FragCoord.y/size.y)).rgb;
		}
	}
	resColor /= wSum;	 
	out_Color = vec4(resColor, 1.0);
	//out_Color = vec4(vec3(z), 1); // camera coord depth test	
}