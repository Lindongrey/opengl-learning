#version 330 core

// 材质对三种光的反射能力
struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

// 光照
struct Light {
	vec3 position;
	// 光的方向
	vec3 direction;

	// 三种光照的颜色
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	// 衰减的三个系数
	float constant;
	float linear;
	float quadratic;

	// 切光角
	float cutOff;
	// 外光切
	float outerCutOff;
};

// 包含在顶点信息里的法向量
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCrood;

out vec4 FragColor;

// 摄像机位置
uniform vec3 cameraPos;
// 材质
uniform Material material;
// 反射强度
uniform Light light;

void main () {
	// 是否在聚光范围内
	// 环境光照
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCrood));

	// 漫反射光照：计算片段法向量与光源向量的叉乘结果
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	// vec3 lightDir = normalize(-light.direction); // 这是平行光的全局光照
	// 光照的分量 * 颜色
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCrood));

	// 镜面反射
	// 反射强度
	vec3 viewDir = normalize(cameraPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	// 32 是反光度
	// 注意 pow 函数不是乘方
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCrood));

	// 软化边缘
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = (light.cutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse  *= intensity;
    specular *= intensity;

	// 光照衰减
	float distance = length(light.position - FragPos);
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * distance * distance);
	diffuse *= attenuation;
	specular *= attenuation;

	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result, 1.0);
}