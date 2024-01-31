//
//  Created by Song Jungeun on 2024/1/4
//

#version 150 core

uniform sampler2D colorTex;
uniform int width;
uniform int height;
out vec4 out_Color;


void main(void)
{	
	out_Color = texture(colorTex, gl_FragCoord.xy / vec2(width, height));
}
