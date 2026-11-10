#define _CRT_SECURE_NO_WARNINGS
#include "ModelGLTF.h"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <tinygltf/tiny_gltf.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <functional>
#include <algorithm>

// Corrección de altura exclusiva del cielo
static constexpr float SKY_VERTICAL_OFFSET = -320.0f;


// ---------------------------------------------------------
// TRANSFORMACIÓN DE NODOS GLTF
// ---------------------------------------------------------

static glm::mat4 GetNodeLocalTransform(
    const tinygltf::Node& node)
{
    if (node.matrix.size() == 16)
    {
        glm::mat4 result(1.0f);

        for (int column = 0; column < 4; ++column)
        {
            for (int row = 0; row < 4; ++row)
            {
                result[column][row] =
                    static_cast<float>(
                        node.matrix[column * 4 + row]
                        );
            }
        }

        return result;
    }

    glm::vec3 translation(0.0f);
    glm::vec3 scale(1.0f);

    glm::quat rotation(
        1.0f, 0.0f, 0.0f, 0.0f
    );

    if (node.translation.size() == 3)
    {
        translation = glm::vec3(
            node.translation[0],
            node.translation[1],
            node.translation[2]
        );
    }

    if (node.scale.size() == 3)
    {
        scale = glm::vec3(
            node.scale[0],
            node.scale[1],
            node.scale[2]
        );
    }

    if (node.rotation.size() == 4)
    {
        rotation = glm::quat(
            static_cast<float>(node.rotation[3]),
            static_cast<float>(node.rotation[0]),
            static_cast<float>(node.rotation[1]),
            static_cast<float>(node.rotation[2])
        );
    }

    return
        glm::translate(glm::mat4(1.0f), translation) *
        glm::mat4_cast(rotation) *
        glm::scale(glm::mat4(1.0f), scale);
}


// ---------------------------------------------------------
// CARGAR TEXTURAS
// ---------------------------------------------------------

static GLuint CreateGLTFTexture(
    const tinygltf::Image& image)
{
    if (image.image.empty() ||
        image.width <= 0 ||
        image.height <= 0)
        return 0;

    GLenum format = GL_RGBA;

    if (image.component == 1)
        format = GL_RED;
    else if (image.component == 2)
        format = GL_RG;
    else if (image.component == 3)
        format = GL_RGB;
    else if (image.component != 4)
        return 0;

    GLuint textureID = 0;

    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_S,
        GL_REPEAT
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_T,
        GL_REPEAT
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        GL_LINEAR_MIPMAP_LINEAR
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MAG_FILTER,
        GL_LINEAR
    );

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        format,
        image.width,
        image.height,
        0,
        format,
        GL_UNSIGNED_BYTE,
        image.image.data()
    );

    glGenerateMipmap(GL_TEXTURE_2D);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    glBindTexture(GL_TEXTURE_2D, 0);

    return textureID;
}


// ---------------------------------------------------------
// LEER ATRIBUTOS DE VÉRTICES
// ---------------------------------------------------------

static bool ReadFloatAttribute(
    const tinygltf::Model& model,
    int accessorIndex,
    int components,
    std::vector<float>& output)
{
    if (accessorIndex < 0 ||
        accessorIndex >= (int)model.accessors.size())
        return false;

    const auto& accessor =
        model.accessors[accessorIndex];

    if (accessor.componentType !=
        TINYGLTF_COMPONENT_TYPE_FLOAT)
        return false;

    if (accessor.bufferView < 0 ||
        accessor.bufferView >= (int)model.bufferViews.size())
        return false;

    const auto& view =
        model.bufferViews[accessor.bufferView];

    const auto& buffer =
        model.buffers[view.buffer];

    const size_t elementSize =
        components * sizeof(float);

    const int byteStride = accessor.ByteStride(view);

    const size_t stride =
        byteStride > 0
        ? static_cast<size_t>(byteStride)
        : elementSize;

    const size_t start =
        view.byteOffset + accessor.byteOffset;

    if (start >= buffer.data.size())
        return false;

    output.resize(accessor.count * components);

    for (size_t i = 0; i < accessor.count; ++i)
    {
        const size_t offset = start + i * stride;

        if (offset + elementSize > buffer.data.size())
            return false;

        const float* values =
            reinterpret_cast<const float*>(
                buffer.data.data() + offset
                );

        for (int c = 0; c < components; ++c)
        {
            output[i * components + c] = values[c];
        }
    }

    return true;
}


