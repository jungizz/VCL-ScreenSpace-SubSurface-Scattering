//
//  Created by Song Jungeun on 2024/1/4
//

#version 150 core

uniform sampler2D colorTex;
uniform vec2 size;

in vec2 texCoords;

out vec4 out_Color;


const int kernelSize = 11;
const float sigma = 5.0;


void main(void)
{
	vec2 texelSize = 1.0/size;
	vec3 resColor = vec3(0.0);
	
	int ws = (kernelSize-1)/2;
	
	float wSum = 0;

	for(int dy=-ws; dy<=ws; dy++) for(int dx=-ws; dx<=ws; dx++)
	{
		float xx = texCoords.x + (dx * texelSize.x);
		float yy = texCoords.y + (dy * texelSize.y);
	
		float w = exp(-(dx*dx+dy*dy)/(2.0 * sigma * sigma));
		wSum += w;
		resColor += w*texture(colorTex, vec2(xx, yy)).rgb;
	}
	resColor /= wSum;
	out_Color = vec4(resColor, 1.0);
	//out_Color = texture(colorTex, gl_FragCoord.xy / size);
}
