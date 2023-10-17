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
    // Normalize the interpolated normal
    vec3 N = normalize(interpolatedNormal);

    // Calculate the light direction
    vec3 L = normalize(lightPosition - interpolatedPosition);

    // Calculate the reflection direction
    vec3 R = reflect(-L, N);

    // Calculate the view direction
    vec3 V = normalize(-interpolatedPosition);

    // Ambient term
    vec3 ambient = 0.1 * interpolatedColor; // Assuming 10% ambient light

    // Diffuse term
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = diff * interpolatedColor;

    // Specular term
    float spec = pow(max(dot(R, V), 0.0), 200.0); // Using 200 as shininess value
    vec3 specular = spec * vec3(1.0, 1.0, 1.0); // White specular color


    vec3 color = ambient + diffuse + specular; //Assign your calculation here instead

    fragColor = vec4(color, 1.0);

}
