#version 330 core

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

// �����ڶ�����Ϣ��ķ�����
in vec3 Normal;
in vec3 FragPos;
in vec2 TexCrood;

out vec4 FragColor;

// �����λ��
uniform vec3 cameraPos;
// ����
uniform Material material;
// ����ǿ��
uniform Light light;

void main () {
	// �Ƿ��ھ۹ⷶΧ��
	// ��������
	vec3 ambient = light.ambient * vec3(texture(material.diffuse, TexCrood));

	// ��������գ�����Ƭ�η��������Դ�����Ĳ�˽��
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(light.position - FragPos);
	// vec3 lightDir = normalize(-light.direction); // ����ƽ�й��ȫ�ֹ���
	// ���յķ��� * ��ɫ
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 diffuse = light.diffuse * diff * vec3(texture(material.diffuse, TexCrood));

	// ���淴��
	// ����ǿ��
	vec3 viewDir = normalize(cameraPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	// 32 �Ƿ����
	// ע�� pow �������ǳ˷�
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
	vec3 specular = light.specular * spec * vec3(texture(material.specular, TexCrood));

	// ����Ե
	float theta = dot(lightDir, normalize(-light.direction));
	float epsilon = (light.cutOff - light.outerCutOff);
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);
    diffuse  *= intensity;
    specular *= intensity;

	// ����˥��
	float distance = length(light.position - FragPos);
	float attenuation = 1.0f / (light.constant + light.linear * distance + light.quadratic * distance * distance);
	diffuse *= attenuation;
	specular *= attenuation;

	vec3 result = ambient + diffuse + specular;
	FragColor = vec4(result, 1.0);
}