//
//  Created by Song Jungeun on 2024/1/4
//

#version 410 core

layout(location=0) in vec3 in_Position;
layout(location=2) in vec2 in_TexCoords;
out vec2 texCoords;

void main(void)
{
	gl_Position = vec4(in_Position, 1.0);
	texCoords = in_TexCoords;
}

