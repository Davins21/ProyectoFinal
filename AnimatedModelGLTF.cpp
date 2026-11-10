
#include "AnimatedModelGLTF.h"
#include <tinygltf/tiny_gltf.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>

#include <iostream>
#include <algorithm>
#include <functional>
#include <cmath>

// ======================================================
// FUNCIONES AUXILIARES
// ======================================================

static bool ReadFloats(
    const tinygltf::Model& model,
    int index,
    int components,
    std::vector<float>& result)
{
    if (index < 0 || index >= (int)model.accessors.size())
        return false;

    const auto& a = model.accessors[index];
    if (a.bufferView < 0 ||
        a.componentType != TINYGLTF_COMPONENT_TYPE_FLOAT)
        return false;

    const auto& v = model.bufferViews[a.bufferView];
    const auto& b = model.buffers[v.buffer];

    int stride = a.ByteStride(v);
    if (stride <= 0) stride = components * sizeof(float);

    result.resize(a.count * components);

    const unsigned char* data =
        b.data.data() + v.byteOffset + a.byteOffset;

    for (size_t i = 0; i < a.count; i++)
    {
        const float* p =
            reinterpret_cast<const float*>(data + i * stride);

        for (int j = 0; j < components; j++)
            result[i * components + j] = p[j];
    }

    return true;
}

