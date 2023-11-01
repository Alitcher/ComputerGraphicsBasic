#version 400

uniform mat4 modelMatrix;
uniform mat4 viewMatrix;
uniform vec3 lightPosition;
uniform sampler2D texture;
uniform sampler2D bumpTexture;

in vec3 interpolatedNormal;
in vec3 interpolatedPosition;
in vec2 interpolatedUv;

void main(void) {
    mat3 normalMatrix = transpose(inverse(mat3(viewMatrix * modelMatrix))); //You will need this here.
                                                                            //Now I guess it would be better to send it as an uniform...
    float stepSize = 0.01; // You might need to adjust this value for different bump maps

    // 1. Gradient Vector Calculation
    float left = texture2D(bumpTexture, interpolatedUv - vec2(stepSize, 0)).r;
    float right = texture2D(bumpTexture, interpolatedUv + vec2(stepSize, 0)).r;
    float down = texture2D(bumpTexture, interpolatedUv - vec2(0, stepSize)).r;
    float up = texture2D(bumpTexture, interpolatedUv + vec2(0, stepSize)).r;
    vec2 gradient = vec2(right - left, up - down);


    // 2. Normal Modification
    vec3 normalShift = vec3(gradient, 0.0);
    normalShift = transpose(inverse(mat3(viewMatrix * modelMatrix))) * normalShift;
    vec3 bumpedNormal = normalize(interpolatedNormal - normalShift);

    //If you want to test with directional light, you could specify a light source direction here like: vec3 l = normalize(vec3(1.0, 1.0, 1.0));
    // 3. Texture UV Modification
    vec2 bumpedUv = interpolatedUv + 0.05 * gradient;
    vec4 sampleColor = texture2D(texture, bumpedUv);

    // Using Phong Lighting Model with bumped normal
    vec3 ambient = 0.1 * sampleColor.rgb;
    vec3 lightDir = normalize(lightPosition - interpolatedPosition);
    float diff = max(dot(bumpedNormal, lightDir), 0.0);
    vec3 diffuse = diff * sampleColor.rgb;
    vec3 viewDir = normalize(-interpolatedPosition);
    vec3 reflectDir = reflect(-lightDir, bumpedNormal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = vec3(0.5) * spec;
    vec3 color = ambient + diffuse + specular;

    /**
     *  --Task--
     *   1. Find the gradient vector
     *   1.1. Create some small step size. In reality this would depend on your bump map size.
     *        For now just write some 0.001 or 0.01 for it. The bumpTexture.png and the jpg images have a different resolution.
     *        You might need to change it when switching the maps.
     *   1.2. Sample from 1 step size from the right of the current UV coordinate, subtract from it the sample 1 step size to the left.
     *   1.3. Sample from 1 step size from the up of the current UV coordinate, subtract from it the sample 1 step size to the down.
	 *					 (Alternatively you can use another finite difference method for steps 1.2 and 1.3)
	 *	 1.4. Construct the vec2 gradient based on the finite differences just found.
     *
     *   2. Our plane is locally located at the xy plane. This means that in local coordinates, z is 0.
     *      Construct the normalShift vector from the gradient. This we will be adding to the normal vector.
     *   2.1. To be totally correct, you should multiply that normalShift vector with the normalMatrix.
     *        Because the camera is looking straight down unto the xy plane (where our plane is also located), the change is not that visible.
     *        But if the conditions were different, the local shift would not be the same as the shift in eye space (where our base normal is).
     *        To see this effect, change the plane rotation to [-45, 45] degrees and output the normalShift as the color.
     *   2.2 Subtract that from the normal and normalize (or add it if you have found the gradient the other way).
     *
     *   3. If we want the effect that the surface is deformed, then we should also deform the colors sampled from the texture.
     *      One simple way to do this is to add a small factor of the normalShift.xy (the one that is in local space, not eye space!) from the UV coordinates.
     *      The exact factor depends again on the bump map itself. You can try 0.05 for example.
     *      If you shift the UV by the shift that is in eye space, then there will be weird effects, if the situation is different.
     *      Try again by rotating the plane in [-45, 45] and using a directional light to investigate.
     *
     *   4. Copy your Phong or Blinn here and use the color and normal we just found.
     */

    gl_FragColor = vec4(color, sampleColor.a);
}
