#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <string>
#include <unordered_map>

#include "Models/MD5Model.h"
#include "Models/OBJMesh.h"

class ResourceManager {
public:
	void build(const std::string&);
	std::string getVertexShader(const std::string&);
	std::string getFragmentShader(const std::string&);
	OBJMesh* getStaticModel(const std::string&);
	MD5Model* getAnimatedModel(const std::string&);
	std::string getBoundsFilePath(const std::string&);
	void clear();

private:
	std::string rootDirectory;
	std::string objects;
	std::string materials;
	std::string textures;
    std::unordered_map<std::string, MD5Model*> animatedModels;
    std::unordered_map<std::string, OBJMesh*> staticModels;
};

#endif