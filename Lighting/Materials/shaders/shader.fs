#version 330 core

// 材质对三种光的反射能力
struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float shininess;
};

// 三种光的颜色
struct Light {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

// 包含在顶点信息里的法向量
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

// 摄像机位置
uniform vec3 cameraPos;
// 材质
uniform Material material;
// 反射强度
uniform Light light;

void main () {
	// 环境光照
	vec3 ambient = light.ambient * material.ambient;

	// 漫反射光照：计算片段法向量与光源向量的叉乘结果
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	// 光照的分量 * 颜色
	float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);

	// 镜面反射
	// 反射强度
	vec3 viewDir = normalize(cameraPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	// 32 是反光度
	// 注意 pow 函数不是乘方
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * (spec * material.specular);

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}