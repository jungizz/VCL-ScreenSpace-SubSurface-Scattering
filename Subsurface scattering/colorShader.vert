//
//  Created by Song Jungeun on 2024/1/4
//

#version 410 core

layout(location=0) in vec3 in_Position;
layout(location=1) in vec3 in_Normal;
layout(location=2) in vec2 in_TexCoords;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projMat;

out vec2 texCoords;

void main(void)
{
	vec4 p = vec4(in_Position.xyz, 1);
	p = projMat * viewMat * modelMat * p;
	gl_Position= p;

    texCoords = in_TexCoords;
}

