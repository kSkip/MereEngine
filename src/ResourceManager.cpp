#include "ResourceManager.h"

void ResourceManager::build(const std::string& root)
{
	rootDirectory = root;
	objects = root + "Objects\\";
	materials = root + "Materials\\";
	textures = root + "Textures\\";
}

std::string ResourceManager::getVertexShader(const std::string& shader)
{
	return rootDirectory + shader + ".vert";
}

std::string ResourceManager::getFragmentShader(const std::string& shader)
{
	return rootDirectory + shader + ".frag";
}

OBJMesh* ResourceManager::getStaticModel(const std::string& name)
{
	auto model = staticModels.find(name);
	if (model != staticModels.end()) {
		return model->second;
	}
	else {
		OBJMesh* newModel = OBJMesh::createOBJMesh(name, objects, materials, textures);
		staticModels[name] = newModel;
		return newModel;
	}
}

MD5Model* ResourceManager::getAnimatedModel(const std::string& name)
{
	auto model = animatedModels.find(name);
	if (model != animatedModels.end()) {
		return model->second;
	}
	else {
		MD5Model* newModel = MD5Model::createMD5Model(name, objects, textures);
		animatedModels[name] = newModel;
		return newModel;
	}
}

std::string ResourceManager::getBoundsFilePath(const std::string& name)
{
	return objects + name + ".bounds";
}

void ResourceManager::clear()
{
	for (auto i = animatedModels.begin(); i != animatedModels.end(); ++i) {
		delete i->second;
	}
	animatedModels.clear();

	for (auto i = staticModels.begin(); i != staticModels.end(); ++i) {
		delete i->second;
	}
	staticModels.clear();
}
