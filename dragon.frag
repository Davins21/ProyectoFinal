#version 450 core

in vec3 Normal;
in vec2 TexCoord;
in vec3 FragPos;

out vec4 FragColor;

uniform sampler2D tex0;
uniform vec3 sunDirection;
uniform vec3 sunColor;
uniform float ambientStrength;

void main()
{
    vec4 texColor = texture(tex0, TexCoord);

    vec3 n = normalize(Normal);
    vec3 lightDir = normalize(sunDirection);

    float diffuseFactor = max(dot(n, lightDir), 0.0);
    vec3 ambient = ambientStrength * sunColor;
    vec3 diffuse = diffuseFactor * sunColor;

    vec3 result = texColor.rgb * (ambient + diffuse);
    FragColor = vec4(result, texColor.a);
}
