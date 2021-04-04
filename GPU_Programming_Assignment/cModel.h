#pragma once

#include <vector>
#include <string>
#include <iostream>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "cMesh.h"
#include "cTexture.h"

class Model
{
public:
	Model();
	~Model();

	void LoadModel(const std::string& fileName);
	void RenderModel();
	void ClearModel();

private:
	void LoadNode(aiNode* node, const aiScene* scene);
	void LoadMesh(aiMesh* mesh, const aiScene* scene);
	void LoadMaterials(const aiScene* scene);

	std::vector<Mesh*> mMeshList;
	std::vector<Texture*> mTextureList;
	std::vector<unsigned int> mMeshToTexture;
};

