#version 330 core

out vec4 FragColor;

/* in VS_OUT
{
    vec2 vTexCoord;
} fs_in; */

in vec2 vTexCoord;

uniform sampler2D texture_diffuse1;

void main()
{
	FragColor = texture(texture_diffuse1, vTexCoord);
}