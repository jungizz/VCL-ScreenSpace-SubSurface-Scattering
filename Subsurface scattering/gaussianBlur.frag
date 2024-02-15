//
//  Created by Song Jungeun on 2024/1/4
//

#version 150 core

uniform sampler2D colorTex;
uniform sampler2D depthTex;
uniform vec2 size;

out vec4 out_Color;

vec2 kernel;// 1~15
const float sigma = 5;
const float alpha = 11; // glbal SSS level
const float beta = 800; // how SSS varies with depth gradient

void main(void)
{
	// depth에 따라 kernel 크기 조절하기 (어케하지~~~ 고민즁)
	float depth = texture(depthTex, gl_FragCoord.xy / size).r; //0~1 (가까울수록 0)

	//kernel.x = alpha / (depth + beta * abs(dFdx(depth) * depth));
	//kernel.y = alpha / (depth + beta * abs(dFdy(depth) * depth));

	kernel.x = 15;
	kernel.y = 15;

	vec2 texelSize = 1.0/size;
	vec3 resColor = vec3(0.0);
	
	int wx = (int(kernel.x)-1)/2;
	int wy = (int(kernel.y)-1)/2;
	
	float wSum = 0;

	for(int dy=-wy; dy<=wy; dy++) for(int dx=-wx; dx<=wx; dx++)
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
