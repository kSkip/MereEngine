#include "Models/BasicGLBufferUtilities.h"

static vertex vertex_data[4] = {

    {-1.0f, -1.0f, 0.0f, 0.0f, 1.0f},

    {1.0f, -1.0f, 0.0f, 1.0f, 1.0f},

    {1.0f, 1.0f, 0.0f, 1.0f, 0.0f},

    {-1.0f, 1.0f, 0.0f, 0.0f, 0.0f}

};

static GLuint element_data[6] = {

0, 1, 2, 0, 2, 3

};

GLuint basicQuadVertexBuffer()
{

    GLuint vertexbuffer;

    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, 4*sizeof(vertex), vertex_data, GL_STATIC_DRAW);

    return vertexbuffer;

}

GLuint basicQuadElementBuffer()
{

    GLuint elementbuffer;

    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6*sizeof(GLuint) , element_data, GL_STATIC_DRAW);

    return elementbuffer;

}
