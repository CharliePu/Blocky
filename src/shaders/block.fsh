#version 330 core
in vec2 TexCoords;
in vec3 Normal;

uniform sampler2D texture_diffuse1;

//lighting
uniform vec3 cameraPos;
in vec3 fragPos;

// fog
in float distanceToCamera;


out vec4 color;

struct DirLight
{
    vec3 direction;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};
uniform DirLight dirLight;

uniform float fogNear;
uniform float fogFar;

vec3 applyDirLight(vec3 color)
{
	vec3 ambient = dirLight.ambient * color;

	vec3 lightDir = normalize(-dirLight.direction);
	float diff = max(dot(Normal, lightDir), 0.0);
	vec3 diffuse = dirLight.diffuse * diff * color;

	vec3 reflectDir = reflect(-lightDir, Normal);
	float spec = pow(max(dot(normalize(cameraPos - fragPos), reflectDir), 0.0f),32);
	vec3 specular = dirLight.specular * color * spec;
	return specular + diffuse + ambient;
}

vec4 applyFog(vec4 color)
{
	float fogDensity = 0.003;
	
	float fogFactor = 1.0 / exp(distanceToCamera * fogDensity * distanceToCamera * fogDensity);
	fogFactor = clamp(fogFactor, 0.0f, 1.0f);

	return mix(vec4(0.5f, 0.8f, 1.0f, 0.0f), color, fogFactor);
}

vec4 applyLinearFog(vec4 color)
{
	float fogFactor = clamp((distanceToCamera - fogNear) / (fogFar - fogNear), 0.0f, 1.0f);
	return mix(color, vec4(0.5f, 0.8f, 1.0f, 0.0f), fogFactor);
}

void main()
{
	color = texture(texture_diffuse1,TexCoords);

	color = vec4(applyDirLight(vec3(color)), 1.0f);

	color = applyLinearFog(applyFog(color));
	//vec4(Normal.r*2+Normal.g*4+Normal.b*8)/40);
}