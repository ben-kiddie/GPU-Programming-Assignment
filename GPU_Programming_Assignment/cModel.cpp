#include "cModel.h"

Model::Model()
{
}

Model::~Model()
{
}

void Model::LoadModel(const std::string& fileName)
{
	Assimp::Importer importer;

	// Parameters of ReadFile
	//	1 - The filepath from which we are reading from
	//	2+ - Enums letting us change how we read data
	const aiScene* scene = importer.ReadFile(fileName, 
												aiProcess_Triangulate					// Force the faces we read from to exlusively be tris
												| aiProcess_FlipUVs						//
												| aiProcess_GenSmoothNormals			// Smooth the normals across our faces so that as we interpolate along them, the illusion of a smooth surface appears
												| aiProcess_JoinIdenticalVertices );	// Treat repeat vertices as just one point to save memory

	if (!scene) {
		printf("Model (%s) failed to load: %s", fileName, importer.GetErrorString());	// Output error message at %s locations (first being filename, second filepath)
		return;
	}

	LoadNode(scene->mRootNode, scene);
	LoadMaterials(scene);
}

void Model::RenderModel()
{
	for (size_t i = 0; i < mMeshList.size(); i++) {
		unsigned int materialIndex = mMeshToTexture[i];
		if (materialIndex < mTextureList.size() && mTextureList[materialIndex])	// If there is both a valid and existing material index, use this material texture
		{
			mTextureList[materialIndex]->UseTexture();
		}
		mMeshList[i]->RenderMesh();
	}
}

void Model::ClearModel()
{
	for (size_t i = 0; i < mMeshList.size(); i++) {
		if (mMeshList[i]) {
			delete mMeshList[i];
			mMeshList[i] = nullptr;
		}
	}

	for (size_t i = 0; i < mTextureList.size(); i++) {
		if (mTextureList[i]) {
			delete mTextureList[i];
			mTextureList[i] = nullptr;
		}
	}
}

void Model::LoadNode(aiNode* node, const aiScene* scene)
{
	for (size_t i = 0; i < node->mNumMeshes; i++) {
		LoadMesh(scene->mMeshes[node->mMeshes[i]], scene);	// Inside mMeshes are all the IDs for meshes in the scene, so we grab every ID one by one and access the mesh stored at this ID in our scene
	}

	for (size_t i = 0; i < node->mNumChildren; i++) {
		LoadNode(node->mChildren[i], scene);	// Using recursion, we will go through every child mesh for every mesh, meaning we will be conducting something like a depth first search (go the the end of a branch and backtrack once we hit a dead end)
	}
}

void Model::LoadMesh(aiMesh* mesh, const aiScene* scene)
{
	std::vector<GLfloat> vertices;
	std::vector<unsigned int> indices;

	for (size_t i = 0; i < mesh->mNumVertices; i++) {
		vertices.insert(vertices.end(), { mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z });
		
		if (mesh->mTextureCoords[0]) 	// The way texture coords is handled here is a bit different. Here, we are checking the first batch of texture coordinates, where mTextureCoords[0] is a group of coords, so if we wanted the u of first tex coord we would say mTexCoords[0][0].
		{
			vertices.insert(vertices.end(), { mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y });
		}
		else {
			vertices.insert(vertices.end(), { 0.0f, 0.0f });	// Even if there are no tex coords we must include dummy values because opengl will look for texture coords regardless and we don't want other data to be mixed up with them
		}

		vertices.insert(vertices.end(), { -mesh->mNormals[i].x, -mesh->mNormals[i].y, -mesh->mNormals[i].z });	// Earlier we passed in an enum "aiProcess_GenSmoothNormals" which will auto generate smoothed normals for us so we don't have to plug in dummy values
	}

	for (size_t i = 0; i < mesh->mNumFaces; i++) {
		aiFace face = mesh->mFaces[i];
		for (size_t j = 0; j < face.mNumIndices; j++) {
			indices.push_back(face.mIndices[j]);
		}
	}

	Mesh* newMesh = new Mesh();
	newMesh->CreateMesh(&vertices[0], &indices[0], vertices.size(), indices.size());
	mMeshList.push_back(newMesh);
	mMeshToTexture.push_back(mesh->mMaterialIndex);
}

void Model::LoadMaterials(const aiScene* scene)
{
	mTextureList.resize(scene->mNumMaterials);	// Initialise our vector to the number of materials we use so that we know how many to iterate through
	for (size_t i = 0; i < scene->mNumMaterials; i++) {
		aiMaterial* material = scene->mMaterials[i];
		mTextureList[i] = nullptr;

		if (material->GetTextureCount(aiTextureType_DIFFUSE))	// If at least one diffuse texture in our material, get the path of this texture out of the material
		{
			aiString path;
			if (material->GetTexture(aiTextureType_DIFFUSE, 0, &path) == AI_SUCCESS)	// Store the path to the first (0th) diffuse texture and check if it was successful so that we can load our texture
			{
				int index = std::string(path.data).rfind("\\");	// Find a value (in this case, backslash) starting from the end of the string
				std::string filename = std::string(path.data).substr(index + 1);
				std::string texPath = std::string("Textures/") + filename;

				mTextureList[i] = new Texture(texPath.c_str());
				if (!mTextureList[i]->LoadTexture()) {
					printf("Failed to load texture at: %s\n", texPath);
					delete mTextureList[i];
					mTextureList[i] = nullptr;	// This is known as a safe delete, to stop memory leaks
				}
			}
		}

		if (!mTextureList[i]) {
			mTextureList[i] = new Texture("Textures/plain.png");
			mTextureList[i]->LoadTextureA();
		}
	}
}
