#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out VS_OUT
{
	vec3 vNormal;
} vs_out;

uniform mat4 model;
uniform mat4 view;

void main()
{
	gl_Position = view * model * vec4(aPos, 1.0);
	// ±‰ªªæÿ’Û
	mat3 normalMatrix = mat3(transpose(inverse(view * model)));
	vs_out.vNormal = normalize(vec3(vec4(normalMatrix * aNormal, 1.0)));
}