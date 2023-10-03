// ---------------------------- Includes -------------------------- //
#include <stdlib.h>         // C++ standard library
#include <stack>            // We use the standard C++ stack implementation to create model matrix stacks
#include <unistd.h>         // Threading
#include <stdio.h>          // Input/Output
#include <GLEW/glew.h>      // OpenGL Extension Wrangler -
#include <GLFW/glfw3.h>     // Windows and input
#include <glm/glm.hpp>      // OpenGL math library
#include "shader_util.h"    // Utility methods to keep this file a bit shorter.
#include <chrono>
#include <iostream>
#include "Camera.h"

// --------------- Forward declarations ------------- //
GLuint createWall(glm::vec3 color);
GLuint createCube(glm::vec3 color);

void changeSecondaryCameraMode(int mode);

// --- Load the shaders declared in glsl files in the project folder ---//
shader_prog shader("chopper.vert.glsl", "chopper.frag.glsl");

GLuint cubeVAO;
GLuint leftWallVAO, rightWallVAO, backWallVAO, ceilingVAO, floorVAO;
GLuint chopperVAO, firstBladeVAO, secondBladeVAO;

GLuint shaderProgram;
GLint translationLocation;
GLint colorLocation;

float fov = 80.0f; //In degrees
float screenWidth = 800;
float screenHeight = 450;

void initWalls() {
    leftWallVAO = createWall(glm::vec3(0.66, 0.66, 0.66));
    rightWallVAO = createWall(glm::vec3(0.4, 0.4, 0.4));
    backWallVAO = createWall(glm::vec3(0.52, 0.52, 0.52));
    ceilingVAO = createWall(glm::vec3(0.33, 0.33, 0.33));
    floorVAO = createWall(glm::vec3(0.22, 0.22, 0.22));
}

GLuint createWall(glm::vec3 color) {
    float s = 10.0;
    /**
    * We start by creating arrays of the data, which we will later send to OpenGL.
    */

    //In this array we define coordinates for the corners of the walls.
    GLfloat vertices[] = {
                            -s, -s, 0.0,
                             s, -s, 0.0,
                             s,  s, 0.0,
                            -s,  s, 0.0
                        };

    //In this array we define faces for the triangles within the walls.
    //Each set of three vertices defines one triangle.
    GLubyte indices[] = {
                            0, 1, 2,
                            0, 2, 3
                        };

    //Colors for the 4 vertices
    GLfloat colors[] = {
                            color[0], color[1], color[2],
                            color[0], color[1], color[2],
                            color[0], color[1], color[2],
                            color[0], color[1], color[2]
                        };

    GLuint vertexArrayHandle;
    glGenVertexArrays(1, &vertexArrayHandle);
    glBindVertexArray(vertexArrayHandle);

    //Here we set up VBO-s for coordinates and colors of the vertices.
    shader.attribute3fv("position", vertices, 12);
    shader.attribute3fv("color", colors, 12);

    // First step. We create a handle for our buffer
    GLuint vboHandle;
    glGenBuffers(1, &vboHandle);
    // Second step. We bind (activate) this buffer for the OpenGL state machine.
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboHandle);
    // Third step. We store the vertex indexes in the VBO.
    // These define the faces of the triangles, which make up the cube/
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLfloat)*12, indices, GL_STATIC_DRAW);
    //We return handle to vertex array. Step 4 takes place in method drawHangar.
    //Step 5 we skip, as all our assets have the same lifetime as rest of the program.
    return vertexArrayHandle;
}

/**
 * Use this method to initialize the 3 components of your chopper
*/
void initChopper() {
chopperVAO = createCube(glm::vec3(1.0f, 0.35f, 0.2f)); 
firstBladeVAO = secondBladeVAO = createCube(glm::vec3(0.65f, 0.78f, 0.97f));
}

