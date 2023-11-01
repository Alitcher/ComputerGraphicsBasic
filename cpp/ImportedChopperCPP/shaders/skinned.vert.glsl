#version 400

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat4 boneMatrices[57]; //57 bones in the Marine (if one of those bones would happen to fall...)
uniform vec3 lightPosition;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec3 color;

layout(location = 3) in ivec4 boneIds;
layout(location = 4) in vec4 boneWeights;
layout(location = 5) in vec2 uv;

out vec3 interpolatedPosition;
out vec3 interpolatedNormal;
out vec3 interpolatedColor;
out vec2 interpolatedUv;

void main(void) {
    mat4 modelViewMatrix = viewMatrix * modelMatrix;

    vec4 weights = normalize(boneWeights);
    mat4 boneMatrix = mat4(0.0);
    boneMatrix += weights[0] * boneMatrices[boneIds[0]];
    boneMatrix += weights[1] * boneMatrices[boneIds[1]];
    boneMatrix += weights[2] * boneMatrices[boneIds[2]];
    boneMatrix += weights[3] * boneMatrices[boneIds[3]];

    gl_Position = projectionMatrix * modelViewMatrix * boneMatrix * vec4(position, 1.0);

    mat3 normalMatrix = transpose(inverse(mat3(modelViewMatrix)));
    interpolatedNormal = normalize(normalMatrix * normal);

    interpolatedColor = color;
    interpolatedUv = uv;
}
