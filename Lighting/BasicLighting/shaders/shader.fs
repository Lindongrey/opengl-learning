#version 330 core
// �����ڶ�����Ϣ��ķ�����
in vec3 Normal;
in vec3 FragPos;

out vec4 FragColor;

uniform vec3 objectColor;
uniform vec3 lightColor;
// ��Դλ��
uniform vec3 lightPos;
// �����λ��
uniform vec3 cameraPos;

void main () {
	// �������գ�ǿ�� * ��ɫ
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;

	// ��������գ�����Ƭ�η��������Դ�����Ĳ�˽��
	vec3 norm = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);
	// ���յķ��� * ��ɫ
	float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

	// ���淴��
	// ����ǿ��
	float specularStrength = 0.5f;
	vec3 viewDir = normalize(cameraPos - FragPos);
	vec3 reflectDir = reflect(-lightDir, norm);
	// 32 �Ƿ����
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
	vec3 specular = specularStrength * spec * lightColor;

    vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}