
#version 450 core

in vec3 Normal;
in vec2 TexCoord;
in vec3 FragPos;

out vec4 FragColor;

uniform sampler2D tex0;
uniform vec4 uColorTint;

uniform vec3 sunDirection;
uniform vec3 sunColor;
uniform float ambientStrength;

uniform int isSky;

// Luz localizada
uniform vec3 pointLightPosition;
uniform vec3 pointLightColor;
uniform float pointLightIntensity;
uniform float pointLightRadius;

void main()
{
    vec4 texColor = texture(tex0, TexCoord);

    float alpha = texColor.a * uColorTint.a;

    // El cielo conserva su textura original
    if (isSky == 1)
    {
        FragColor = vec4(
            texColor.rgb * uColorTint.rgb,
            alpha
        );
        return;
    }

    vec3 norm = normalize(Normal);

    // Luz solar direccional
    vec3 lightDir = normalize(sunDirection);

    vec3 ambient = ambientStrength * sunColor;

    float diff = max(dot(norm, lightDir), 0.0);

    vec3 diffuse = diff * sunColor;

    // Luz puntual localizada
    vec3 toPointLight = pointLightPosition - FragPos;

    float distanceToLight = length(toPointLight);

    vec3 pointDir = normalize(toPointLight);

    float pointDiff = max(dot(norm, pointDir), 0.0);

    // Atenuación según la distancia
    float attenuation = pow(
        max(1.0 - distanceToLight / pointLightRadius, 0.0),
        2.0
    );

    vec3 pointContribution =
        pointLightColor *
        pointLightIntensity *
        pointDiff *
        attenuation;

    // Combinar las dos iluminaciones
    vec3 finalLight =
        ambient +
        diffuse +
        pointContribution;

    vec3 finalColor =
        finalLight *
        texColor.rgb *
        uColorTint.rgb;

    FragColor = vec4(finalColor, alpha);
}
