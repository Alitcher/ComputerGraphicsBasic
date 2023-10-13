#version 400

uniform vec3 lightPosition;

in vec3 interpolatedNormal;
in vec3 interpolatedPosition;
in vec3 interpolatedColor;

// This is the output variable of the fragment shader
// We configure this to be "output 0" for our fragment shader
// in the main program using glBindFragDataLocation
out vec4 fragColor;

void main(void) {

    /**
     * --Task--
     * Do the same Phong's lighting/reflection model calculation that you did in Gouraud vertex shader before.
     */
    vec3 color = interpolatedColor; //Assign your calculation here instead

    fragColor = vec4(color, 1.0);

}