static bool ReadJoints(
    const tinygltf::Model& model,
    int index,
    std::vector<glm::vec4>& result)
{
    const auto& a = model.accessors[index];
    const auto& v = model.bufferViews[a.bufferView];
    const auto& b = model.buffers[v.buffer];

    int stride = a.ByteStride(v);
    if (stride <= 0)
        stride = a.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE ? 4 : 8;

    const unsigned char* data =
        b.data.data() + v.byteOffset + a.byteOffset;

    result.resize(a.count);

    for (size_t i = 0; i < a.count; i++)
    {
        const unsigned char* p = data + i * stride;

        for (int j = 0; j < 4; j++)
        {
            if (a.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
                result[i][j] = reinterpret_cast<const uint16_t*>(p)[j];
            else if (a.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
                result[i][j] = p[j];
            else
                return false;
        }
    }

    return true;
}

static bool ReadIndices(
    const tinygltf::Model& model,
    int index,
    std::vector<uint32_t>& result)
{
    const auto& a = model.accessors[index];
    const auto& v = model.bufferViews[a.bufferView];
    const auto& b = model.buffers[v.buffer];

    const unsigned char* data =
        b.data.data() + v.byteOffset + a.byteOffset;

    result.resize(a.count);

    for (size_t i = 0; i < a.count; i++)
    {
        if (a.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT)
            result[i] = reinterpret_cast<const uint16_t*>(data)[i];
        else if (a.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT)
            result[i] = reinterpret_cast<const uint32_t*>(data)[i];
        else if (a.componentType == TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE)
            result[i] = data[i];
        else
            return false;
    }

    return true;
}

static GLuint LoadTexture(const tinygltf::Image& image)
{
    if (image.image.empty()) return 0;

    GLenum format = image.component == 4 ? GL_RGBA :
        image.component == 3 ? GL_RGB : GL_RED;

    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexImage2D(
        GL_TEXTURE_2D, 0, format,
        image.width, image.height, 0,
        format, GL_UNSIGNED_BYTE, image.image.data()
    );

    glGenerateMipmap(GL_TEXTURE_2D);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);

    return id;
}

// ======================================================
// CARGAR MODELO
// ======================================================

bool AnimatedModelGLTF::Load(const std::string& filename)
{
    Delete();

    tinygltf::Model source;
    tinygltf::TinyGLTF loader;

    std::string error, warning;

    size_t dot = filename.find_last_of('.');
    std::string ext = dot == std::string::npos ? "" : filename.substr(dot + 1);

    bool loadedFile = (ext == "glb" || ext == "GLB")
        ? loader.LoadBinaryFromFile(&source, &error, &warning, filename)
        : loader.LoadASCIIFromFile(&source, &error, &warning, filename);

    if (!warning.empty()) std::cout << warning << std::endl;
    if (!error.empty()) std::cout << error << std::endl;
    if (!loadedFile) return false;

    // NODOS
    nodes.resize(source.nodes.size());

    for (size_t i = 0; i < source.nodes.size(); i++)
    {
        const auto& s = source.nodes[i];
        auto& n = nodes[i];

        n.mesh = s.mesh;
        n.skin = s.skin;
        n.children = s.children;

        if (s.translation.size() == 3)
            n.translation = glm::vec3(s.translation[0], s.translation[1], s.translation[2]);

        if (s.scale.size() == 3)
            n.scale = glm::vec3(s.scale[0], s.scale[1], s.scale[2]);

        if (s.rotation.size() == 4)
            n.rotation = glm::quat(
                s.rotation[3], s.rotation[0],
                s.rotation[1], s.rotation[2]
            );
    }

    for (size_t i = 0; i < nodes.size(); i++)
        for (int child : nodes[i].children)
            if (child >= 0 && child < (int)nodes.size())
                nodes[child].parent = (int)i;

    // ESQUELETO
    if (!source.skins.empty())
    {
        const auto& skin = source.skins[0];

        skinRoot = skin.skeleton;
        jointNodeIndices = skin.joints;

        std::vector<float> matrices;

        if (skin.inverseBindMatrices >= 0 &&
            ReadFloats(source, skin.inverseBindMatrices, 16, matrices))
        {
            inverseBindMatrices.resize(matrices.size() / 16);

            for (size_t i = 0; i < inverseBindMatrices.size(); i++)
                for (int c = 0; c < 4; c++)
                    for (int r = 0; r < 4; r++)
                        inverseBindMatrices[i][c][r] =
                        matrices[i * 16 + c * 4 + r];
        }
        else
        {
            inverseBindMatrices.assign(
                jointNodeIndices.size(), glm::mat4(1.0f)
            );
        }
    }

    // TEXTURAS
    for (const auto& image : source.images)
        textures.push_back(LoadTexture(image));

    // GEOMETRIA
    for (size_t ni = 0; ni < source.nodes.size(); ni++)
    {
        const auto& node = source.nodes[ni];

        if (node.mesh < 0) continue;

        for (const auto& primitive : source.meshes[node.mesh].primitives)
        {
            if (primitive.mode != -1 &&
                primitive.mode != TINYGLTF_MODE_TRIANGLES)
                continue;

            auto posIt = primitive.attributes.find("POSITION");
            auto normIt = primitive.attributes.find("NORMAL");
            auto uvIt = primitive.attributes.find("TEXCOORD_0");
            auto jointIt = primitive.attributes.find("JOINTS_0");
            auto weightIt = primitive.attributes.find("WEIGHTS_0");

            if (posIt == primitive.attributes.end() ||
                jointIt == primitive.attributes.end() ||
                weightIt == primitive.attributes.end())
                continue;

            std::vector<float> pos, norm, uv, weights;
            std::vector<glm::vec4> joints;
            std::vector<uint32_t> indices;

            if (!ReadFloats(source, posIt->second, 3, pos) ||
                !ReadJoints(source, jointIt->second, joints) ||
                !ReadFloats(source, weightIt->second, 4, weights))
                continue;

            size_t count = pos.size() / 3;

            norm.assign(count * 3, 0.0f);
            uv.assign(count * 2, 0.0f);

            if (normIt != primitive.attributes.end())
                ReadFloats(source, normIt->second, 3, norm);

            if (uvIt != primitive.attributes.end())
                ReadFloats(source, uvIt->second, 2, uv);

            if (primitive.indices >= 0 &&
                !ReadIndices(source, primitive.indices, indices))
                continue;

            // Posicion, normal, UV, joints y weights
            std::vector<float> vertices;
            vertices.reserve(count * 16);

            for (size_t i = 0; i < count; i++)
            {
                for (int j = 0; j < 3; j++) vertices.push_back(pos[i * 3 + j]);
                for (int j = 0; j < 3; j++) vertices.push_back(norm[i * 3 + j]);
                for (int j = 0; j < 2; j++) vertices.push_back(uv[i * 2 + j]);
                for (int j = 0; j < 4; j++) vertices.push_back(joints[i][j]);
                for (int j = 0; j < 4; j++) vertices.push_back(weights[i * 4 + j]);
            }

            GLuint tex = 0;

            if (primitive.material >= 0)
            {
                const auto& mat = source.materials[primitive.material];
                int ti = mat.pbrMetallicRoughness.baseColorTexture.index;

                if (ti >= 0 && ti < (int)source.textures.size())
                {
                    int image = source.textures[ti].source;
                    if (image >= 0 && image < (int)textures.size())
                        tex = textures[image];
                }
            }

            AnimatedPrimitive p;
            p.nodeIndex = (int)ni;
            p.skinIndex = node.skin;
            p.texture = tex;
            p.indexed = !indices.empty();
            p.indexCount = (GLsizei)indices.size();

            glGenVertexArrays(1, &p.vao);
            glGenBuffers(1, &p.vbo);

            glBindVertexArray(p.vao);
            glBindBuffer(GL_ARRAY_BUFFER, p.vbo);

            glBufferData(
                GL_ARRAY_BUFFER,
                vertices.size() * sizeof(float),
                vertices.data(),
                GL_STATIC_DRAW
            );

            if (p.indexed)
            {
                glGenBuffers(1, &p.ebo);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, p.ebo);
                glBufferData(
                    GL_ELEMENT_ARRAY_BUFFER,
                    indices.size() * sizeof(uint32_t),
                    indices.data(),
                    GL_STATIC_DRAW
                );
            }

            GLsizei stride = 16 * sizeof(float);

            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
            glEnableVertexAttribArray(1);

            glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
            glEnableVertexAttribArray(2);

            glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, stride, (void*)(8 * sizeof(float)));
            glEnableVertexAttribArray(3);

            glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, stride, (void*)(12 * sizeof(float)));
            glEnableVertexAttribArray(4);

            glBindVertexArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

            primitives.push_back(p);
        }
    }

    // ANIMACION: primera animacion del archivo
    if (!source.animations.empty())
    {
        const auto& anim = source.animations[0];
        animationName = anim.name;

        for (const auto& channel : anim.channels)
        {
            const auto& sampler = anim.samplers[channel.sampler];

            if (sampler.interpolation != "LINEAR" &&
                sampler.interpolation != "STEP")
            {
                std::cout << "Interpolacion no implementada: "
                    << sampler.interpolation << std::endl;
                continue;
            }

            int components = channel.target_path == "rotation" ? 4 : 3;

            if (channel.target_path != "translation" &&
                channel.target_path != "rotation" &&
                channel.target_path != "scale")
                continue;

            std::vector<float> times, values;

            if (!ReadFloats(source, sampler.input, 1, times) ||
                !ReadFloats(source, sampler.output, components, values))
                continue;

            AnimationTrack track;
            track.node = channel.target_node;
            track.path = channel.target_path == "translation" ? 0 :
                channel.target_path == "rotation" ? 1 : 2;

            for (size_t i = 0; i < times.size(); i++)
            {
                track.times.push_back(times[i]);

                glm::vec4 value(0.0f);
                for (int j = 0; j < components; j++)
                    value[j] = values[i * components + j];

                track.values.push_back(value);
            }

            if (!times.empty())
                duration = std::max(duration, times.back());

            tracks.push_back(track);
        }
    }

    if (primitives.empty())
    {
        std::cout << "No se encontraron primitivas compatibles." << std::endl;
        Delete();
        return false;
    }

    loaded = true;
    paused = false;

    EvaluateAnimation(0.0f);

    std::cout << "Mountain Dragon cargado." << std::endl;
    std::cout << "Nodos: " << nodes.size() << std::endl;
    std::cout << "Articulaciones: " << jointNodeIndices.size() << std::endl;
    std::cout << "Pistas: " << tracks.size() << std::endl;
    std::cout << "Animacion: " << animationName << std::endl;
    std::cout << "Duracion: " << duration << " segundos" << std::endl;

    return true;
}

