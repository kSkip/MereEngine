#ifndef SHADER_H
#define SHADER_H

#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#endif

#include <iostream>
#include <cstdio>
#include <cstdlib>

#ifdef _WIN32
#ifndef GLEW_H
#define GLEW_H
#include <GL/glew.h>
#endif // GLEW_H
#endif

#include <GL/gl.h>

#ifndef _WIN32
#include <GL/glext.h>
#endif // WIN32

class Shader{
    public:
        Shader();
        ~Shader();
        Shader(const Shader & rhs);

        bool loadShader(char* vertexfile, char* fragmentfile);

        void activate();
        void deactivate();

        GLint texture, translation, rotation, scale, position, normal, texcoord, boneIds, boneWeights, skipBones, skipMVP, skipLighting, boneTransformations[20], modelView, projection, fades, life, mouseOver;

    private:
        GLuint vertexShader, fragmentShader, program;

        GLuint makeShader(GLenum type, const char *filename);
        GLuint makeProgram(GLuint vertex_shader, GLuint fragment_shader);
        void* file_contents(const char *filename, GLint *length);
        void show_info_log(GLuint object,PFNGLGETSHADERIVPROC glGet__iv,PFNGLGETSHADERINFOLOGPROC glGet__InfoLog);
};

#endif
