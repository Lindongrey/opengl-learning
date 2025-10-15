#version 330 core
// 包含在顶点信息里的法向量
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
// 光源位置
uniform vec3 lightPos;
// 摄像机位置
uniform vec3 cameraPos;

void main () {
	// 环境光照：强度 * 颜色
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;

	// 漫反射光照：计算片段法向量与光源向量的叉乘结果
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	// 光照的分量 * 颜色
	float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

	// 镜面反射
	// 反射强度
	float specularStrength = 0.5f;
	vec3 viewDir = normalize(cameraPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	// 32 是反光度
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}