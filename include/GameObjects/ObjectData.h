#ifndef OBJECTDATA_H
#define OBJECTDATA_H

#include "Platform.h"

#include "Utilities/DataBlock.h"
#include "Models/Armature.h"
#include "boundary.h"

class ObjectData{

public:

    ObjectData();
    ObjectData(DataBlock& def);

    std::string type; //remember this may be referred to

    /*
     * GPU array buffers
     */
    GLuint vertexBuffer;
    GLuint elementBuffer;
    GLsizei elementCount;

    /*
     * local memory data for CPU processing
     */
    std::map<std::string,Armature*> armatures;
    unsigned int numVertices;
    std::vector<UnskinnedVertex> unskinnedVertices;
    std::vector<vertex> vertices;

    /*
     * GPU texture buffers
     */
    GLuint diffuseTex;
    GLuint normalTex;
    GLuint displacementTex;
    GLuint occlusionTex;

    /*
     * bounds vertices for collisions
     */
    boundary* objectBounds;

private:

    void loadBounds(DataBlock& def);

    void loadOBJMesh(DataBlock& def);

    void loadMD5Mesh(DataBlock& def);

};

#endif // OBJECTDATA_H
