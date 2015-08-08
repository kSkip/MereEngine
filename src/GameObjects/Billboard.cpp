#include "GameObjects/Billboard.h"

Billboard::Billboard(){}

Billboard::Billboard(const char* directory, const char* texturefile, float position[2], float scale[2], GameState* state){

    data = new ObjectData;

    std::string path = std::string(directory) + std::string(texturefile);

    GLuint texture = SOIL_load_OGL_texture(path.c_str(),SOIL_LOAD_RGBA,SOIL_CREATE_NEW_ID,SOIL_FLAG_TEXTURE_REPEATS | SOIL_FLAG_INVERT_Y | SOIL_FLAG_COMPRESS_TO_DXT);

    vertex *vertex_data = (vertex*) malloc(4 * sizeof(vertex));

    vertex_data[0].position[0] = -1.0f*scale[0] + position[0];
    vertex_data[0].position[1] = -1.0f*scale[1] + position[1];
    vertex_data[0].position[2] = -1.0f;

    vertex_data[1].position[0] = 1.0f*scale[0] + position[0];
    vertex_data[1].position[1] = -1.0f*scale[1] + position[1];
    vertex_data[1].position[2] = -1.0f;

    vertex_data[2].position[0] = 1.0f*scale[0] + position[0];
    vertex_data[2].position[1] = 1.0f*scale[1] + position[1];
    vertex_data[2].position[2] = -1.0f;

    vertex_data[3].position[0] = -1.0f*scale[0] + position[0];
    vertex_data[3].position[1] = 1.0f*scale[1] + position[1];
    vertex_data[3].position[2] = -1.0f;


    vertex_data[0].texcoord[0] = 0.0f;
    vertex_data[0].texcoord[1] = 1.0f;

    vertex_data[1].texcoord[0] = 1.0f;
    vertex_data[1].texcoord[1] = 1.0f;

    vertex_data[2].texcoord[0] = 1.0f;
    vertex_data[2].texcoord[1] = 0.0f;

    vertex_data[3].texcoord[0] = 0.0f;
    vertex_data[3].texcoord[1] = 0.0f;


    glGenBuffers(1, &data->vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, data->vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, 4*sizeof(vertex), vertex_data, GL_STATIC_DRAW);

    free(vertex_data);

    GLuint *element_data = (GLuint*) malloc(6 * sizeof(GLuint));

    element_data[0] = 0;
    element_data[1] = 1;
    element_data[2] = 2;
    element_data[3] = 0;
    element_data[4] = 2;
    element_data[5] = 3;

    glGenBuffers(1, &data->elementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data->elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*sizeof(GLuint) , element_data, GL_STATIC_DRAW);

    data->elementCount = 6;

    data->objectBounds = NULL;

    free(element_data);

    data->diffuseTex = texture;

    skipMVP = true;
    skipLighting = true;

    translation = glm::mat4(1.0f);
    rotation = glm::mat4(1.0f);

}

Billboard::Billboard(GLuint texture, float position[2], float scale[2], GameState* state){

    data = new ObjectData;

    vertex *vertex_data = (vertex*) malloc(4 * sizeof(vertex));

    vertex_data[0].position[0] = -1.0f*scale[0] + position[0];
    vertex_data[0].position[1] = -1.0f*scale[1] + position[1];
    vertex_data[0].position[2] = -1.0f;

    vertex_data[1].position[0] = 1.0f*scale[0] + position[0];
    vertex_data[1].position[1] = -1.0f*scale[1] + position[1];
    vertex_data[1].position[2] = -1.0f;

    vertex_data[2].position[0] = 1.0f*scale[0] + position[0];
    vertex_data[2].position[1] = 1.0f*scale[1] + position[1];
    vertex_data[2].position[2] = -1.0f;

    vertex_data[3].position[0] = -1.0f*scale[0] + position[0];
    vertex_data[3].position[1] = 1.0f*scale[1] + position[1];
    vertex_data[3].position[2] = -1.0f;


    vertex_data[0].texcoord[0] = 0.0f;
    vertex_data[0].texcoord[1] = 1.0f;

    vertex_data[1].texcoord[0] = 1.0f;
    vertex_data[1].texcoord[1] = 1.0f;

    vertex_data[2].texcoord[0] = 1.0f;
    vertex_data[2].texcoord[1] = 0.0f;

    vertex_data[3].texcoord[0] = 0.0f;
    vertex_data[3].texcoord[1] = 0.0f;


    glGenBuffers(1, &data->vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, data->vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, 4*sizeof(vertex), vertex_data, GL_STATIC_DRAW);

    free(vertex_data);

    GLuint *element_data = (GLuint*) malloc(6 * sizeof(GLuint));

    element_data[0] = 0;
    element_data[1] = 1;
    element_data[2] = 2;
    element_data[3] = 0;
    element_data[4] = 2;
    element_data[5] = 3;

    glGenBuffers(1, &data->elementBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, data->elementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*sizeof(GLuint) , element_data, GL_STATIC_DRAW);

    data->elementCount = 6;

    data->objectBounds = NULL;

    free(element_data);

    data->diffuseTex = texture;

    skipMVP = true;
    skipLighting = true;

    translation = glm::mat4(1.0f);
    rotation = glm::mat4(1.0f);

}

Billboard::~Billboard(){}

Billboard::Billboard(const Billboard & rhs){}

void Billboard::render(GameState* state){

    renderMeshElements(data->vertexBuffer,data->elementBuffer,data->elementCount,PTYPE_TRIANGLES,state);

}
