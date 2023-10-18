#version 400

uniform vec3 lightPosition;

in vec3 interpolatedNormal;
in vec3 interpolatedPosition;
in vec3 interpolatedColor;

// This is the output variable of the fragment shader
// We configure this to be "output 0" for our fragment shader
// in the main program using glBindFragDataLocation
out vec4 fragColor;

vec3 GammaCorrect(vec3 color, float gamma)
{
    return pow(color, vec3(gamma));
}

void main(void) {

    /**
     * --Task--
     * Do the same Phong's lighting/reflection model calculation that you did in Gouraud vertex shader before.
     */
    // Normalize the interpolated normal
    vec3 N = normalize(interpolatedNormal);

    // Calculate the light direction
    vec3 L = normalize(lightPosition - interpolatedPosition);

    // Calculate the view direction
    vec3 V = normalize(-interpolatedPosition);

    // Calculate the reflection direction
    //vec3 R = reflect(-L, N);

    // Calculate the half-angle vector
    vec3 H = normalize(L + V);


    vec3 ambient = 0.1 * interpolatedColor; // Assuming 10% ambient light
    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = diff * interpolatedColor;

    //Phong Specular term
    //float spec = pow(max(dot(R, V), 0.0), 200.0); // Using 200 as shininess value

    // Blinn-Phong Specular term
    float spec = pow(max(dot(H, N), 0.0), 200.0);
    vec3 specular = spec * vec3(1.0, 1.0, 1.0); // White specular color

    // Decode the interpolated color from gamma space to linear space
    vec3 decodedColor = GammaCorrect(interpolatedColor, 2.2); // Convert from gamma space to linear space

    // Perform lighting calculations using the decoded color
    vec3 colorLinearSpace = ambient + diffuse * decodedColor + specular;

    // Encode the final color back to gamma space before writing to the output
    vec3 colorGammaSpace = GammaCorrect(colorLinearSpace, 1.0 / 2.2);  // Convert from linear space to gamma space
    fragColor = vec4(colorGammaSpace, 1.0);

}
