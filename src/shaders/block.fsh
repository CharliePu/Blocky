#version 330 core
in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;
out vec4 color;

uniform sampler2D texture_diffuse1;

uniform vec3 viewPos;
void main()
{
	color = texture(texture_diffuse1,TexCoords)+vec4(Normal.r*2+Normal.g*4+Normal.b*8)/40;
}