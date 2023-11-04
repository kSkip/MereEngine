#include "GameObjects/ObjectData.h"
#include "Utilities/TextManipulation.h"
#include "Models/OBJGLBufferUtilities.h"
#include "Models/MD5GLBufferUtilities.h"

ObjectData::ObjectData(){}

ObjectData::ObjectData(DataBlock& def)
{

    loadBounds(def);

    if(def("type") == "obj")
    {

        loadOBJMesh(def);

    }
    else if(def("type") == "md5")
    {

        loadMD5Mesh(def);

    }

}

void ObjectData::loadBounds(DataBlock& def)
{

    std::string boundsFile = def("rootdir") + def("bounds");

    if(!def("bounds").empty())
    {

        objectBounds = new boundary;
        objectBounds->LoadBoundaries(boundsFile);

    }
    else
        objectBounds = NULL;

}

void ObjectData::loadOBJMesh(DataBlock& def)
{

    std::string meshFile = def("rootdir") + def("mesh");

    std::string dirFile  = def("rootdir") + def("dir");

    if(!def("mesh").empty())
    {

        unsigned int numElements;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;

        std::string err = tinyobj::LoadObj(shapes,materials,meshFile.c_str(),dirFile.c_str());

        vertexBuffer  =  OBJCreateVertexBuffer(&shapes);
        elementBuffer =  OBJCreateElementBuffer(&shapes, &numElements);
        elementCount  =  3*numElements;
        diffuseTex    =  OBJCreateTextureBuffer(&materials,dirFile.c_str());

    }

}

void ObjectData::loadMD5Mesh(DataBlock& def)
{

    std::string meshFile = def("rootdir") + def("mesh");

    std::string dirFile  = def("rootdir") + def("dir");

    strvec anims = split(def("anim"),',');

    if(!def("mesh").empty())
    {

        {
            unsigned int numElements;
            md5meshdata md5data;

            MD5MeshFile file(meshFile.c_str());

            file.getMeshData(md5data);

            vertexBuffer = MD5CreateVertexBuffer(&md5data, &vertices, &unskinned_vertices, &num_vertices);
            elementBuffer = MD5CreateElementBuffer(&md5data, &numElements);
            elementCount = 3 * numElements;
            diffuseTex = MD5CreateTextureBuffer(&md5data, dirFile.c_str());
        }

        unsigned int i;
        for(i=0;i<anims.size();i+=2)
        {

            std::string animFile = def("rootdir") + anims.at(i+1);

            struct md5animdata* md5anim = getMD5AnimData(animFile.c_str());

            Armature* newArm = new Armature;

            newArm->buildArmature(md5anim);

            armatures[anims.at(i)] = newArm;

            freeMD5AnimData(md5anim);

        }

    }

}
