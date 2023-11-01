import * as THREE from 'three';
import { GLTFLoader } from 'three/addons/loaders/GLTFLoader.js';
import { ColladaLoader } from 'three/addons/loaders/ColladaLoader.js';
import { MTLLoader } from 'three/addons/loaders/MTLLoader.js';
import { OBJLoader } from 'three/addons/loaders/OBJLoader.js';
import { THREEx } from './THREEx.KeyboardState.js';

// The usual three
var renderer, scene, camera;

// We have those main objects: chopper from .obj+.mtl, chopper from .dae (collada) and a marine from .gltf file.
var chopperOBJ, chopperCollada, marine;
var assetsToLoad = 1;
var textureLoader = new THREE.TextureLoader();

var lightTrajectory;
var light;

// We use the clock to measure time, an extension for the keyboard
var clock = new THREE.Clock();
var keyboard = new THREEx.KeyboardState();
var speed = 0; // Current marine speed

var statusContainer;

function toRad(degree) {

    return Math.PI * 2 * degree / 360;
}

window.onLoad = async () => {    
    var canvasContainer = document.getElementById('myCanvasContainer');
    var width = 800;
    var height = 500;

    // Initially we show the loading text under the canvas
    statusContainer = document.getElementById('status');
    statusContainer.innerHTML = 'Loading...';

    renderer = new THREE.WebGLRenderer();
    renderer.setSize(width, height);
    renderer.outputEncoding = THREE.sRGBEncoding; // You can try with (more correct way) and without output gamma correction
    canvasContainer.appendChild(renderer.domElement);

    scene = new THREE.Scene();

    // Create a perspective camera.
    // We are going to modify the camera for a third-person view later
    camera = new THREE.PerspectiveCamera(80, width / height, 1, 1000);
    camera.up = new THREE.Vector3(0, 1, 0);
    camera.position.set(0, 0, 0);

    // We add a red point light
    lightTrajectory = new THREE.CatmullRomCurve3([
        new THREE.Vector3(0, 7, -8),
        new THREE.Vector3(-2, 3, -5),
        new THREE.Vector3(-3, 6, 36),
        new THREE.Vector3(8, 4, 39),
    ]);
    lightTrajectory.closed = true;
    light = new THREE.PointLight(0xaa3333);
    scene.add(light);

    // And a white directional light
    var directionalLight = new THREE.DirectionalLight(0xffffff);
    directionalLight.position.set(1, 1, 1);
    scene.add(directionalLight);

    // This is needed for texture loading from the web
    textureLoader.crossOrigin = 'Anonymous';

    console.log('Assets to load: ' + assetsToLoad + ' (change that value if you have implemented less assets)');

    addHangar();

    /**
     * --Task--
     * Load the OBJ and MTL chopper, for that:
     * 1) Load the MTL with MTLLoader
     * 2) On the loaded callback of the MTLLoader load the OBJ with OBJLoader
     * 3) Set the materials of the objLoader to be the materials loaded by the mtlLoader
     * Use the function "loadChopperObj" as the final callback.
     */

    /**
     * --Task--
     * Load the Collada chopper with the ColladaLoader
     * Use the function "loadChopperCollada" as the callback.
     */

    /**
     * --Task--
     * Load the GLTF Marine object with GLTFLoader()
     * Don't forget to set: "loader.crossOrigin = 'Anonymous';"
     * Address is:
     * GLTF: https://cglearn.eu/files/course/9/models/marine/marine.gltf
     * Use the function "loadMarine" as the callback
     *
     * The texture is also already packed into that file. If you want to check out how the texture looks like, see:
     * https://cglearn.eu/files/course/9/models/marine/MarineCv2_color_small.jpg
     */
}

/**
 * --Task--
 * Finish this function.
 * This function gets the data of the objects in the .obj file. The objects are grouped under a common parent, which is sent here.
 */
function loadChopperObj(chopper) {

    // Create a new Group with a name "ChopperOBJ".
    chopperOBJ = new THREE.Group();
    chopperOBJ.name = 'ChopperOBJ';

    // Give the "chopper" object a value "Chopper" for the name attribute.
    chopper.name = 'Chopper';

    // OBJ does not store object hierarchy, that's why now:
    // - Take all the parts (eg. Blade1, Blade2, Blades) by name from the chopper.
    // - Remove them from the chopper, add them back hierarchically
    // - Create the "Blades" Group if there is none for that.

    // Scale the chopper with 0.8 (e.g) coefficients in all axes (so that it will fit in the hangar)
    // Rotate the chopper 90 degrees around the Y axis, so that it will be sideways
    // Position the chopper 10 units down on the Y axis

    // Add the chopper to the chopperOBJ
    // Add the chopperOBJ to the scene

    if (!(--assetsToLoad)) {
        loaded();
    }
}

