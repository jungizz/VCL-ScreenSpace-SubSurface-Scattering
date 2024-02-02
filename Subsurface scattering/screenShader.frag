//
//  Created by Song Jungeun on 2024/1/4
//

#version 150 core

uniform sampler2D colorTex;
uniform sampler2D depthTex;

uniform ivec2 size;
out vec4 out_Color;


void main(void)
{	
	//out_Color = texture(colorTex, gl_FragCoord.xy / size);
	out_Color = vec4(vec3(texture(depthTex, gl_FragCoord.xy / size).r)/2,1); // depthmap
}
