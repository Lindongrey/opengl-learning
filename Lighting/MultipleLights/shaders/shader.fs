#version 330 core

// 包含在顶点信息里的法向量
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCrood;

out vec4 FragColor;

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

// 平行光
struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform DirLight dirLight;

// 点光源
struct PointLight {
	vec3 position;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];

// 聚光
struct SpotLight {
	vec3 position;
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;

	float cutOff;
	float outerCutOff;
};
uniform SpotLight spotLight;

// 摄像机位置
uniform vec3 cameraPos;
// 材质
uniform Material material;
// 反射强度
uniform Light light;

vec3 CalcDirLight(DirLight dirLight, vec3 norm, vec3 viewDir)
{
	vec3 ambient = dirLight.ambient * vec3(texture(material.diffuse, TexCrood));

	vec3 lightDir = normalize(-dirLight.direction);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = dirLight.diffuse * diff * vec3(texture(material.diffuse, TexCrood));

	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = dirLight.specular * spec * vec3(texture(material.specular, TexCrood));

	vec3 result = ambient + diffuse + specular;
	return result;
}

vec3 CalcPointLight(PointLight pointLight, vec3 norm, vec3 viewDir, vec3 FragPos)
{
	vec3 ambient = pointLight.ambient * vec3(texture(material.diffuse, TexCrood));

	vec3 lightDir = normalize(pointLight.position - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = pointLight.diffuse * diff * vec3(texture(material.diffuse, TexCrood));

	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = pointLight.specular * spec * vec3(texture(material.specular, TexCrood));

	float distance = length(pointLight.position - FragPos);
	float attenuation = 1.0f / (pointLight.constant + pointLight.linear * distance + pointLight.quadratic * distance * distance);
	diffuse *= attenuation;
	specular *= attenuation;

	vec3 result = ambient + diffuse + specular;
	return result;
}

vec3 CalcSpotLight(SpotLight spotLight, vec3 norm, vec3 viewDir, vec3 FragPos)
{
	vec3 ambient = spotLight.ambient * vec3(texture(material.diffuse, TexCrood));

	vec3 lightDir = normalize(spotLight.position - FragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = spotLight.diffuse * diff * vec3(texture(material.diffuse, TexCrood));

	vec3 reflectDir = reflect(-lightDir, norm);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = spotLight.specular * spec * vec3(texture(material.specular, TexCrood));

	// 软化边缘
	float theta = dot(lightDir, normalize(-spotLight.direction));
	float epsilon = (spotLight.cutOff - spotLight.outerCutOff);
    float intensity = clamp((theta - spotLight.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse  *= intensity;
    specular *= intensity;

	float distance = length(spotLight.position - FragPos);
	float attenuation = 1.0f / (spotLight.constant + spotLight.linear * distance + spotLight.quadratic * distance * distance);
	diffuse *= attenuation;
	specular *= attenuation;

	vec3 result = ambient + diffuse + specular;
	return result;
}

void main () {
	vec3 viewDir = normalize(cameraPos - FragPos);
	vec3 norm = normalize(Normal);
	vec3 result;

	// 计算平行光的结果
	result += CalcDirLight(dirLight, norm, viewDir);
	// 计算点光源的结果
	for (int i = 0; i < NR_POINT_LIGHTS; i++)
	{
		result += CalcPointLight(pointLights[i], norm, viewDir, FragPos);
	}
	// 计算聚光的效果
	result += CalcSpotLight(spotLight, norm, viewDir, FragPos);

	FragColor = vec4(result, 1.0);
}