// ======================================================
// MATRICES Y ANIMACION
// ======================================================

glm::mat4 AnimatedModelGLTF::GetLocalMatrix(const AnimatedNode& n) const
{
    return glm::translate(glm::mat4(1.0f), n.translation) *
        glm::mat4_cast(n.rotation) *
        glm::scale(glm::mat4(1.0f), n.scale);
}

void AnimatedModelGLTF::UpdateNodeGlobals()
{
    std::function<void(int, const glm::mat4&)> visit =
        [&](int index, const glm::mat4& parent)
        {
            auto& n = nodes[index];

            n.local = GetLocalMatrix(n);
            n.global = parent * n.local;

            for (int child : n.children)
                visit(child, n.global);
        };

    for (size_t i = 0; i < nodes.size(); i++)
        if (nodes[i].parent < 0)
            visit((int)i, glm::mat4(1.0f));
}

void AnimatedModelGLTF::EvaluateAnimation(float time)
{
    for (const auto& track : tracks)
    {
        if (track.node < 0 || track.node >= (int)nodes.size() ||
            track.times.empty())
            continue;

        size_t b = 0;
        while (b < track.times.size() && track.times[b] < time)
            b++;

        size_t a = b == 0 ? 0 : b - 1;
        b = std::min(b, track.times.size() - 1);

        float t = 0.0f;

        if (a != b)
        {
            float interval = track.times[b] - track.times[a];
            if (interval > 0.0f)
                t = (time - track.times[a]) / interval;
        }

        t = glm::clamp(t, 0.0f, 1.0f);

        auto& n = nodes[track.node];

        if (track.path == 0)
        {
            n.translation = glm::vec3(
                glm::mix(track.values[a], track.values[b], t)
            );
        }
        else if (track.path == 2)
        {
            n.scale = glm::vec3(
                glm::mix(track.values[a], track.values[b], t)
            );
        }
        else
        {
            glm::quat q1(
                track.values[a].w,
                track.values[a].x,
                track.values[a].y,
                track.values[a].z
            );

            glm::quat q2(
                track.values[b].w,
                track.values[b].x,
                track.values[b].y,
                track.values[b].z
            );

            n.rotation = glm::normalize(glm::slerp(q1, q2, t));
        }
    }

    UpdateNodeGlobals();
}

