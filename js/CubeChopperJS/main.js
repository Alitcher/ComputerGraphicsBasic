import * as THREE from 'https://unpkg.com/three@v0.150.0/build/three.module.js';

var renderer, scene, camera;
var chopper; // We will store our main object (the chopper) in this variable

/**
 * Function to tell us the milliseconds
 */
function millis() {
    return (new Date()).getTime();
}

/**
 * Converts degrees to radians
 */
function toRad(degree) {
    return Math.PI * 2 * degree / 360;
}

window.onLoad = () => {
    var canvasContainer = document.getElementById('myCanvasContainer');
    var width = 800;
    var height = 500;

    renderer = new THREE.WebGLRenderer();
    renderer.setSize(width, height);
    canvasContainer.appendChild(renderer.domElement);

    scene = new THREE.Scene();

    // We will use a perspective camera, with FOV 80 degrees, correct aspect ratio, near plane at distance 1, far plane at distance 1000
    camera = new THREE.PerspectiveCamera(80, width / height, 1, 1000);
    camera.position.z = 15; // Push the camera along the z-direction
    camera.position.y = 2; // Push the camera a bit upwards, if you want
    camera.up = new THREE.Vector3(0, 1, 0);	// The up-direction of the camera
    camera.lookAt(new THREE.Vector3(0, 0, 0)); // Camera will be looking at the center of the scene
    scene.add(camera);

    addHangar();	// This will create and add the hangar walls to the scene
    chopper = addChopper(); // This will create and add the chopper itself

    scene.add(chopper);
    draw();
}



/**
 * Create and add the hangar to the scene.
 * Hangar currently consists of 5 walls which have different colors.
 */
function addHangar() {
    var hangar = new THREE.Object3D();
    var halfPi = Math.PI / 2;

    var leftWall = createWall(0x555555);
    leftWall.position.set(-10, 0, 0);
    leftWall.rotation.set(0, halfPi, 0);
    hangar.add(leftWall);

    var rightWall = createWall(0x333333);
    rightWall.position.set(10, 0, 0);
    rightWall.rotation.set(0, -halfPi, 0);
    hangar.add(rightWall);

    var backWall = createWall(0x444444);
    backWall.position.set(0, 0, -10);
    hangar.add(backWall);

    var ceiling = createWall(0x111111);
    ceiling.position.set(0, 10, 0);
    ceiling.rotation.set(halfPi, 0, 0);
    hangar.add(ceiling);

    var floor = createWall(0x222222);
    floor.position.set(0, -10, 0);
    floor.rotation.set(-halfPi, 0, 0);
    hangar.add(floor);

    scene.add(hangar);
}

/**
 * This function creates and adds the chopper.
 * --Task-- Finish this function
 */
function addChopper() {
    var chopper = new THREE.Object3D();
    // Position the entire chopper

    var body = createCube(0xeeeeee); // compose chopper from cube
    // Set the scale and position of the body here:
    // Scale the cube to be a long and thick cuboid
    // Position it a bit towards the floor (or above if you put the root origin is at the chopper's base)
    // Try to have the body and the blades be somehow centered in the chopper's local coordinate system
    body.scale.set(1, 1, 1);
    chopper.add(body);

    var bladeOffsetY = 1.5;
    var blades = new THREE.Object3D();
    // Group the blades in one Object3D called "blades"
    // Move the blades somewhat above the origin (or more above if the root origin is at the base) such that it would be above the body

    var blade1 = createCube(0xcceecc);
    // Scale and position the blade 1 in relation to the blades object
    blade1.position.set(0,bladeOffsetY,-2)
    blade1.scale.set(0.5, 0.1, bladeOffsetY)
    blades.add(blade1);

    var blade2 = createCube(0xcceecc);
    // Scale and position the blade 2 in relation to the blades object
    blade2.scale.set(0.5, 0.1, bladeOffsetY)
    blade2.position.set(0,bladeOffsetY,2)
    blades.add(blade2);

    // Add the blades to the chopper and chopper to the scene.
    chopper.add(blades);
    scene.add(chopper);
    return chopper;
}

