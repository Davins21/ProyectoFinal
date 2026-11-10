#ifndef ANIMATED_MODEL_GLTF_H
#define ANIMATED_MODEL_GLTF_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <string>
#include <vector>
#include <cstdint>

struct AnimatedPrimitive
{
    GLuint vao = 0, vbo = 0, ebo = 0, texture = 0;
    GLsizei indexCount = 0;
    bool indexed = false;
    int nodeIndex = -1;
    int skinIndex = -1;
};

struct AnimatedNode
{
    int parent = -1;
    std::vector<int> children;
    int mesh = -1;
    int skin = -1;
    glm::vec3 translation{ 0.0f };
    glm::quat rotation{ 1.0f,0.0f,0.0f,0.0f };
    glm::vec3 scale{ 1.0f };
    glm::mat4 baseMatrix{ 1.0f };
    bool hasMatrix = false;
    glm::mat4 local{ 1.0f };
    glm::mat4 global{ 1.0f };
};

struct AnimationTrack
{
    int node = -1;
    int path = 0; // 0 translation, 1 rotation, 2 scale
    std::vector<float> times;
    std::vector<glm::vec4> values;
};

class AnimatedModelGLTF
{
public:
    bool Load(const std::string& filename);
    void Update(float deltaTime);
    void Draw(GLuint shaderProgram, const glm::mat4& viewProjection, const glm::mat4& modelMatrix);
    void ResetAnimation();
    void SetPaused(bool value);
    bool IsPaused() const;
    void SetAnimationRange(float start, float end, bool loop);
    bool IsRangeFinished() const;
    void SetAnimationTime(float time);
    void Delete();

    float GetDuration() const { return duration; }
    float GetAnimationTime() const { return animationTime; }
    const std::string& GetAnimationName() const { return animationName; }

private:
    std::vector<AnimatedPrimitive> primitives;
    std::vector<AnimatedNode> nodes;
    std::vector<glm::mat4> inverseBindMatrices;
    std::vector<int> jointNodeIndices;
    std::vector<AnimationTrack> tracks;
    std::vector<GLuint> textures;

    float rangeStart = 0.0f;
    float rangeEnd = 0.0f;
    bool rangeLoop = true;
    bool rangeFinished = false;
    int skinRoot = -1;
    float duration = 0.0f;
    float animationTime = 0.0f;
    std::string animationName;
    bool loaded = false;
    bool paused = false;

    void EvaluateAnimation(float time);
    void UpdateNodeGlobals();
    glm::mat4 GetLocalMatrix(const AnimatedNode& node) const;
};

#endif
