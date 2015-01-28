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

bool Shader::loadShader(char* vertexfile, char* fragmentfile){
    int i;
    char name[32];

    vertexShader = makeShader(GL_VERTEX_SHADER,vertexfile);
	if (vertexShader == 0)
	    return 0;

	fragmentShader = makeShader(GL_FRAGMENT_SHADER,fragmentfile);
	if (fragmentShader == 0)
	    return 0;

	program = makeProgram(vertexShader,fragmentShader);
	if (program == 0)
	    return 0;


	texture  = glGetUniformLocation(program, "tex");
	translation = glGetUniformLocation(program,"translation");
	rotation = glGetUniformLocation(program,"rotation");
	scale = glGetUniformLocation(program,"scale");
	modelView = glGetUniformLocation(program,"MV");
	projection = glGetUniformLocation(program,"P");
	skipBones = glGetUniformLocation(program,"skipBones");
	skipMVP = glGetUniformLocation(program,"skipMVP");
	skipLighting = glGetUniformLocation(program,"skipLighting");

	fades = glGetUniformLocation(program,"fades");
	life = glGetUniformLocation(program,"life");
	mouseOver = glGetUniformLocation(program,"mouseOver");

	for(i=0;i<20;i++){
	    sprintf(name,"boneMat[%d]",i);
        boneTransformations[i] = glGetUniformLocation(program,name);
	}

	position  = glGetAttribLocation(program, "position");
	normal = glGetAttribLocation(program, "normal");
	texcoord  = glGetAttribLocation(program, "texcoord");
	boneIds  = glGetAttribLocation(program, "boneIds");
	boneWeights  = glGetAttribLocation(program, "boneWeights");

    return true;
}

void Shader::activate(){
    glUseProgram(program);
	glEnableVertexAttribArray(position);
	glEnableVertexAttribArray(texcoord);
	glEnableVertexAttribArray(normal);
	glEnableVertexAttribArray(boneIds);
	glEnableVertexAttribArray(boneWeights);
}

void Shader::deactivate(){
    glDisableVertexAttribArray(position);
	glDisableVertexAttribArray(texcoord);
	glDisableVertexAttribArray(normal);
	glDisableVertexAttribArray(boneIds);
	glDisableVertexAttribArray(boneWeights);
}

GLuint Shader::makeShader(GLenum type, const char *filename){

    GLint length;
    GLchar *source = (GLchar*)file_contents(filename, &length);
    GLuint shader;
    GLint shader_ok;

    if (!source)
        return 0;

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

    if (!f) {
        fprintf(stderr, "Unable to open %s for reading\n", filename);
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
