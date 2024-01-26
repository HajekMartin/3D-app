#pragma once
#include "ShaderProgram.h"
#include "Vertex.h"
#include "Camera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
class Mesh {
public:
    glm::vec3 position;
    float width;
    float height;
    bool transparent = false;
    ShaderProgram shader;
    std::vector<vertex> vertices;
    GLuint VAO = 0;
    GLuint VBO = 0;
    GLuint EBO = 0;
    GLuint texture_id = 0;
    GLenum primitive = GL_TRIANGLES;

    std::string obj_path;
    Mesh(void);
    Mesh(std::string obj_path, ShaderProgram shader, GLuint texture_id, float texture_scale, float height, float width, glm::vec3 position);
    void setModelMatrix(glm::mat4 modelMatrix);
    glm::mat4 getModelMatrix(void);
    void draw();
private:
    glm::mat4 modelMatrix;
    glm::mat3 modelNormalMatrix;
};
