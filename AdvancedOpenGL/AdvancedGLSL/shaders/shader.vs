#version 330 core

layout (location = 0) in vec3 aPos;
// layout (location = 1) in vec2 aTexCoord;

/* out VS_OUT
{
	vec2 TexCoord;
} vs_out; */

layout (std140) uniform Matrices
{
	mat4 projection;
	mat4 view;
};

uniform mat4 model;

void main()
{
	// vs_out.TexCoord = aTexCoord;
	gl_Position = projection * view * model * vec4(aPos, 1.0);
	// gl_PointSize = gl_Position.z;
}