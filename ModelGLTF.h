
#ifndef MODEL_GLTF_H
#define MODEL_GLTF_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <string>
#include <cstdint>

#include "ShaderClass.h"
#include "VAO.h"
#include "VBO.h"
#include "EBO.h"

struct PrimitiveMesh
{
    VAO vao;
    VBO vbo;
    EBO ebo;

    GLsizei indexCount;
    GLenum indexType;

    GLuint textureID;

    bool hasIndices;
    bool isSky;

    glm::mat4 nodeTransform;

    PrimitiveMesh(
        const std::vector<float>& vertices,
        const std::vector<uint32_t>& indices,
        GLuint tex,
        const glm::mat4& transform,
        bool sky
    );

    void Draw(Shader& shaderProgram);
    void Delete();
};

class ModelGLTF
{
public:

    std::vector<PrimitiveMesh> meshes;
    std::vector<GLuint> ownedTextures;

    ModelGLTF() = default;

    bool Load(const std::string& filename);

    void Draw(Shader& shaderProgram);

    void Delete();
};

#endif
