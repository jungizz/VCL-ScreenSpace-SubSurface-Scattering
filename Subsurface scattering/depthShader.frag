//
//  Created by Song Jungeun on 2024/1/4
//

#version 150 core

//out vec4 out_Color;
out vec4 fragDepth;

float near = 0.1; 
float far  = 10.0;

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0;
    return (2.0 * near * far) / (far + near - z * (far - near));	
}

void main(void)
{
	//out_Color = vec4(vec3(gl_FragCoord.z), 1);

	//fragDepth = gl_FragCoord.z;

	fragDepth = vec4(vec3(LinearizeDepth(gl_FragCoord.z) / far), 1);
}
