/*
	This will load and compile a shader from HTML document.
	http://learningwebgl.com/blog/?p=28
*/
function getShader(gl, id) {
      var shaderScript = document.getElementById(id);
      if (!shaderScript) {
          return null;
      }

      var str = "";
      var k = shaderScript.firstChild;
      while (k) {
          if (k.nodeType == 3)
              str += k.textContent;
          k = k.nextSibling;
      }

      var shader;
      if (shaderScript.type == "x-shader/x-fragment") {
          shader = gl.createShader(gl.FRAGMENT_SHADER);
      } else if (shaderScript.type == "x-shader/x-vertex") {
          shader = gl.createShader(gl.VERTEX_SHADER);
      } else {
          return null;
      }

      gl.shaderSource(shader, str);
      gl.compileShader(shader);

      if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
          alert(gl.getShaderInfoLog(shader));
          return null;
      }

      return shader;
}

 
function initShaders(gl) {
	var shaderProgram;
    var fragmentShader = getShader(gl, "shader-fs");
    var vertexShader = getShader(gl, "shader-vs");

    shaderProgram = gl.createProgram();
    gl.attachShader(shaderProgram, vertexShader);
    gl.attachShader(shaderProgram, fragmentShader);
    gl.linkProgram(shaderProgram);

    if (!gl.getProgramParameter(shaderProgram, gl.LINK_STATUS)) {
		alert("Could not initialise shaders");
    }

    gl.useProgram(shaderProgram);
	shaderProgram.aVertexPosition = gl.getAttribLocation(shaderProgram, "aVertexPosition");
    gl.enableVertexAttribArray(shaderProgram.aVertexPosition);

    // Compute aspect ratio
    var aspectRatio = 800 / 500;

    // Create an orthographic projection matrix
    var projectionMatrix = mat4.create();
    mat4.ortho(projectionMatrix, -1.0 * aspectRatio, 1.0 * aspectRatio, -1.0, 1.0, -1.0, 1.0);

    // Get the uniform location for the projection matrix in the shader program
    var projectionMatrixUniform = gl.getUniformLocation(shaderProgram, "uProjectionMatrix");

    // Set the uniform
    gl.uniformMatrix4fv(projectionMatrixUniform, false, projectionMatrix);


	return shaderProgram;
}
