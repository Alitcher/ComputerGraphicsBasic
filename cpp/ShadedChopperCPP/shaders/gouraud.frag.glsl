#version 400

uniform vec3 lightPosition;
uniform vec3 viewerPosition;

in vec3 interpolatedColor;

// This is the output variable of the fragment shader, we assign the pixel color there
out vec4 fragColor;

const float gamma = 2.2;

vec3 GammaCorrect()
{
    return pow(interpolatedColor, vec3(1.0/gamma));
}

// the interpolated colors are already in linear space, so no need to decode.

void main(void) {
    vec3 gammaCorrectedColor = GammaCorrect();
    fragColor = vec4(gammaCorrectedColor, 1.0);
}
