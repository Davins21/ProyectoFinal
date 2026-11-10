
#version 450 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec3 Normal;
out vec2 TexCoord;
out vec3 FragPos;

uniform mat4 uMVP;
uniform mat4 uModel;
uniform mat4 uNodeTransform;

void main()
{
    // Transformación combinada del modelo y del nodo
    mat4 finalModel = uModel * uNodeTransform;

    // Posición en el espacio del mundo
    FragPos = vec3(finalModel * vec4(aPos, 1.0));

    // Transformación correcta de las normales
    Normal = mat3(transpose(inverse(finalModel))) * aNormal;

    // Coordenadas de textura
    TexCoord = aTexCoord;

    // Posición final en pantalla
    gl_Position = uMVP * uNodeTransform * vec4(aPos, 1.0);
}
