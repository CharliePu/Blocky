#version 330 core
in vec2 TexCoords;
in vec3 Normal;
in float distanceToCamera;
in vec3 Position;
out vec4 color;

uniform sampler2D texture_diffuse1;

uniform vec3 viewPos;

void main()
{
	float fogDensity = 0.003;
	
	float fogFactor = 1.0 / exp(distanceToCamera * fogDensity * distanceToCamera * fogDensity);
	fogFactor = clamp(fogFactor, 0.0f, 1.0f);
	
	color = mix(vec4(0.5f, 0.8f, 1.0f, 0.0f), texture(texture_diffuse1,TexCoords)+vec4(Normal.r*2+Normal.g*4+Normal.b*8)/40, fogFactor);
//	color = texture(texture_diffuse1,TexCoords)+vec4(Normal.r*2+Normal.g*4+Normal.b*8)/40;
}