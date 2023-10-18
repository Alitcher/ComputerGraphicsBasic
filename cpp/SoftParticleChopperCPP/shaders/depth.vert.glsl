#version 400

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform vec2 frustum;

layout(location = 0) in vec3 position;


void main(void) {
    vec4 viewPos = viewMatrix * modelMatrix * vec4(position, 1.0);

    //If you decided to use a normalized camera space z, then here you should normalize it as well.

    gl_Position = projectionMatrix * viewPos;
}
