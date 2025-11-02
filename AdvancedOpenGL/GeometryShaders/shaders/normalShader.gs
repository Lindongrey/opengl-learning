#version 330 core

layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT
{
	vec3 vNormal;
} gs_in[];

uniform mat4 projection;

const float MAGNITUDE = 0.4;

// 先找到顶点，然后找到法线的另一端，由于是绘制 line 模式，所以会画出线条
void GenerateLine(int index)
{
	gl_Position = projection * gl_in[index].gl_Position;
	EmitVertex();
	gl_Position = projection * (gl_in[index].gl_Position + vec4(gs_in[index].vNormal, 0.0) * MAGNITUDE);
	EmitVertex();

	EndPrimitive();
}

void main()
{
	// 有三个顶点
	GenerateLine(0);
	GenerateLine(1);
	GenerateLine(2);
}