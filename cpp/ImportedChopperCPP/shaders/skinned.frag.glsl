#version 400

uniform sampler2D texture;
uniform vec3 lightPosition;

in vec3 interpolatedColor;
in vec3 interpolatedNormal;
in vec3 interpolatedPosition;
in vec2 interpolatedUv;

void main(void) {

    /**
     * --Optional Task--
     * You can try some specular highlights (either Phong or Blinn-Phong) here as well.
     */
    vec3 removeThis = lightPosition;
    vec3 l = normalize(lightPosition - interpolatedPosition);
    vec3 n = normalize(interpolatedNormal);

    vec3 color = texture2D(texture, interpolatedUv).rgb;
    color = color * (0.2 + max(0.0, dot(n, l)));

    gl_FragColor = vec4(color, 1.0);
}
