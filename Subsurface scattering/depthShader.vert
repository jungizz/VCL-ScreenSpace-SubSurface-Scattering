//
//  Created by Song Jungeun on 2024/1/4
//

#version 410 core

layout(location=0) in vec3 in_Position;

uniform mat4 shadowMVP;

void main(void)
{
	gl_Position = shadowMVP * vec4(in_Position, 1.0);
}

