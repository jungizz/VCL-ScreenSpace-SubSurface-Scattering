//
//  Created by Song Jungeun on 2024/1/4
//

#version 150 core

uniform sampler2D colorTex;
uniform sampler2D depthTex;
uniform vec2 size;

out vec4 out_Color;


const int kernelSize = 11;
const float sigma = 3.0;


void main(void)
{
	vec2 texelSize = 1.0/size;
	vec3 resColor = vec3(0.0);
	
	int ws = (kernelSize-1)/2;
	
	float wSum = 0;

	for(int dy=-ws; dy<=ws; dy++) for(int dx=-ws; dx<=ws; dx++)
	{
		// texCoords 대신 gl_FragCoord.xy/size 사용
		float xx =  gl_FragCoord.x / size.x + (dx * texelSize.x);
		float yy =  gl_FragCoord.y / size.y + (dy * texelSize.y);

		float w = exp(-(dx*dx+dy*dy)/(2.0 * sigma * sigma));
		wSum += w;
		resColor += w*texture(colorTex, vec2(xx, yy)).rgb;
	}
	
	out_Color = vec4(resColor/wSum, 1.0);
	//out_Color = texture(colorTex, gl_FragCoord.xy / size);
	//out_Color = vec4(vec3(texture(depthTex, gl_FragCoord.xy / size).r)/2,1); // depthmap
}