function draw() {
    requestAnimationFrame(draw); // call every frame

    // --Task--
    // Blades object is the second child of the chopper object. Fetch it and set its rotation according to the time.
    // The rotation around the y-axis could be something like: toRad((millis() / 10) % 360)

     chopper.rotation.y = toRad((millis() / 150) % 360);
     chopper.rotation.x = toRad((millis() / 150) % 360);

    if(chopper && chopper.children.length > 1) {
        var blades = chopper.children[1]; // assuming blades is the second child of chopper
        blades.rotation.y = toRad((millis() / 10) % 360); // rotate the blades around the y-axis
    }

    renderer.render(scene, camera); // We render our scene with our camera
}

/**
 * Creates a cube.
 * --Task-- finish this function.
 */
function createCube(colorCode) {
    // You could create a cube using the THREE.BoxGeometry, but for now let us try to specify it ourselves...
    // ... it's just a cube, how hard can it be?

    // Cube has 8 vertices, each vertex has 3 coordinates
    var vertices = [
       -1.0, -1.0,  1.0, // 0
       1.0, -1.0,  1.0, // 1
       1.0,  1.0,  1.0, // 2
       -1.0,  1.0,  1.0, // 3
        -1.0, -1.0, -1.0, // 4

        1.0, -1.0, -1.0, // 5
        1.0,  1.0, -1.0, // 6
       -1.0,  1.0, -1.0, // 7
        // ... continue them
    ];
    var faces = [
        0, 1, 2, // Front face 1st triangle
        0, 2, 3, // Front face 2nd triangle
        
        7, 6, 5, // Back face 1st triangle
        7, 5, 4, // Back face 2nd triangle
        
        4, 5, 1, // Bottom face 1st triangle
        4, 1, 0, // Bottom face 2nd triangle
        
        3, 2, 6, // Top face 1st triangle
        3, 6, 7, // Top face 2nd triangle
        
        4, 0, 3, // Left face 1st triangle
        4, 3, 7, // Left face 2nd triangle
        
        1, 5, 6, // Right face 1st triangle
        1, 6, 2  // Right face 2nd triangle
    ];
    // Triangulized cube has 12 faces, each face has 3 indices
    // var faces = [
    //     0, 1, 2, // Front face
    //     0, 2, 3,

    //     0,3,4,
    //     4,3,7,

    //     1,2,6,
    //     1,5,6,

    //     4,5,6,
    //     4,7,6,

    //     1,0,4,
    //     1,4,5,

    //     3,2,6,
    //     3,7,6
    //     // ... continue them
    // ];



    // The indices of the faces depend on the order in which you specify the vertices.
    // Remember which side is the front face and which the back face of each triangle.

    // We create our own custom geometry
    var geometry = new THREE.BufferGeometry();
    var verticesCount = vertices.length;
    var i;

    var points = [];

    // ... for each vertex we need to create a THREE.Vector3 and push it into the vertex array.
    for(i = 0; i < verticesCount; i+=3){ //3 coordinates
        var vec = new THREE.Vector3(vertices[0 + i], vertices[1+i], vertices[2+i]);
        points.push(vec);
    }
    // ... the points and indices have to be added into the geometry.
geometry.setFromPoints(points);
geometry.setIndex(faces);
    // You may want to "console.log(geometry);" in order to debug.

    // Create the corresponding material and a mesh, return it
    var material = new THREE.MeshBasicMaterial({ color: colorCode });
    var cube = new THREE.Mesh(geometry, material);

    return cube;
}

/**
 * Creates a wall using the plane geometry.
 */
//already implemented. do not edit anything
function createWall(colorCode) {
    var geometry = new THREE.PlaneGeometry(20, 20, 1);
    var material = new THREE.MeshBasicMaterial({ color: colorCode });
    var wall = new THREE.Mesh(geometry, material);

    return wall;
}
