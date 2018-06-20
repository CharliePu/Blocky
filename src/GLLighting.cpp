#include "GLLighting.h"

bool pointLightUsedIndices[20];
GLLighting::PointLight::PointLight()
{
	int i = 0;
	for (i = 0; pointLightUsedIndices[i]; i++);
	pointLightUsedIndices[i] = true;
	lightID = i;
}

GLLighting::PointLight::~PointLight()
{
	pointLightUsedIndices[lightID] = false;
}

void GLLighting::PointLight::set(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
{
	this->position	= position;
	this->ambient	= ambient;
	this->diffuse	= diffuse;
	this->specular	= specular;
} 

void GLLighting::PointLight::apply()
{
	glUniform1f(getUniformLocation(((std::string)"pointLights[" + (char)(lightID + 48) + (std::string)"].constant").c_str()), 1.0f);
	glUniform1f(getUniformLocation(((std::string)"pointLights[" + (char)(lightID + 48) + (std::string)"].linear").c_str()), 0.22f);
	glUniform1f(getUniformLocation(((std::string)"pointLights[" + (char)(lightID + 48) + (std::string)"].quadratic").c_str()), 0.20f);
	glUniform3fv(getUniformLocation(((std::string)"pointLights[" + (char)(lightID + 48) + (std::string)"].position").c_str()),1, glm::value_ptr(position));
	glUniform3fv(getUniformLocation(((std::string)"pointLights[" + (char)(lightID + 48) + (std::string)"].ambient").c_str()),	1, glm::value_ptr(ambient));
	glUniform3fv(getUniformLocation(((std::string)"pointLights[" + (char)(lightID + 48) + (std::string)"].diffuse").c_str()),	1, glm::value_ptr(diffuse));
	glUniform3fv(getUniformLocation(((std::string)"pointLights[" + (char)(lightID + 48) + (std::string)"].specular").c_str()),1, glm::value_ptr(specular));
}

void GLLighting::DirectionLight::set(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
{
	this->direction = direction;
	this->ambient	= ambient;
	this->diffuse	= diffuse;
	this->specular	= specular;
}

void GLLighting::DirectionLight::apply()
{
	glUniform3fv(getUniformLocation("dirLight.direction"), 1, glm::value_ptr(direction));
	glUniform3fv(getUniformLocation("dirLight.ambient"), 1, glm::value_ptr(ambient));
	glUniform3fv(getUniformLocation("dirLight.diffuse"), 1, glm::value_ptr(diffuse));
	glUniform3fv(getUniformLocation("dirLight.specular"), 1, glm::value_ptr(specular));
}

bool spotLightUsedIndices[20];
GLLighting::SpotLight::SpotLight()
{
	int i = 0;
	for (i = 0; spotLightUsedIndices[i]; i++);
	spotLightUsedIndices[i] = true;
	lightID = i;
}

GLLighting::SpotLight::~SpotLight()
{
	spotLightUsedIndices[lightID] = false;
}

void GLLighting::SpotLight::set(glm::vec3 position, glm::vec3 direction, GLfloat cutOff, GLfloat outerCutOff, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular)
{
	this->position = position;
	this->direction = direction;
	this->cutOff = glm::cos(glm::radians(cutOff));
	this->outerCutOff = glm::cos(glm::radians(outerCutOff));
	this->ambient = ambient;
	this->diffuse = diffuse;
	this->specular = specular;
}

void GLLighting::SpotLight::apply()
{
	glUniform1f(getUniformLocation(((std::string)"spotLights[" + (char)(lightID + 48) + (std::string)"].constant").c_str()), 1.0f);
	glUniform1f(getUniformLocation(((std::string)"spotLights[" + (char)(lightID + 48) + (std::string)"].linear").c_str()), 0.22f);
	glUniform1f(getUniformLocation(((std::string)"spotLights[" + (char)(lightID + 48) + (std::string)"].quadratic").c_str()), 0.20f);
	glUniform1f(getUniformLocation(((std::string)"spotLights[" + (char)(lightID + 48) + (std::string)"].cutOff").c_str()), cutOff);
	glUniform1f(getUniformLocation(((std::string)"spotLights[" + (char)(lightID + 48) + (std::string)"].outerCutOff").c_str()), outerCutOff);
	glUniform3fv(getUniformLocation(((std::string)"spotLights[" + (char)(lightID + 48) + (std::string)"].position").c_str()), 1, glm::value_ptr(position));
	glUniform3fv(getUniformLocation(((std::string)"spotLights[" + (char)(lightID + 48) + (std::string)"].direction").c_str()), 1, glm::value_ptr(direction));
	glUniform3fv(getUniformLocation(((std::string)"spotLights[" + (char)(lightID + 48) + (std::string)"].ambient").c_str()), 1, glm::value_ptr(ambient));
	glUniform3fv(getUniformLocation(((std::string)"spotLights[" + (char)(lightID + 48) + (std::string)"].diffuse").c_str()), 1, glm::value_ptr(diffuse));
	glUniform3fv(getUniformLocation(((std::string)"spotLights[" + (char)(lightID + 48) + (std::string)"].specular").c_str()), 1, glm::value_ptr(specular));
}