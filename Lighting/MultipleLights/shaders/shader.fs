#version 330 core

// �����ڶ�����Ϣ��ķ�����
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCrood;

out vec4 FragColor;

// ���ʶ����ֹ�ķ�������
struct Material {
	sampler2D diffuse;
	sampler2D specular;
	float shininess;
};

// ����
struct Light {
	vec3 position;
	// ��ķ���
	vec3 direction;

	// ���ֹ��յ���ɫ
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	// ˥��������ϵ��
	float constant;
	float linear;
	float quadratic;

	// �й��
	float cutOff;
	// �����
	float outerCutOff;
};

// ƽ�й�
struct DirLight {
	vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform DirLight dirLight;

// ���Դ
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

// �۹�
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

// �����λ��
uniform vec3 cameraPos;
// ����
uniform Material material;
// ����ǿ��
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

	// ����Ե
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

	// ����ƽ�й�Ľ��
	result += CalcDirLight(dirLight, norm, viewDir);
	// ������Դ�Ľ��
	for (int i = 0; i < NR_POINT_LIGHTS; i++)
	{
		result += CalcPointLight(pointLights[i], norm, viewDir, FragPos);
	}
	// ����۹��Ч��
	result += CalcSpotLight(spotLight, norm, viewDir, FragPos);

	FragColor = vec4(result, 1.0);
}