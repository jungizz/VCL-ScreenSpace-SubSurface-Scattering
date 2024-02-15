//
//  Created by Song Jungeun on 2024/1/4
//

#version 150 core

uniform sampler2D specTex;
uniform sampler2D depthTex;
uniform vec2 size;

out vec4 out_Color;


void main(void)
{	
	vec3 s = texture(specTex, gl_FragCoord.xy / size).rgb;
	out_Color = vec4(s, 1);

	//out_Color = vec4(vec3(texture(depthTex, gl_FragCoord.xy / size).r)/2 , 1); // depthmap
}
