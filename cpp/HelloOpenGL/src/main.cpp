// ---------------------------- Includes -------------------------- //
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <GLFW/glfw3.h>
#include <cmath>
// ---------------------------- Forward declarations -------------------------- //
void draw(GLFWwindow *win);
void drawTriangle(float x, float y);

// ---------------------------- Input -------------------------- //
// Listen for input event
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

// ---------------------------- Main -------------------------- //
int main(int argc, char *argv[]) {
    GLFWwindow *win;

    if (!glfwInit()) { //Something went wrong with GLFW initialization
        exit (EXIT_FAILURE);
    }

    win = glfwCreateWindow(640, 480, "Hello GLFW!", NULL, NULL);

    if (!win) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    //Sets the win context current in OpenGL state machine.
    //Following OpenGL commands will be directed at this context.
    glfwMakeContextCurrent(win);

    //Register the callback method for input
    glfwSetKeyCallback(win, key_callback);

    const GLubyte* renderer = glGetString (GL_RENDERER); // get renderer string
    const GLubyte* version = glGetString (GL_VERSION); // version as a string
    printf ("Renderer: %s\n", renderer);
    printf ("OpenGL version supported %s\n", version);

    //Event loop
    while (!glfwWindowShouldClose(win)) {
        draw(win);
        usleep(1000);
        glfwPollEvents();
    }

    glfwTerminate();
    exit(EXIT_SUCCESS);
    return 0;
}

// ---------------------------- Drawing -------------------------- //
/**
 * Everything related to drawing the scene
 */
void draw(GLFWwindow *win) {
    int width, height;
    glfwGetFramebufferSize(win, &width, &height);
    float aspect_ratio = (float) width / (float) height;
    
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-aspect_ratio, aspect_ratio, -1.f, 1.f, 1.f, -1.f);
    glMatrixMode(GL_MODELVIEW);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT); //Clear last frame

    drawTriangle(0.0f, 0.0f);

    glfwSwapBuffers(win);
}
/**
 * --Task--
 * Find the correct coordinates and draw the equilateral triangle
 * In the older OpenGL there is a function glVertex*() to specify vertices.
 * See: https://www.opengl.org/sdk/docs/man2/xhtml/glVertex.xml
 */
void drawTriangle(float x, float y) {
    //Draw triangle here

    float edgeLength = 0.5f;  // Length of an edge of the triangle
    float height = sqrt(edgeLength * edgeLength - (edgeLength / 2.0f) * (edgeLength / 2.0f));

    // Calculate the coordinates of the vertices
    float x1 = x - edgeLength / 2.0f;
    float y1 = y - height / 3.0f;

    float x2 = x + edgeLength / 2.0f;
    float y2 = y1;

    float x3 = x;
    float y3 = y + 2.0f * height / 3.0f;

    glBegin(GL_TRIANGLES);

    //vertex 1
    glColor3f (1.0, 0.0, 0.0); //set color
    glVertex2f(x1, y1);

    //vertex 2
    glColor3f(0.0f, 1.0f, 0.0f);
    glVertex2f(x2, y2);

    //vertex 3
    glColor3f(0.0f, 0.0f, 1.0f);
    glVertex2f(x3, y3);

    glEnd ();
}