/**
 * --Task--
 * Finish this function.
 * This function gets the colladaObject that is a scene graph node of the entire hierarchy described in the modeling software.
 * We have described the chopper hierarchy and Collada format stores that info.
 */
function loadChopperCollada(colladaObject) {

    // We assign the Collada scene node to the chopperCollada variable
    chopperCollada = colladaObject.scene;

    // Next, give the chopperCollada a name "ChopperCollada"
    // Position it 20 units along the Z axis, so that it doesn't collide with the OBJ chopper

    // Fetch the chopper from the chopperCollada by its name "Chopper" (it is already there, because Collada has stored it)
    // Set the chopper scale to 0.8 in all axes
    // Rotate the chopper 90 degrees around the Y axis, so that it is sideways

    // Add the chopperCollada to the scene

    // You may want to use this to see what data Collada can give us:
    // console.log(colladaObject);

    if (!(--assetsToLoad)) {
        draw();
    }
}

/**
 * --Task--
 * Finish this function.
 * Here we load a GLTF Marine together with different animations it has stored in the GLTF format.
 * There are different things packed into the GLTF format. We will need to find and use the mesh and animations.
 * Note: Previously this part was with Three.js-s JSON format.
 */
function loadMarine(gltf) {
    // 0) Try console.log(gltf) to see what is actually loaded.


    // 1) Find out where is the loaded Marine's mesh. Then assign it into the "marine" variable.
    //    The Marine's mesh is of type SkinnedMesh. This means the geometry is tied to a skeleton (armature) and can be animated by it.


    // 2) Set the marine object's name attribute to "marine" 


    // 3) Find out where are the loaded animations. Add them to the marine object under an "animations" attribute.
    //    In JavaScript new attributes can be added to objects at runtime.


    // 4) Create a new THREE.AnimationMixer object for the marine. Hint: new THREE.AnimationMixer(marine);
    //    Assign it also to the marine, under some "animationMixer" attribute.


    // 5) Loop through the AnimationClip objects and use the created mixer's .clipAction(action) and .existingActionn(action.name) methods to:
    for (var i = 0; i < gltf.animations.length; ++i) {
        var animAction = gltf.animations[i];

        // 5.1) Set the clips to the mixer as actions


        // 5.2) Set all the clips to play

    }

    // 6) That marine is big, scale it with 0.015 in all axes
    //   Set the position to (0, -10, 80), so that it will be in the far end of the hangar


    // 7) Add the marine to our scene. 
    //    Note that this removes it from the loaded GLTF scene.


    if (!(--assetsToLoad)) {
        loaded();
    }
}

/**
 * This function loads the floor texture.
 */
function onTextureLoaded(texture) {
    texture.wrapS = THREE.RepeatWrapping;
    texture.wrapT = THREE.RepeatWrapping;
    texture.repeat.set(2, 10);
    texture.encoding = THREE.sRGBEncoding;
    texture.needsUpdate = true;

    if (!(--assetsToLoad)) {
        loaded();
    }
}

/**
 * Things are loaded. Initialize draw loop.
 */
function loaded() {
    statusContainer.innerHTML = 'Running.';
    draw();
}

/**
 * Here we parse some controls and specify some speeds based on the input.
 */
function parseControls(dt) {
    if (keyboard.pressed("left")) {
        marine.rotation.set(0, marine.rotation.y + toRad(60 * dt % 360), 0);
    }

    if (keyboard.pressed("right")) {
        marine.rotation.set(0, marine.rotation.y - toRad(60 * dt % 360), 0);
    }

    if (keyboard.pressed("up")) {
        speed += 3 * dt;
    }

    if (keyboard.pressed("down")) {
        speed -= 3 * dt;
    }
    
    speed = Math.min(Math.max(0, speed), 10);
}

/**
 * --Task--
 * Finish this function.
 * We are going to blend together the idle, walk and run animations based on the speed of the marine.
 * Use the Bernstein polynomials to blend the animations together: http://en.wikipedia.org/wiki/Bernstein_polynomial
 */
