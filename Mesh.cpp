#include "Mesh.h"
#include <GL/glew.h>
#include <GL/wglew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include "Vertex.h"
#include <glm/gtc/type_ptr.hpp>
#include "ShaderProgram.h"
#include "OBJloader.h"

Mesh::Mesh(void): shader(shader)
{

}

Mesh::Mesh(std::string obj_path, ShaderProgram shader,GLuint texture_id,float texture_scale,float height, float width,glm::vec3 position): obj_path(obj_path), shader(shader),texture_id(texture_id), position(position){
    this->height = height;
    this->width = width;
    std::vector<glm::vec3> tmp_vertices;
    std::vector<glm::vec2> tmp_uvs;
    std::vector<glm::vec3> tmp_normals;
    std::string objpath = obj_path;
    const char* objpath_p = objpath.c_str();
    loadOBJ(objpath_p, tmp_vertices, tmp_uvs, tmp_normals);

    for (size_t i = 0; i < tmp_vertices.size(); ++i) {
        vertices.emplace_back((vertex{ tmp_vertices.at(i), (tmp_uvs.at(i)*texture_scale), tmp_normals.at(i) }));
    }

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vertex), vertices.data(), GL_STATIC_DRAW);

    //explain GPU the memory layout of the data...
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<void*>(0 + offsetof(vertex, position)));
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<void*>(0 + offsetof(vertex, texCoord)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), reinterpret_cast<void*>(0 + offsetof(vertex, normal)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
void Mesh::setModelMatrix(glm::mat4 M_m)
{
    modelMatrix = M_m;
    modelNormalMatrix = glm::mat3(glm::transpose(glm::inverse(modelMatrix)));
}
glm::mat4 Mesh::getModelMatrix(void)
{
    return modelMatrix;
}

void Mesh::draw()
{
    shader.activate();
    glActiveTexture(GL_TEXTURE0);
    shader.setUniform("tex0", 0);
    shader.setUniform("uMm", modelMatrix);
    shader.setUniform("uMm_n", modelNormalMatrix);
    glBindVertexArray(VAO);
    if (texture_id !=0)
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glDrawArrays(primitive, 0, vertices.size());
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
};

