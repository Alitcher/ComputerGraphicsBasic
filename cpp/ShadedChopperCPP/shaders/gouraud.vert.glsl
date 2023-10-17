#version 400

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform vec3 lightPosition;

layout(location=0) in vec3 position; //Need to define specific attribute locations
layout(location=1) in vec3 color;    //In order to switch the shaders
layout(location=2) in vec3 normal;   //Between Phong and Gouraud

out vec3 interpolatedColor; //We want to interpolate the color between the vertices
out vec3 interpolatedNormal;

void main(void) {
    mat4 modelViewMatrix = viewMatrix * modelMatrix;               //Used to transform points from local space to camera space
    mat3 normalMatrix = transpose(inverse(mat3(modelViewMatrix))); //Used to transform normal vectors from local space to camera space

    gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(position, 1.0);


    /**
     * --Task--
     *
     * 1. Calculate the vertexPosition in the camera space.
     * 2. Calculate the correct normal in the camera space.
     * 3. Find the direction towards the viewer, normalize.
     * 4. Find the direction towards the light source, normalize.
     *
     * Implement Phong's lighting / reflection model:
     * - Ambient term you can just add
     * - Use the values calculated before for the diffuse/Lambertian term
     * - For Phong's specular, find the reflection of the light ray from the point
     *   See: https://www.opengl.org/sdk/docs/man/html/reflect.xhtml
     *   Use a value like 200 for the shininess
     */

     // 1. Calculate the vertexPosition in the camera space.
     // 2. Calculate the correct normal in the camera space.
     // 3. Find the direction towards the viewer, normalize.
     // 4. Find the direction towards the light source, normalize.
    vec3 vertexPosition = vec3(modelViewMatrix * vec4(position,1.0));
    vec3 correctedNormal = normalize(normalMatrix * normal);
    vec3 viewDirection = normalize(-vertexPosition);
    vec3 lightDirection = normalize(lightPosition - vertexPosition);

    vec3 ambient = color * 0.1; // Assuming 10% ambient light

    // Diffuse/Lambertian term
    float diffuseValue = max(dot(correctedNormal, lightDirection), 0.0);
    vec3 diffuse = diffuseValue * color;

    vec3 reflectionDirection = reflect(-lightDirection, correctedNormal);

    float clampedDotProduct = max(dot(viewDirection, reflectionDirection),0.0);


    float powDot = pow(clampedDotProduct, 200.0);

    vec3 specular = powDot * vec3(1.0, 1.0, 1.0); // White specular highlights

    // Combining the results
    interpolatedColor = ambient + diffuse + specular;
    interpolatedNormal = correctedNormal;
}