function updateMarineAnimationBlend(dt, speed) {
    // Update the mixer for the marine with dt: mixer.update(dt)

    // All the animations have some length (period) in seconds
    var idleLength = 3.96667;
    var walkLength = 1.0;
    var runLength = 0.733333;
    var normLength;

    // The idea is, that if we blend the animations together, then we need to sync the animation lengths.
    // If the animations are not synced, then:
    //  - run animation might say that the right leg should be bended forward, 
    //  - walk animation thinks it should bended backward
    //  - result is that the leg staggers somewhere in between, this is bad
    var w_idle = 0;
    var w_walk = w_idle;
    var w_run = 1;

    //If the speed is between 0 and 4, then we do some blending
    if (speed >= 0 && speed < 4) {
        // Find the main weight (percentage between 0 and 4 of the current speed)

        // Assign w_idle, w_walk and w_run using the the Bernstein basis polynomials.

        // Use w_idle, w_walk and w_run as the .weight parametrs of the corresponding animation actions in the marine's mixer object
        // You can use marine.mixer.clipAction("name") to get the action from the mixer.

        // Find also the blended animation length by taking a convex combination of lengths of different animations
        // Call it "normLength" for example.

        // Find what percentage the different lengths are compared to the normLength
        // Assign a corresponding .timeScale value for each of the animation actions

        // Think about it this way, if we are running, then the length of the total animation interval should be 0.7(3) seconds
        // In order to blend the walk animation into it, we need to increase the speed of the walk animation ~1.364 times.

        // Here is some debugging:
        document.getElementById('stat').innerText = (Math.round(100 * w_idle) / 100) + ' vs ' + (Math.round(100 * w_walk) / 100) + ' vs ' + (Math.round(100 * w_run) / 100);

    } else if (speed >= 4) {
        w_idle = 0;
        w_walk = 0;
        w_run = 1;
    }

    mixer.clipAction('idle').weight = w_idle;
    mixer.clipAction('walk').weight = w_walk;
    mixer.clipAction('run').weight = w_run;
}

/**
 * --Task--
 * Finish this function.
 * This function should move the marine according to the speed and the current orientation.
 * Also the speed should decrease by some percentage
 * Finally we need to position the camera to be in the third-person view behind the marine.
 */
function updateMarinePosition(dt) {
    // Find the "forward" vector based on the marine's current rotation around Y axis.
    // You can take the world's forward vector, do applyAxisAngle on it.

    // Then multiply that forward vector with this: "speed * speed * dt / 2"
    // It may not be physically correct and you can try out different things here. It just gave an ok result.

    // Move the marine by adding the found movement vector to the position

    // I assume that the center, that we want to look at is somewhere here:
    // var center = new THREE.Vector3().copy(marine.position).add(new THREE.Vector3(0, 3, 0));

    // Find the backward vector, multiply it by 3.5. Then move 4 units up.
    // That would be the position of the camera.

    // Update the camera position and lookAt.

    // Damp the speed (slow down)
    speed -= speed * dt / 2.0;
}


function draw() {
    var dt = clock.getDelta();

    var time = clock.getElapsedTime(); //Take the time
    requestAnimationFrame(draw);

    parseControls(dt);
    updateMarineAnimationBlend(dt, speed);
    updateMarinePosition(dt);

    /**
     * --Task--
     * Make both of the choppers move up-down with a sine function (or some other way, if you want).
     * We currently know, where the choppers should be... so the movement on Y axis could be like: 1.5 * Math.sin(toRad(time * 50 % 360)) - 7
     * Also, of course, rotate the blades of both choppers. :)
     */


    var m = time / 6;
    light.position.copy(lightTrajectory.getPoint(m - parseInt(m)));

    renderer.render(scene, camera);
}

/**
 * Adding the hangar
 */
function addHangar() {
    var hangar = new THREE.Object3D();
    var halfPi = Math.PI / 2;

    var leftWall = createWall(0x555555, 100, 20);
    leftWall.position.set(-10, 0, 40);
    leftWall.rotation.set(0, halfPi, 0);
    hangar.add(leftWall);

    var rightWall = createWall(0x333333, 100, 20);
    rightWall.position.set(10, 0, 40);
    rightWall.rotation.set(0, -halfPi, 0);
    hangar.add(rightWall);

    var backWall = createWall(0x444444, 20, 20);
    backWall.position.set(0, 0, -10);
    hangar.add(backWall);

    var ceiling = createWall(0x111111, 20, 100);
    ceiling.position.set(0, 10, 40);
    ceiling.rotation.set(halfPi, 0, 0);
    hangar.add(ceiling);

    var floor = createWall(0x222222, 20, 100);
    floor.position.set(0, -10, 40);
    floor.rotation.set(-halfPi, 0, 0);

    // We load this texture for the floor. Overriding the material assigned to us in the createWall
    floor.material = new THREE.MeshLambertMaterial({
        map: textureLoader.load('https://cglearn.eu/files/course/7/textures/wallTexture2.jpg', onTextureLoaded)
    });
    hangar.add(floor);

    scene.add(hangar);
}

function createWall(colorCode, width, height) {
    var geometry = new THREE.PlaneGeometry(width, height, width, height);
    var material = new THREE.MeshBasicMaterial({ color: colorCode });
    var wall = new THREE.Mesh(geometry, material);

    return wall;
}