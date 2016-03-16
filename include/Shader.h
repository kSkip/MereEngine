#ifndef SHADER_H
#define SHADER_H

#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES
#endif

#include <iostream>
#include <stdexcept>
#include <sstream>

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

#define LOCATION_POSITION 0
#define LOCATION_NORMAL 1
#define LOCATION_TEXCOORD 2

#define ENABLE_POSITION 1
#define ENABLE_NORMAL 2
#define ENABLE_TEXCOORD 4

class Shader{

    public:
        Shader();
        ~Shader();
        Shader(const Shader & rhs);

        bool loadShader(const std::string& vertexFile, const std::string& fragmentFile);

        void activate(unsigned short atrribs);
        void deactivate(unsigned short atrribs);

        GLint texture, translation, rotation, scale, position, normal, texcoord, skipMVP, skipLighting, modelView, projection, mouseOver;

    private:
        GLuint vertexShader, fragmentShader, program;

        GLuint makeShader(GLenum type, const char *filename);
        GLuint makeProgram(GLuint vertex_shader, GLuint fragment_shader);
        void* file_contents(const char *filename, GLint *length);
        void show_info_log(GLuint object,PFNGLGETSHADERIVPROC glGet__iv,PFNGLGETSHADERINFOLOGPROC glGet__InfoLog);
};

#endif
