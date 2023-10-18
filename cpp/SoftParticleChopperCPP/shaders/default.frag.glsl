#version 400

uniform vec3 lightPosition;

in vec3 interpolatedColor;
in vec3 interpolatedNormal;
in vec3 interpolatedPosition;

void main(void) {

    /**
     * --Task--
     * Copy your Phong or Blinn-Phong lighting model here.
     */

    gl_FragColor = vec4(interpolatedColor, 1.0);
}
