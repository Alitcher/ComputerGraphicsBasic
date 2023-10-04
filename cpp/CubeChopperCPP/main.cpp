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
#include <memory>

// --------------- Forward declarations ------------- //
GLuint createWall(glm::vec3 color);
GLuint createCube(glm::vec3 color);

void changeSecondaryCameraMode(int mode);

// --- Load the shaders declared in glsl files in the project folder ---//
shader_prog shader("chopper.vert.glsl", "chopper.frag.glsl");

GLuint cubeVAO;
GLuint floorVAO;
GLuint chopperVAO, firstBladeVAO, secondBladeVAO;

GLuint shaderProgram;
GLint translationLocation;
GLint colorLocation;

float fov = 80.0f; //In degrees
GLint screenWidth = 800;
GLint screenHeight = 450;
float aspectRatio;

void initWalls() {
    floorVAO = createWall(glm::vec3(0.22, 0.22, 0.22));
}

void initChopper() {
chopperVAO = createCube(glm::vec3(1.0f, 0.35f, 0.2f)); 
firstBladeVAO = secondBladeVAO = createCube(glm::vec3(0.65f, 0.78f, 0.97f));
}

GLuint createWall(glm::vec3 color) {
    float s = 10.0;

    GLfloat vertices[] = {
                            -s, -s, 0.0,
                             s, -s, 0.0,
                             s,  s, 0.0,
                            -s,  s, 0.0
                        };

    GLubyte indices[] = {
                            0, 1, 2,
                            0, 2, 3
                        };

    GLfloat colors[] = {
                            color[0], color[1], color[2],
                            color[0], color[1], color[2],
                            color[0], color[1], color[2],
                            color[0], color[1], color[2]
                        };

    GLuint vertexArrayHandle;
    glGenVertexArrays(1, &vertexArrayHandle);
    glBindVertexArray(vertexArrayHandle);

    shader.attribute3fv("position", vertices, 12);
    shader.attribute3fv("color", colors, 12);

    GLuint vboHandle;
    glGenBuffers(1, &vboHandle);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboHandle);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLfloat)*12, indices, GL_STATIC_DRAW);
    return vertexArrayHandle;
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

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);

    if (action == GLFW_REPEAT || action == GLFW_PRESS) {
        if (key == GLFW_KEY_LEFT) {
            fov -= 1.0f;
            if (fov < 1.0f) {
                fov = -80.0f;
            }
        }
        if (key == GLFW_KEY_RIGHT) {
            fov += 1.0f;
            if (fov > 179.0f) {
                fov = 200.0f;
            }
        }
        glm::mat4 newProjection = glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 100.f);
        mainCamera->setProjection(newProjection);
    }
}

void drawHangar() {
    std::stack<glm::mat4> ms; // this is matrix stack.
    ms.push(glm::mat4(1.0)); //Push an identity matrix to the bottom of stack

        ms.push(ms.top()); //Floor
            ms.top() = glm::rotate(ms.top(), glm::radians(-90.0f), glm::vec3(1.0, 0.0, 0.0));
            ms.top() = glm::translate(ms.top(), glm::vec3(0.0, 0.0, -10.0));

            shader.uniformMatrix4fv("modelMatrix", ms.top());
            glBindVertexArray(floorVAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);
        ms.pop();

    ms.pop();
}

// ---------------------------- Main -------------------------- //
int main(int argc, char *argv[]) {
    aspectRatio = screenWidth / screenHeight;

    GLFWwindow *win;
    GLenum error = glGetError();


    if (!glfwInit()) {
        exit (EXIT_FAILURE);
    }

    win = glfwCreateWindow(screenWidth, screenHeight, "Cube Chopper!", NULL, NULL);

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
    mainCamera = std::make_shared<Camera>(
        glm::perspective(glm::radians(fov), aspectRatio, 0.1f, 100.f),
        glm::lookAt(
            glm::vec3(0.0, 4.0, 15.0),
            glm::vec3(0.0, 0.0, 0.0),
            glm::vec3(0.0, 1.0, 0.0)
        )
    );

    initChopper();
    initWalls();

    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    start = std::chrono::system_clock::now();

    while (!glfwWindowShouldClose(win)) {

        glViewport(0,0, screenWidth, screenHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.uniformMatrix4fv("projectionMatrix", mainCamera->getProjection());
        shader.uniformMatrix4fv("viewMatrix", mainCamera->getView());
        drawHangar();
        drawChopper();

        glfwSwapBuffers(win);
        glfwPollEvents();
        end = std::chrono::system_clock::now();
        deltaTime = end - start; // calculate deltaTime
        rotationAngle += deltaTime.count(); // accumulate rotation angle
        start = end;
        if (error != GL_NO_ERROR) {
            // Handle the OpenGL error, log it, or print an error message.
            fprintf(stderr, "OpenGL error: %d\n", error);
        }
    }

    glfwTerminate();
    exit(EXIT_SUCCESS);

    return 0;
}
