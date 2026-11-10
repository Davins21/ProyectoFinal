#version 450 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aNormal;
layout(location = 2) in vec2 aTexCoord;
layout(location = 3) in vec4 aJoints;
layout(location = 4) in vec4 aWeights;

out vec3 Normal;
out vec2 TexCoord;
out vec3 FragPos;

uniform mat4 uMVP;
uniform mat4 uModel;
uniform mat4 uJoints[128];

void main()
{
    ivec4 joints = ivec4(aJoints);
    mat4 skin = mat4(0.0);

    skin += aWeights.x * uJoints[joints.x];
    skin += aWeights.y * uJoints[joints.y];
    skin += aWeights.z * uJoints[joints.z];
    skin += aWeights.w * uJoints[joints.w];

    // Algunos vértices pueden no tener pesos válidos.
    float totalWeight = dot(aWeights, vec4(1.0));
    vec4 skinnedPosition = (totalWeight > 0.0001)
        ? skin * vec4(aPos, 1.0)
        : vec4(aPos, 1.0);

    vec3 skinnedNormal = (totalWeight > 0.0001)
        ? mat3(skin) * aNormal
        : aNormal;

    vec4 worldPosition = uModel * skinnedPosition;
    FragPos = worldPosition.xyz;
    Normal = mat3(transpose(inverse(uModel))) * skinnedNormal;
    TexCoord = aTexCoord;

    gl_Position = uMVP * skinnedPosition;
}