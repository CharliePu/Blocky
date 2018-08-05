#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out vec2 TexCoords;
out vec3 Normal;
out vec3 ToCameraDirection;
out vec3 CameraPosition;
out float distanceToCamera;

uniform vec3 cameraPosition;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection*view*vec4(position,1.0f);
	TexCoords = texCoords;
	Normal = normal;
	CameraPosition = cameraPosition;
	ToCameraDirection = position - cameraPosition;
	distanceToCamera = length(view * vec4(position, 1.0f));
}  