GLuint createCube(glm::vec3 color) {
    GLfloat vertices[] = {
        -1.0, -1.0, 1.0,  // 0
        1.0, -1.0, 1.0,   // 1
        1.0, 1.0, 1.0,    // 2
        -1.0, 1.0, 1.0,   // 3
        -1.0, -1.0, -1.0, // 4

        1.0, -1.0, -1.0, // 5
        1.0, 1.0, -1.0,  // 6
        -1.0, 1.0, -1.0, // 7
    };

    GLubyte indices[] = {

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
    };

    glm::vec3 lightColor = glm::vec3(1.0f, 0.98f, 0.73f); // some yellow
    glm::vec3 darkColor = glm::vec3(0.24f, 0.15f, 0.325f); // some purple

    float blendFactor = 0.5f; 
    glm::vec3 blendedLightColor = lightColor * blendFactor + color * (1 - blendFactor);
    glm::vec3 blendedDarkColor = darkColor * blendFactor + color * (1 - blendFactor);

    GLfloat colors[] = {
        blendedLightColor.x, blendedLightColor.y, blendedLightColor.z, // 0
        blendedLightColor.x, blendedLightColor.y, blendedLightColor.z, // 1
        blendedLightColor.x, blendedLightColor.y, blendedLightColor.z, // 2
        blendedLightColor.x, blendedLightColor.y, blendedLightColor.z, // 3

        blendedDarkColor.x, blendedDarkColor.y, blendedDarkColor.z, // 4
        blendedDarkColor.x, blendedDarkColor.y, blendedDarkColor.z, // 5
        blendedDarkColor.x, blendedDarkColor.y, blendedDarkColor.z, // 6
        blendedDarkColor.x, blendedDarkColor.y, blendedDarkColor.z  // 7
    };

    GLuint vertexArrayHandle;
    glGenVertexArrays(1, &vertexArrayHandle);
    glBindVertexArray(vertexArrayHandle);

    shader.attribute3fv("position", vertices, 36);
    shader.attribute3fv("color", colors, 36);
//shader.attribute3fv("color", colors, 24); // 8 vertices * 3 values per vertex


    GLuint vboHandle;
    glGenBuffers(1, &vboHandle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboHandle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLfloat) * 36, indices, GL_STATIC_DRAW);

    return vertexArrayHandle;
}

std::chrono::time_point<std::chrono::system_clock> start;
std::chrono::time_point<std::chrono::system_clock> end;
std::chrono::duration<double> elapsed_seconds;
std::chrono::duration<double> deltaTime;
double rotationAngle = 0.0;

void drawChopper() {

    float rotationAngleX = static_cast<float>(rotationAngle * 2 * glm::pi<double>());
    float rotationAngleY = rotationAngleX; 
    std::stack<glm::mat4> ms; // this is matrix stack.
    ms.push(glm::mat4(1.0)); //Push an identity matrix to the bottom of stack
        ms.push(ms.top());
            // ms.top() = glm::rotate(ms.top(), glm::radians(static_cast<float>(rotationAngleY)), glm::vec3(0.0, 1.0, 0.0));
            // ms.top() = glm::rotate(ms.top(), glm::radians(static_cast<float>(rotationAngleX)), glm::vec3(1.0, 0.0, 0.0));
        
            ms.push(ms.top());

                ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 0.0, 0.0));
                ms.top() = glm::scale(ms.top(),glm::vec3(5.0, 2.0, 3.0));
                // Send the current model matrix at the top of stack to the vertex shader.
                shader.uniformMatrix4fv("modelMatrix", ms.top());
                glBindVertexArray(chopperVAO);
                glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, 0);
            ms.pop();
            
            ms.push(ms.top());
                ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 4.0, 0.0));
                ms.top() = glm::rotate(ms.top(), static_cast<float>(rotationAngleX), glm::vec3(0.0, 1.0, 0.0));
                ms.top() = glm::scale(ms.top(),glm::vec3(5.0, 0.5, 2.0));

                ms.push(ms.top());
                    ms.top() = glm::translate(ms.top(), glm::vec3(-1.25, 0.0, 0.0));
                    shader.uniformMatrix4fv("modelMatrix", ms.top());
                    glBindVertexArray(firstBladeVAO);
                    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, 0);
                ms.pop();
                
                ms.push(ms.top());
                    ms.top() = glm::translate(ms.top(), glm::vec3(1.25, 0.0, 0.0));
                    shader.uniformMatrix4fv("modelMatrix", ms.top());
                    glBindVertexArray(secondBladeVAO);
                    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, 0);
                ms.pop();
            ms.pop();
         ms.pop();
    ms.pop();   

}



