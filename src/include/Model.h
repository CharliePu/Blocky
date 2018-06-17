#pragma once
#include <string>
#include <sstream>
#include <vector>

#include "Texture.h"
#include "Shader.h"
#include "Mesh.h"

#include <GLFW\glfw3.h>
#include <glm\glm.hpp>



class Model
{
public:
	/*  Functions   */
	Model(GLchar* path);
	void draw();
private:
	/*  Model Data  */
	std::vector<Mesh> meshes;
	std::string directory;
	std::vector<Texture> textures_loaded;
	/*  Functions   */
	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
		textureType typeName);
};