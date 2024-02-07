#include "ShaderProgram.h"
#include <fstream>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

ShaderProgram::ShaderProgram(std::filesystem::path& VS_file, std::filesystem::path& FS_file)
{
	GLuint VS_h, FS_h, prog_h;
    VS_h = glCreateShader(GL_VERTEX_SHADER);
    FS_h = glCreateShader(GL_FRAGMENT_SHADER);

    std::string vs = ReadFromFile(VS_file);
    const char* VS_string = vs.c_str();
    std::string fs = ReadFromFile(FS_file);
    const char* FS_string = fs.c_str();

    glShaderSource(VS_h, 1, &VS_string, NULL);
    glShaderSource(FS_h, 1, &FS_string, NULL);

    glCompileShader(VS_h);
    glCompileShader(FS_h);

    getShaderInfoLog(VS_h);
    getShaderInfoLog(FS_h);

    prog_h = glCreateProgram();
    glAttachShader(prog_h, FS_h);
    glAttachShader(prog_h, VS_h);
    glLinkProgram(prog_h);

    getProgramInfoLog(prog_h);

    glDetachShader(prog_h, VS_h);
    glDetachShader(prog_h, FS_h);

    ID = prog_h;

}
void ShaderProgram::activate(void)
{
    glUseProgram(ID);
}
void ShaderProgram::deactivate(void)
{
    glUseProgram(0);
}
void ShaderProgram::clear(void)
{
       glDeleteProgram(ID);
}

std::string ShaderProgram::getShaderInfoLog(const GLuint obj)
{
    GLint isCompiled = 0;
    glGetShaderiv(obj, GL_COMPILE_STATUS, &isCompiled);
    if (isCompiled == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &maxLength);

        std::vector<GLchar> infoLog(maxLength);
        glGetShaderInfoLog(obj, maxLength, &maxLength, &infoLog[0]);

        glDeleteShader(obj);

        std::cerr << infoLog.data();
        return infoLog.data();
    }
    return "";
}

std::string ShaderProgram::getProgramInfoLog(const GLuint obj)
{
    GLint isLinked = 0;
    glGetProgramiv(obj, GL_LINK_STATUS, (int*)&isLinked);
    if (isLinked == GL_FALSE)
    {
        GLint maxLength = 0;
        glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(obj, maxLength, &maxLength, &infoLog[0]);

        std::cerr << infoLog.data();

        return infoLog.data();
    }
    return "";
}

std::string ShaderProgram::ReadFromFile(std::filesystem::path& path) {
    std::ifstream file(path);
    std::string content;

    if (file.is_open()) {
        std::getline(file, content, '\0');
        file.close();
    }
    else {
        std::cout << "Unable to open shader file";
    }

    return content;
}

GLuint ShaderProgram::getId()
{
    return ID;
}

void ShaderProgram::setUniform(const std::string& name, float val)
{
    GLint location = glGetUniformLocation(ID, name.c_str());
    glUniform1f(location, val);
}

void ShaderProgram::setUniform(const std::string& name, int val)
{
    GLint location = glGetUniformLocation(ID, name.c_str());
    glUniform1i(location,val);
}

void ShaderProgram::setUniform(const std::string& name, glm::vec3 val)
{
    GLint location = glGetUniformLocation(ID, name.c_str());
    glUniform3fv(location, 1, glm::value_ptr(val));
}

void ShaderProgram::setUniform(const std::string& name, glm::vec4 val)
{
    GLint location = glGetUniformLocation(ID, name.c_str());
    glUniform4fv(location, 1, glm::value_ptr(val));
}

void ShaderProgram::setUniform(const std::string& name, glm::mat3 val)
{
    GLint location = glGetUniformLocation(ID, name.c_str());
    glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(val));
}

void ShaderProgram::setUniform(const std::string& name, glm::mat4 val)
{
    GLint location = glGetUniformLocation(ID, name.c_str());
    glUniformMatrix4fv(location,1,GL_FALSE,glm::value_ptr(val));
}