void AnimatedModelGLTF::Update(float deltaTime)
{
    if (!loaded || paused || duration <= 0.0f)
        return;

    if (rangeFinished)
        return;

    animationTime += deltaTime;

    if (animationTime >= rangeEnd)
    {
        if (rangeLoop)
        {
            float length = rangeEnd - rangeStart;

            if (length > 0.0f)
            {
                animationTime = rangeStart +
                    std::fmod(animationTime - rangeStart, length);
            }
        }
        else
        {
            animationTime = rangeEnd;
            rangeFinished = true;
            paused = true;
        }
    }

    EvaluateAnimation(animationTime);
}

void AnimatedModelGLTF::SetAnimationRange(
    float start,
    float end,
    bool loop)
{
    if (!loaded || duration <= 0.0f)
        return;

    rangeStart = glm::clamp(start, 0.0f, duration);
    rangeEnd = glm::clamp(end, rangeStart, duration);

    rangeLoop = loop;
    rangeFinished = false;
    paused = false;

    animationTime = rangeStart;

    EvaluateAnimation(animationTime);
}

bool AnimatedModelGLTF::IsRangeFinished() const
{
    return rangeFinished;
}


// ======================================================
// CONTROLES MANUALES
// ======================================================

void AnimatedModelGLTF::SetPaused(bool value)
{
    paused = value;
}

bool AnimatedModelGLTF::IsPaused() const
{
    return paused;
}

void AnimatedModelGLTF::SetAnimationTime(float time)
{
    if (!loaded || duration <= 0.0f)
        return;

    animationTime = glm::clamp(time, 0.0f, duration);

    rangeFinished = false;

    EvaluateAnimation(animationTime);
}

void AnimatedModelGLTF::ResetAnimation()
{
    animationTime = 0.0f;
    paused = false;
    EvaluateAnimation(0.0f);
}

// ======================================================
// DIBUJAR MODELO
// ======================================================

void AnimatedModelGLTF::Draw(
    GLuint shaderProgram,
    const glm::mat4& viewProjection,
    const glm::mat4& modelMatrix)
{
    if (!loaded) return;

    GLint modelLoc = glGetUniformLocation(shaderProgram, "uModel");
    GLint mvpLoc = glGetUniformLocation(shaderProgram, "uMVP");
    GLint jointsLoc = glGetUniformLocation(shaderProgram, "uJoints[0]");

    // Matrices de articulaciones
    std::vector<glm::mat4> palette;

    for (size_t i = 0; i < jointNodeIndices.size() && i < 128; i++)
    {
        int node = jointNodeIndices[i];

        if (node < 0 || node >= (int)nodes.size())
            continue;

        glm::mat4 ibm = i < inverseBindMatrices.size()
            ? inverseBindMatrices[i] : glm::mat4(1.0f);

        palette.push_back(nodes[node].global * ibm);
    }

    if (jointsLoc >= 0 && !palette.empty())
    {
        glUniformMatrix4fv(
            jointsLoc,
            (GLsizei)palette.size(),
            GL_FALSE,
            glm::value_ptr(palette[0])
        );
    }

    for (const auto& p : primitives)
    {
        glm::mat4 finalModel =
            modelMatrix * nodes[p.nodeIndex].global;

        glm::mat4 mvp = viewProjection * finalModel;

        if (modelLoc >= 0)
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(finalModel));

        if (mvpLoc >= 0)
            glUniformMatrix4fv(mvpLoc, 1, GL_FALSE, glm::value_ptr(mvp));

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, p.texture);

        GLint texLoc = glGetUniformLocation(shaderProgram, "tex0");
        if (texLoc >= 0)
            glUniform1i(texLoc, 0);

        glBindVertexArray(p.vao);

        if (p.indexed)
            glDrawElements(GL_TRIANGLES, p.indexCount, GL_UNSIGNED_INT, 0);

        glBindVertexArray(0);
    }
}

// ======================================================
// LIBERAR RECURSOS
// ======================================================

void AnimatedModelGLTF::Delete()
{
    for (auto& p : primitives)
    {
        if (p.ebo) glDeleteBuffers(1, &p.ebo);
        if (p.vbo) glDeleteBuffers(1, &p.vbo);
        if (p.vao) glDeleteVertexArrays(1, &p.vao);
    }

    primitives.clear();

    for (GLuint texture : textures)
        if (texture) glDeleteTextures(1, &texture);

    textures.clear();
    nodes.clear();
    tracks.clear();
    inverseBindMatrices.clear();
    jointNodeIndices.clear();

    duration = 0.0f;
    animationTime = 0.0f;
    animationName.clear();

    paused = false;
    loaded = false;
    skinRoot = -1;
}