// ---------------------------- Input -------------------------- //
// This method is called when keyboard event happens.
// Sets GLFW window should close flag to true, when escape key is pressed.
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (action == GLFW_REPEAT || action == GLFW_PRESS) {
        if (key == GLFW_KEY_LEFT) {
            std::cout << "Change the global fov, create a new perspective matrix, assign it to mainCamera->projection" << "\n";
            // fov -= 1.0f;
            // if (fov < 1.0f) {
            //     fov = 1.0f;
            // }
            glm::mat4 newProjection = glm::perspective(glm::radians(fov), screenWidth / screenHeight, 0.1f, 100.f);
            mainCamera->setProjection(newProjection);
           // mainCamera.setsetProjection(glm::perspective(glm::radians(fov), aspectRatio, near, far));
        }
        if (key == GLFW_KEY_RIGHT) {
            std::cout << "Change the global fov, create a new perspective matrix, assign it to mainCamera->projection" << "\n";
            // fov += 1.0f;
            // if (fov > 179.0f) {
            //     fov = 179.0f;
            // }
            glm::mat4 newProjection = glm::perspective(glm::radians(fov), screenWidth / screenHeight, 0.1f, 100.f);
            mainCamera->setProjection(newProjection);
        }
    }
}

void changeSecondaryCameraMode(int mode) {
    if (1 == mode) {
        secondaryCamera->getView() = glm::lookAt(
            glm::vec3(0.0, 0.0, 15.0), //Position
            glm::vec3(0.0, 0.0, 0.0),  //LookAt
            glm::vec3(0.0, 1.0, 0.0)   //Up
        );
    } else if (2 == mode) {
        // Look from the top
        secondaryCamera->setView(glm::lookAt(
            glm::vec3(0.0, 15.0, 0.0),
            glm::vec3(0.0, 0.0, 0.0),
            glm::vec3(0.0, 0.0, -1.0)
        ));
    } else {
        secondaryCamera->setView(glm::lookAt(
            glm::vec3(-15.0, 0.0, 0.0),
            glm::vec3(0.0, 0.0, 0.0),
            glm::vec3(0.0, 1.0, 0.0)
        ));
    }
}

void drawHangar() {
    std::stack<glm::mat4> ms; // this is matrix stack.
    ms.push(glm::mat4(1.0)); //Push an identity matrix to the bottom of stack

        ms.push(ms.top());
            ms.top() = glm::rotate(ms.top(), glm::radians(90.0f), glm::vec3(0.0, 1.0, 0.0));
            ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 0.0, -10.0));
            // Send the current model matrix at the top of stack to the vertex shader.
            shader.uniformMatrix4fv("modelMatrix", ms.top());
            // Bind a vertex array to the current OpenGL context
            glBindVertexArray(leftWallVAO);

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
            // Pop back to last model matrix. This doesn't have the scaling, rotation and translation,
            // which were added after the last push.
        ms.pop();
        ms.push(ms.top()); //Right wall
            ms.top() = glm::rotate(ms.top(), glm::radians(-90.0f), glm::vec3(0.0, 1.0, 0.0));
            ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 0.0, -10.0));
            shader.uniformMatrix4fv("modelMatrix", ms.top());
            glBindVertexArray(rightWallVAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
        ms.pop();
        ms.push(ms.top()); //Floor
            ms.top() = glm::rotate(ms.top(), glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
            ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 0.0, -10.0));

            shader.uniformMatrix4fv("modelMatrix", ms.top());
            glBindVertexArray(floorVAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
        ms.pop();
        ms.push(ms.top()); //Ceiling
            ms.top() = glm::rotate(ms.top(), glm::radians(90.0f), glm::vec3(1.0, 0.0, 0.0));
            ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 0.0, -10.0));
            shader.uniformMatrix4fv("modelMatrix", ms.top());
            glBindVertexArray(ceilingVAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
        ms.pop();
        ms.push(ms.top()); //Back wall
            ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 0.0, -10.0));
            shader.uniformMatrix4fv("modelMatrix", ms.top());
            glBindVertexArray(backWallVAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
        ms.pop();

    ms.pop();
}

