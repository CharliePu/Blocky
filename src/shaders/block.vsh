#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec3 texCoords;

out vec3 TexCoords;
out vec3 Normal;
out vec3 fragPos;
out float distanceToCamera;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection*view*vec4(position,1.0f);
	TexCoords = texCoords;
	Normal = normal;
	fragPos = position;
	distanceToCamera = length(view * vec4(position, 1.0f));
}  