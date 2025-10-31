#version 330 core

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D frontTex;
uniform sampler2D backTex;

void main()
{
	// Æ¬¶Î×ø±ê
	/* if (gl_FragCoord.x > 400.0)
		FragColor = vec4(1.0, 0.0, 0.0, 1.0);
	else
		FragColor = vec4(0.0, 1.0, 0.0, 1.0); */
	// ÅÐ¶ÏÃæ³¯Ïò
	if (gl_FrontFacing)
		FragColor = texture(frontTex, TexCoord);
	else
		FragColor = texture(backTex, TexCoord);
}