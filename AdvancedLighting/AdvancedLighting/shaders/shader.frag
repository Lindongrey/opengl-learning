#version 330 core

out vec4 FragColor;

in vec3 vPos;
in vec3 vNormal;
in vec2 vTexCoord;

uniform sampler2D texture1;
uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform bool isBlinn;  // 切换 phong 和 blinn-phong

void main()
{
	vec3 color = texture(texture1, vTexCoord).rgb;

	// 环境光
	vec3 ambient = 0.05 * color;
	// 漫反射
	vec3 lightDir = normalize(lightPos - vPos);
	vec3 normal = normalize(vNormal);
	float diff = max(dot(lightDir, normal), 0.0);
	vec3 diffuse = diff * color;
	// 镜面反射
	vec3 cameraDir = normalize(cameraPos - vPos);
	float spec = 0.0;
	if (isBlinn)
	{
		// 半程向量和法向量的夹角
		vec3 halfwayVec = normalize(lightDir + cameraDir);
		spec = pow(max(dot(normal, halfwayVec), 0.0), 256.0);
	}
	else
	{
		// 反射向量和视点向量的夹角
		vec3 reflectDir = reflect(-lightDir, normal);
		spec = pow(max(dot(cameraDir, reflectDir), 0.0), 16.0);
	}
	vec3 specular = vec3(0.3) * spec;

	FragColor = vec4(ambient + diffuse + specular, 1.0);
}
