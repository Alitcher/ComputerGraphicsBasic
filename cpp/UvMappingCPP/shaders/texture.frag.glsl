#version 400

uniform vec3 lightPosition;
uniform sampler2D myTexture; //Receive the texture uniform.

/**
 * --Task--
 * Receive the texture uniform.
 * - Find out what type is it
 * - You can use any name for it, because there is only one texture at a time
 */

in vec3 interpolatedNormal;
in vec3 interpolatedPosition;
in vec2 interpolatedUv;


void main(void) {
    vec3 viewerPosition = vec3(0.0);
    vec4 sampleColor =  texture(myTexture, interpolatedUv);
    /**
     * --Task--
     * Copy your Phong or Blinn lighting model here.
     * Use the diffuse and ambient color from the texture - sample it from the correct coordinates.
     */

    // Calculate lighting using Phong model
    vec3 ambient = 0.1 * sampleColor.rgb;
    vec3 lightDir = normalize(lightPosition - interpolatedPosition);
    float diff = max(dot(interpolatedNormal, lightDir), 0.0);
    vec3 diffuse = diff * sampleColor.rgb;

    vec3 viewDir = normalize(viewerPosition - interpolatedPosition);
    vec3 reflectDir = reflect(-lightDir, interpolatedNormal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = vec3(0.5) * spec;

    vec3 color = ambient + diffuse + specular;
    gl_FragColor = vec4(color, sampleColor.a);
}