// ---------------------------------------------------------
// LEER ÍNDICES
// ---------------------------------------------------------

static bool ReadIndices(
    const tinygltf::Model& model,
    int accessorIndex,
    std::vector<uint32_t>& indices)
{
    if (accessorIndex < 0 ||
        accessorIndex >= (int)model.accessors.size())
        return false;

    const auto& accessor =
        model.accessors[accessorIndex];

    if (accessor.bufferView < 0 ||
        accessor.bufferView >= (int)model.bufferViews.size())
        return false;

    const auto& view =
        model.bufferViews[accessor.bufferView];

    const auto& buffer =
        model.buffers[view.buffer];

    const unsigned char* data =
        buffer.data.data() +
        view.byteOffset +
        accessor.byteOffset;

    indices.resize(accessor.count);

    for (size_t i = 0; i < accessor.count; ++i)
    {
        if (accessor.componentType ==
            TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
        {
            indices[i] =
                reinterpret_cast<const uint8_t*>(data)[i];
        }
        else if (accessor.componentType ==
            TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
        {
            indices[i] =
                reinterpret_cast<const uint16_t*>(data)[i];
        }
        else if (accessor.componentType ==
            TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
        {
            indices[i] =
                reinterpret_cast<const uint32_t*>(data)[i];
        }
        else
        {
            return false;
        }
    }

    return true;
}


// ---------------------------------------------------------
// CONSTRUCTOR DE PRIMITIVEMESH
// ---------------------------------------------------------

PrimitiveMesh::PrimitiveMesh(
    const std::vector<float>& vertices,
    const std::vector<uint32_t>& indices,
    GLuint tex,
    const glm::mat4& transform,
    bool sky
)
    : indexCount(static_cast<GLsizei>(indices.size())),
    indexType(GL_UNSIGNED_INT),
    textureID(tex),
    hasIndices(!indices.empty()),
    isSky(sky),
    nodeTransform(transform),
    vbo(
        const_cast<float*>(vertices.data()),
        static_cast<GLsizeiptr>(
            vertices.size() * sizeof(float)
            )
    ),
    ebo(
        hasIndices
        ? const_cast<GLuint*>(indices.data())
        : nullptr,
        static_cast<GLsizeiptr>(
            hasIndices
            ? indices.size() * sizeof(GLuint)
            : 0
            )
    )
{
    vao.Bind();

    vbo.Bind();

    if (hasIndices)
        ebo.Bind();

    // Posición
    vao.LinkAttrib(
        vbo, 0, 3, GL_FLOAT,
        8 * sizeof(float), (void*)0
    );

    // Normales
    vao.LinkAttrib(
        vbo, 1, 3, GL_FLOAT,
        8 * sizeof(float),
        (void*)(3 * sizeof(float))
    );

    // Coordenadas UV
    vao.LinkAttrib(
        vbo, 2, 2, GL_FLOAT,
        8 * sizeof(float),
        (void*)(6 * sizeof(float))
    );

    vao.Unbind();
    vbo.Unbind();

    if (hasIndices)
        ebo.Unbind();
}


// ---------------------------------------------------------
// DIBUJAR CADA PRIMITIVA
// ---------------------------------------------------------

void PrimitiveMesh::Draw(Shader& shaderProgram)
{
    // Enviar transformación individual del nodo
    GLint transformLoc =
        glGetUniformLocation(
            shaderProgram.ID,
            "uNodeTransform"
        );

    if (transformLoc >= 0)
    {
        glUniformMatrix4fv(
            transformLoc,
            1,
            GL_FALSE,
            glm::value_ptr(nodeTransform)
        );
    }

    // Identificar si pertenece al cielo
    GLint skyLoc =
        glGetUniformLocation(
            shaderProgram.ID,
            "isSky"
        );

    if (skyLoc >= 0)
    {
        glUniform1i(
            skyLoc,
            isSky ? 1 : 0
        );
    }

    // Activar textura
    glActiveTexture(GL_TEXTURE0);

    glBindTexture(
        GL_TEXTURE_2D,
        textureID
    );

    GLint texLoc =
        glGetUniformLocation(
            shaderProgram.ID,
            "tex0"
        );

    if (texLoc >= 0)
        glUniform1i(texLoc, 0);

    vao.Bind();

    if (hasIndices)
    {
        glDrawElements(
            GL_TRIANGLES,
            indexCount,
            indexType,
            0
        );
    }

    vao.Unbind();
}


// ---------------------------------------------------------
// LIBERAR PRIMITIVA
// ---------------------------------------------------------

void PrimitiveMesh::Delete()
{
    vao.Delete();
    vbo.Delete();

    if (hasIndices)
        ebo.Delete();

    // Las texturas se liberan desde ModelGLTF,
    // porque pueden compartirse entre varias primitivas.
}


// ---------------------------------------------------------
// CARGAR MODELO GLTF
// ---------------------------------------------------------

bool ModelGLTF::Load(const std::string& filename)
{
    meshes.clear();

    tinygltf::Model model;
    tinygltf::TinyGLTF loader;

    std::string err, warn;

    bool loaded = false;

    size_t dot = filename.find_last_of('.');

    std::string ext =
        dot == std::string::npos
        ? ""
        : filename.substr(dot + 1);

    if (ext == "glb" || ext == "GLB")
    {
        loaded = loader.LoadBinaryFromFile(
            &model, &err, &warn, filename
        );
    }
    else
    {
        loaded = loader.LoadASCIIFromFile(
            &model, &err, &warn, filename
        );
    }

    if (!warn.empty())
        std::cout << "GLTF Advertencia: " << warn << std::endl;

    if (!err.empty())
        std::cout << "GLTF Error: " << err << std::endl;

    if (!loaded)
        return false;


    // 1. Cargar texturas
    std::vector<GLuint> textures;

    for (const auto& image : model.images)
    {
        textures.push_back(
            CreateGLTFTexture(image)
        );
    }


    // 2. Recorrer jerarquía de nodos
    std::function<void(int, const glm::mat4&)> processNode;

    processNode =
        [&](int nodeIndex, const glm::mat4& parentTransform)
        {
            if (nodeIndex < 0 ||
                nodeIndex >= (int)model.nodes.size())
                return;

            const tinygltf::Node& node =
                model.nodes[nodeIndex];

            glm::mat4 localTransform =
                GetNodeLocalTransform(node);

            // Corregir altura únicamente del cielo
            if (node.name == "Sky")
            {
                std::cout
                    << "Cielo identificado correctamente: Sky"
                    << std::endl;

                localTransform =
                    localTransform *
                    glm::translate(
                        glm::mat4(1.0f),
                        glm::vec3(
                            0.0f,
                            0.0f,
                            SKY_VERTICAL_OFFSET
                        )
                    );
            }

            glm::mat4 worldTransform =
                parentTransform * localTransform;


            // 3. Procesar malla del nodo
            if (node.mesh >= 0 &&
                node.mesh < (int)model.meshes.size())
            {
                const auto& mesh =
                    model.meshes[node.mesh];

                for (const auto& primitive : mesh.primitives)
                {
                    if (primitive.mode != -1 &&
                        primitive.mode != TINYGLTF_MODE_TRIANGLES)
                    {
                        std::cout
                            << "Aviso: primitiva omitida."
                            << std::endl;

                        continue;
                    }

                    auto posIt =
                        primitive.attributes.find("POSITION");

                    if (posIt == primitive.attributes.end())
                        continue;

                    std::vector<float> positions;

                    if (!ReadFloatAttribute(
                        model,
                        posIt->second,
                        3,
                        positions))
                        continue;

                    size_t vertexCount =
                        positions.size() / 3;

                    std::vector<float> normals(
                        vertexCount * 3, 0.0f
                    );

                    std::vector<float> texcoords(
                        vertexCount * 2, 0.0f
                    );

                    auto normalIt =
                        primitive.attributes.find("NORMAL");

                    if (normalIt != primitive.attributes.end())
                    {
                        ReadFloatAttribute(
                            model,
                            normalIt->second,
                            3,
                            normals
                        );
                    }

                    auto uvIt =
                        primitive.attributes.find("TEXCOORD_0");

                    if (uvIt != primitive.attributes.end())
                    {
                        ReadFloatAttribute(
                            model,
                            uvIt->second,
                            2,
                            texcoords
                        );
                    }


                    // 4. Construir datos de vértices
                    std::vector<float> vertexData;

                    vertexData.reserve(vertexCount * 8);

                    for (size_t i = 0; i < vertexCount; ++i)
                    {
                        // Posición
                        vertexData.push_back(positions[i * 3]);
                        vertexData.push_back(positions[i * 3 + 1]);
                        vertexData.push_back(positions[i * 3 + 2]);

                        // Normales
                        vertexData.push_back(normals[i * 3]);
                        vertexData.push_back(normals[i * 3 + 1]);
                        vertexData.push_back(normals[i * 3 + 2]);

                        // Coordenadas UV
                        vertexData.push_back(texcoords[i * 2]);
                        vertexData.push_back(texcoords[i * 2 + 1]);
                    }


                    // 5. Leer índices
                    std::vector<uint32_t> indices;

                    if (primitive.indices >= 0)
                    {
                        if (!ReadIndices(
                            model,
                            primitive.indices,
                            indices))
                            continue;
                    }


                    // 6. Identificar textura del material
                    GLuint texID = 0;

                    if (primitive.material >= 0 &&
                        primitive.material < (int)model.materials.size())
                    {
                        const auto& material =
                            model.materials[primitive.material];

                        auto textureIt =
                            material.values.find("baseColorTexture");

                        if (textureIt != material.values.end())
                        {
                            int textureIndex =
                                textureIt->second.TextureIndex();

                            if (textureIndex >= 0 &&
                                textureIndex < (int)model.textures.size())
                            {
                                int imageIndex =
                                    model.textures[textureIndex].source;

                                if (imageIndex >= 0 &&
                                    imageIndex < (int)textures.size())
                                {
                                    texID = textures[imageIndex];
                                }
                            }
                        }
                    }


                    // 7. Crear primitiva con transformación y clasificación
                    meshes.emplace_back(
                        vertexData,
                        indices,
                        texID,
                        worldTransform,
                        node.name == "Sky"
                    );
                }
            }


            // 8. Procesar hijos
            for (int childIndex : node.children)
            {
                processNode(
                    childIndex,
                    worldTransform
                );
            }
        };


    // 9. Identificar escena activa
    if (model.scenes.empty())
        return false;

    int activeScene = model.defaultScene;

    if (activeScene < 0 ||
        activeScene >= (int)model.scenes.size())
    {
        activeScene = 0;
    }

    const auto& scene =
        model.scenes[activeScene];

    for (int rootNode : scene.nodes)
    {
        processNode(
            rootNode,
            glm::mat4(1.0f)
        );
    }


    // 10. Guardar identificadores de texturas.
    // Se liberan una sola vez, incluso si se comparten.
    for (GLuint tex : textures)
    {
        if (tex != 0)
            ownedTextures.push_back(tex);
    }

    std::cout
        << "Modelo glTF cargado. Primitivas: "
        << meshes.size()
        << std::endl;

    return !meshes.empty();
}


// ---------------------------------------------------------
// DIBUJAR MODELO COMPLETO
// ---------------------------------------------------------

void ModelGLTF::Draw(Shader& shaderProgram)
{
    // Primero dibujamos el cielo
    for (auto& mesh : meshes)
    {
        if (mesh.isSky)
            mesh.Draw(shaderProgram);
    }

    // Después dibujamos el resto del escenario
    for (auto& mesh : meshes)
    {
        if (!mesh.isSky)
            mesh.Draw(shaderProgram);
    }
}


// ---------------------------------------------------------
// LIBERAR RECURSOS
// ---------------------------------------------------------

void ModelGLTF::Delete()
{
    for (auto& mesh : meshes)
    {
        mesh.Delete();
    }

    meshes.clear();

    for (GLuint tex : ownedTextures)
    {
        if (tex != 0)
            glDeleteTextures(1, &tex);
    }

    ownedTextures.clear();
}
