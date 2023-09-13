import * as THREE from 'https://unpkg.com/three@v0.150.0/build/three.module.js';

var renderer, scene;

// Create a global function 'onLoad'
window.onLoad = () => {
    var canvasContainer = document.getElementById('myCanvasContainer'); // This time we have a <div> element
    var width = 800; // We specify the dimensions here
    var height = 500;

    // Let us create a WebGL renderer with the specified dimensions
    renderer = new THREE.WebGLRenderer();
    renderer.setSize(width, height);
    canvasContainer.appendChild(renderer.domElement); // This will add the <canvas>

    // Every object is in a scene
    scene = new THREE.Scene();

    // Orthographic camera will handle the aspect ratio calculation for us
    var camera = new THREE.OrthographicCamera(width / - 2, width / 2, height / 2, height / - 2, 1, 1000);
    camera.position.z = 100; // Push the camera along the z-direction
    scene.add(camera);

    drawTriangle([0, 0]); // This should draw you a triangle with the midpoint at [0, 0].

    renderer.render(scene, camera); // We render our scene with our camera
}

function drawTriangle(position) {
    var geometry = new THREE.BufferGeometry();		// Vertices (and other info) are held in a BufferGeometry object
    var material = new THREE.MeshBasicMaterial({	// There are different types of material, let us use a basic one
        vertexColors: true,
        side: THREE.DoubleSide 
    });

    /* --Task--
        Task is to create 3 THREE.Vector3() objects that specify vertex coordinates of our triangle.
        Then you need to add them to the geometry via geometry.setFromPoints(points).
        Because we want our geometry to be filled, we need to add a face of our triangle.
        You can do this by creating an array of vertex indices. To add the indices to the
        geometry, use geometry.setIndex(indices). If you do not see a triangle, make sure the
        face is facing the camera.
    */


    var edgeLength = 200;
    var height = Math.sqrt(Math.pow(edgeLength, 2) - Math.pow(edgeLength / 2, 2));
    var xMid = position[0];
    var yMid = position[1];
    
    var x1 = xMid - edgeLength / 2;
    var y1 = yMid - height / 3;

    var x2 = xMid + edgeLength / 2;
    var y2 = y1;

    var x3 = xMid;
    var y3 = yMid + 2 * height / 3;

    var v1 = new THREE.Vector3(x1, y1, 0);
    var v2 = new THREE.Vector3(x2, y2, 0);
    var v3 = new THREE.Vector3(x3, y3, 0);

    geometry.setFromPoints([v1,v2,v3]);

    var indices = [0, 2, 1];
    geometry.setIndex(indices);
    console.log(geometry);

    console.log(`Vertices: (${x1}, ${y1}), (${x2}, ${y2}), (${x3}, ${y3})`);
    console.log(`centrid: ${(x1+x2+x3)/3},${(y1+y2+y3)/3}`);

    var color1 = new THREE.Color('#DAF7A6');
    var color2 = new THREE.Color('#C70039');
    var color3 = new THREE.Color('#581845');

    geometry.setAttribute('color', new THREE.BufferAttribute(new Float32Array([...color1.toArray(), ...color2.toArray(), ...color3.toArray()]), 3));


    var triangle = new THREE.Mesh(geometry, material); // The triangle will be a mesh object
    scene.add(triangle);							   // We add it to our scene
}