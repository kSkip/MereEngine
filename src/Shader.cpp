#include "Shader.h"

Shader::Shader(){}

Shader::~Shader(){
    glDetachShader(program,vertexShader);
    glDetachShader(program,fragmentShader);
    glDeleteProgram(program);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

Shader::Shader(const Shader & rhs){}

bool Shader::loadShader(const std::string& vertexFile, const std::string& fragmentFile){

    vertexShader = makeShader(GL_VERTEX_SHADER,vertexFile.c_str());
	if (vertexShader == 0)
	    throw std::runtime_error("Failed to create vertexShader");

	fragmentShader = makeShader(GL_FRAGMENT_SHADER,fragmentFile.c_str());
	if (fragmentShader == 0)
	    throw std::runtime_error("Failed to create fragmentShader");

	program = makeProgram(vertexShader,fragmentShader);
	if (program == 0)
	    throw std::runtime_error("Failed to compile Shader program");


	texture  = glGetUniformLocation(program, "tex");
	translation = glGetUniformLocation(program,"translation");
	rotation = glGetUniformLocation(program,"rotation");
	scale = glGetUniformLocation(program,"scale");

	modelView = glGetUniformLocation(program,"MV");
	projection = glGetUniformLocation(program,"P");

	skipMVP = glGetUniformLocation(program,"skipMVP");
	skipLighting = glGetUniformLocation(program,"skipLighting");

	mouseOver = glGetUniformLocation(program,"mouseOver");

    return true;
}

void Shader::activate(unsigned short atrribs){

    glUseProgram(program);

    if(atrribs & ENABLE_POSITION){
        glEnableVertexAttribArray(LOCATION_POSITION);
    }
    if(atrribs & ENABLE_NORMAL){
        glEnableVertexAttribArray(LOCATION_NORMAL);
    }
    if(atrribs & ENABLE_TEXCOORD){
        glEnableVertexAttribArray(LOCATION_TEXCOORD);
    }

}

void Shader::deactivate(unsigned short atrribs){

    if(atrribs & ENABLE_POSITION){
        glDisableVertexAttribArray(LOCATION_POSITION);
    }
    if(atrribs & ENABLE_NORMAL){
        glDisableVertexAttribArray(LOCATION_NORMAL);
    }
    if(atrribs & ENABLE_TEXCOORD){
        glDisableVertexAttribArray(LOCATION_TEXCOORD);
    }

}

GLuint Shader::makeShader(GLenum type, const char *filename){

    GLint length;
    GLchar *source = (GLchar*)file_contents(filename, &length);
    GLuint shader;
    GLint shader_ok;

    if (!source) return 0;

	shader = glCreateShader(type);
    glShaderSource(shader, 1, (const GLchar**)&source, &length);
    free(source);
    glCompileShader(shader);

    glGetShaderiv(shader, GL_COMPILE_STATUS, &shader_ok);
    show_info_log(shader, glGetShaderiv, glGetShaderInfoLog);
    if (!shader_ok) {
        fprintf(stderr, "Failed to compile %s:\n", filename);
        show_info_log(shader, glGetShaderiv, glGetShaderInfoLog);
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

GLuint Shader::makeProgram(GLuint vertex_shader, GLuint fragment_shader){

    GLint program_ok;

    GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);

    glGetProgramiv(program, GL_LINK_STATUS, &program_ok);
    if (!program_ok) {
        fprintf(stderr, "Failed to link shader program:\n");
        show_info_log(program, glGetProgramiv, glGetProgramInfoLog);
        glDeleteProgram(program);
        return 0;
    }
	show_info_log(program, glGetProgramiv, glGetProgramInfoLog);

    return program;
}

void Shader::show_info_log(
    GLuint object,
    PFNGLGETSHADERIVPROC glGet__iv,
    PFNGLGETSHADERINFOLOGPROC glGet__InfoLog
)
{
    GLint log_length;
    char *log;

    glGet__iv(object, GL_INFO_LOG_LENGTH, &log_length);
    log = (char*)malloc(log_length);
    glGet__InfoLog(object, log_length, NULL, log);
    fprintf(stderr, "%s", log);
    free(log);
}

void* Shader::file_contents(const char *filename, GLint *length)
{
    FILE *f = fopen(filename, "r");
    void *buffer;

    if (!f)
    {

        std::stringstream ss;
        ss << "Unable to open " << filename << " for reading";

        throw std::runtime_error(ss.str());
        return NULL;

    }

    fseek(f, 0, SEEK_END);
    *length = ftell(f);
    fseek(f, 0, SEEK_SET);

    buffer = malloc(*length+1);
    *length = fread(buffer, 1, *length, f);
    fclose(f);
    ((char*)buffer)[*length] = '\0';

    return buffer;
}
