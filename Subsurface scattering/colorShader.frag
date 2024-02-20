//
//  Created by Song Jungeun on 2024/1/4
//

#version 150 core

uniform sampler2D diffTex;

in vec2 texCoords;

out vec4 out_Color;


void main(void)
{
	// color
	vec4 color = texture(diffTex, texCoords);
	color.rgb = pow(color.rgb, vec3(2.2)); // gamma correction (to linear space)


	// final
	out_Color = vec4(color.rgb, 1); 
}
