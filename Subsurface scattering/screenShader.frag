//
//  Created by Song Jungeun on 2024/1/4
//

#version 150 core

uniform sampler2D gaussianDiffTex;
uniform sampler2D specTex;
uniform vec2 size;

out vec4 out_Color;


void main(void)
{	
	vec3 d = texture(gaussianDiffTex, gl_FragCoord.xy / size).rgb;
	vec3 s = texture(specTex, gl_FragCoord.xy / size).rgb/2;
	out_Color = vec4(vec3(d+s), 1);

	//out_Color = vec4(vec3(texture(depthTex, gl_FragCoord.xy / size).r)/2 , 1); // depthmap
}
