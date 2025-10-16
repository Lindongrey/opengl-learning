#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCrood;

out vec3 Normal;
// 片段的世界位置
out vec3 FragPos;
// 纹理坐标
out vec2 TexCrood;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main () {
	// 从右往左乘
	gl_Position = projection * view * model * vec4(aPos, 1.0f);
	Normal = mat3(transpose(inverse(model))) * aNormal;
	FragPos = vec3(model * vec4(aPos, 1.0f));
	TexCrood = aTexCrood;
}