// ---------------------------- Main -------------------------- //
int main(int argc, char *argv[]) {
    GLFWwindow *win;

    if (!glfwInit()) {
        exit (EXIT_FAILURE);
    }

    win = glfwCreateWindow(640, 480, "Cube Chopper!", NULL, NULL);

    if (!win) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(win);

    //Some extensions are marked as experimental. To use the latest
    //version of OpenGL supported, we have to set this flag to true.
    glewExperimental = GL_TRUE;
    GLenum status = glewInit();

    if(status != GLEW_OK) {
        fprintf(stderr, "Error: %s\n", glewGetErrorString(status));
    }

    const GLubyte* renderer = glGetString (GL_RENDERER); // get renderer string
    const GLubyte* version = glGetString (GL_VERSION); // version as a string
    printf ("Renderer: %s\n", renderer);
    printf ("OpenGL version supported %s\n", version);

    glfwSetKeyCallback(win, key_callback);

    shader.use();

    //Here we create the initial main camera
    mainCamera = new Camera(
        glm::perspective(glm::radians(fov), screenWidth / screenHeight, 0.1f, 100.f),
        glm::lookAt(
            glm::vec3(0.0, 4.0, 15.0), //Position
            glm::vec3(0.0, 0.0, 0.0),  //LookAt
            glm::vec3(0.0, 1.0, 0.0)   //Up
        )
    );

    //Here we create the initial secondary camera
    float halfSize = 10.0f; //You may want to change this so that your chopper fits
    secondaryCamera = new Camera(
        glm::ortho(-halfSize, halfSize, -halfSize, halfSize, 0.1f, 100.f),
        glm::lookAt(
            glm::vec3(0.0, 0.0, 15.0), //Position
            glm::vec3(0.0, 0.0, 0.0),  //LookAt
            glm::vec3(0.0, 1.0, 0.0)   //Up
        )
    );

    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0, 6.0, 15.0), //Position
        glm::vec3(0.0, 0.0, 0.0),  //LookAt
        glm::vec3(0.0, 1.0, 0.0)   //Up
    );
    //shader.uniformMatrix4fv("projectionMatrix", projection);
    shader.uniformMatrix4fv("viewMatrix", view);

    initChopper();
    initWalls();

    //Turn on depth test, so the objects closer to the camera would
    //be drawn in front of the objects further away.
    glEnable(GL_DEPTH_TEST);
    //Enable back-face culling
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    //Clear our background to black
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    start = std::chrono::system_clock::now();

    while (!glfwWindowShouldClose(win)) {

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.uniformMatrix4fv("projectionMatrix", mainCamera->getProjection());
        shader.uniformMatrix4fv("viewMatrix", mainCamera->getView());

        drawHangar();
        drawChopper();

        //This calls the changeSecondaryCameraMode with a different parameter every 3 seconds.
        float timeSwitch = fmod(glfwGetTime(), 9.0f);
        if (timeSwitch < 3) {
            changeSecondaryCameraMode(0);
        } else if (timeSwitch < 6) {
            changeSecondaryCameraMode(1);
        } else {
            changeSecondaryCameraMode(2);
        }

        glfwSwapBuffers(win);
        glfwPollEvents();
        end = std::chrono::system_clock::now();
        deltaTime = end - start; // calculate deltaTime
        rotationAngle += deltaTime.count(); // accumulate rotation angle
        start = end;
    }

    delete mainCamera;
    delete secondaryCamera;

    glfwTerminate();
    exit(EXIT_SUCCESS);

    return 0;
}
