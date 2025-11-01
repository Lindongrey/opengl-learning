#version 330 core

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT
{
    vec2 vTexCoord;
} gs_in[];

out vec2 gTexCoord;

uniform float time;

// 四个屋顶有雪的房子
// void build_house(vec4 position)
// {
//     // 因为在所有 emit 之前，所以写在这里就相当于给所有顶点都传去这个颜色
//     fColor = gs_in[0].color;
// 
// 	gl_Position = position + vec4(-0.2, -0.2, 0.0, 0.0);    // 1:左上 
// 	EmitVertex();
// 	gl_Position = position + vec4( 0.2, -0.2, 0.0, 0.0);    // 2:右下
//     EmitVertex();
//     gl_Position = position + vec4(-0.2,  0.2, 0.0, 0.0);    // 3:左上
//     EmitVertex();
//     gl_Position = position + vec4( 0.2,  0.2, 0.0, 0.0);    // 4:右上
//     EmitVertex();
//     // fColor = vec3(1.0, 1.0, 1.0); // 写在这里和写在下面是等价的，它们处于同一个 emit 之前
//     gl_Position = position + vec4( 0.0,  0.4, 0.0, 0.0);    // 5:顶部
//     // 将最后一个顶点设置为白色
//     // 写在这里就是给后面所有顶点传去白色
//     // 片段着色器接收到的，就是顶部的顶点是白色，其他顶点按照顶点着色器传来的颜色
//     fColor = vec3(1.0, 1.0, 1.0);
//     EmitVertex();
// 	EndPrimitive();
// }

// 获得法向量
vec3 GetNormal()
{
    vec3 a = vec3(gl_in[0].gl_Position) - vec3(gl_in[1].gl_Position);
    vec3 b = vec3(gl_in[1].gl_Position) - vec3(gl_in[2].gl_Position);
    return normalize(cross(a, b));
}

// 爆炸效果
vec4 explode(vec4 position, vec3 normal)
{
    float magnitude = 2.0;
    vec3 direction = normal * ((sin(time) + 1.0) / 2.0) * magnitude;
    return position + vec4(direction, 1.0);
}

void main()
{
	// build_house(gl_in[0].gl_Position);
    vec3 normal = GetNormal();

    gl_Position = explode(gl_in[0].gl_Position, normal);
    gTexCoord = gs_in[0].vTexCoord;
    EmitVertex();
    gl_Position = explode(gl_in[1].gl_Position, normal);
    gTexCoord = gs_in[1].vTexCoord;
    EmitVertex();
    gl_Position = explode(gl_in[2].gl_Position, normal);
    gTexCoord = gs_in[2].vTexCoord;
    EmitVertex();

    EndPrimitive();
}