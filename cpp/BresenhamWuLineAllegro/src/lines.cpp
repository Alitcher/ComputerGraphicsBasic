// ---------------------------- Includes -------------------------- //

// Include for console logging.
#include <iostream>
using namespace std;

// Use mathematics routines
#include <math.h>
#include <algorithm>
// Include Allegro headers.
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>

#include <chrono>

// ---------------------------- Global variables -------------------------- //
ALLEGRO_DISPLAY*    display;
ALLEGRO_EVENT_QUEUE* event_queue;
ALLEGRO_EVENT       event;

// ---------------------------- Forward declarations -------------------------- //
void init();
void deinit();
void event_loop();
void draw();

// You will need to implement those two functions
void bresenham_line(int x1, int y1, int x2, int y2, ALLEGRO_COLOR color);
void wu_line(int x0, int y0, int x1, int y1, ALLEGRO_COLOR color);

void measurePerformance(void (*lineAlgorithm)(int, int, int, int, ALLEGRO_COLOR), const char* algorithmName);

// ---------------------------- Main -------------------------- //
// The overall structure of our program is the familiar GUI event loop:

int main(){
    init();         // Initialize Allegro.
    event_loop();   // Run the event processing loop until a program is requested to quit
    deinit();       // Deinitialize

    return 0;       // This is simply required by the C++ standard to indicate successful completion
}


// ---------------------------- Allegro initialization -------------------------- //
// Initialize the Allegro system.

void init() {
    al_init();
    display = al_create_display(900, 300);
    event_queue = al_create_event_queue();
    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_install_keyboard();
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_init_primitives_addon();

}

// ---------------------------- Allegro deinitialization -------------------------- //
// Just the opposite of "init". Destroy whatever we created.

void deinit() {
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);
}

// ---------------------------- Event loop -------------------------- //

void event_loop() {
    while(1) {
        // We only need events to close the app when necessary
        bool have_event = al_wait_for_event_timed(event_queue, &event, 0.05);
        if (have_event) {
            if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE || event.type == ALLEGRO_EVENT_KEY_UP) break;
        }
        // No animation this time, simply drawing
        draw();
   }
}

// ---------------------------- Drawing routines -------------------------- //
// Where all the drawing is performed.

void al_draw_line_wrapper(int x1, int y1, int x2, int y2, ALLEGRO_COLOR color) {
    al_draw_line((float)x1, (float)y1, (float)x2, (float)y2, color, 1.0f);
}

void draw() {
    al_clear_to_color(al_map_rgb(255,255,255));

    // Here we test the implementations of the line algorithms.
    int CX = 150;
    int CY = 150;
    int R = 100;
    int NUM_LINES = 20;
    float STEP = 2*3.14/NUM_LINES;

    // Reference
    ALLEGRO_COLOR c = al_color_name("black");
    for (int i = 0; i < NUM_LINES; i++) {
        al_draw_line(CX, CY, CX + (int)(R*cos(STEP*i)), CY + (int)(R*sin(STEP*i)), c, 1);
    }

    // Bresenham
    c = al_color_name("red");
    CX += 300;
    for (int i = 0; i < NUM_LINES; i++) {
        bresenham_line(CX, CY, CX + (int)(R*cos(STEP*i)), CY + (int)(R*sin(STEP*i)), c);
    }


    // Wu
    c = al_color_name("purple");
    CX += 300;
    for (int i = 0; i < NUM_LINES; i++) {
        wu_line(CX, CY, CX + (int)(R * cos(STEP * i)), CY + (int)(R * sin(STEP * i)), c);
    }

//    // Measure performance
//    measurePerformance(bresenham_line, "Bresenham's Algorithm");
//    measurePerformance(wu_line, "Wu's Algorithm");
//    measurePerformance(al_draw_line_wrapper, "Canvas Drawing (Reference)");

    // We end by flipping the buffer:
    al_flip_display();
}

// ----------------------- Exercises ------------------------ //

void bresenham_line(int x1, int y1, int x2, int y2, ALLEGRO_COLOR color) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);

    int x = x1;
    int y = y1;

    int xStep = (x1 < x2) ? 1 : -1;  // Decide step direction for x
    int yStep = (y1 < y2) ? 1 : -1;  // Decide step direction for y

    float error = 0.0;

    if(dx >= dy) {  // If line is more horizontal
        float deltaError = static_cast<float>(dy) / dx;

        for (; x != x2; x += xStep) {
            al_draw_pixel(x, y, color);
            error += deltaError;
            if (error >= 0.5) {
                y += yStep;
                error -= 1.0;
            }
        }
    } else {  // If line is more vertical
        float deltaError = static_cast<float>(dx) / dy;

        for (; y != y2; y += yStep) {
            al_draw_pixel(x, y, color);
            error += deltaError;
            if (error >= 0.5) {
                x += xStep;
                error -= 1.0;
            }
        }
    }

    al_draw_pixel(x2, y2, color);  // Ensure endpoint is plotted
}

// get fractional part of a number
float fpart(float x) {
    return x - floor(x);
}

// get the reverse of the fractional part
float rfpart(float x) {
    return 1.0 - fpart(x);
}

// draw a pixel with given alpha
void plot(int x, int y, float alpha, ALLEGRO_COLOR color) {
    float r, g, b, a;
    al_unmap_rgb_f(color, &r, &g, &b); // Unmap the color to retrieve its components.

    // Create a new color based on the original color and brightness
    ALLEGRO_COLOR blended_color = al_map_rgba_f(r * alpha, g * alpha, b * alpha, alpha);
    al_draw_pixel(x, y, blended_color);
}

void wu_line(int x0, int y0, int x1, int y1, ALLEGRO_COLOR color) {
    bool steep = abs(y1 - y0) > abs(x1 - x0); // If line is more vertical, , swap x and y
    if (steep) {
        std::swap(x0, y0);
        std::swap(x1, y1);
    }
    if (x0 > x1) { // If line is more horizontal, swap the endpoints
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    float dx = x1 - x0;
    float dy = y1 - y0;
    float gradient = (dx == 0) ? 1.0 : dy / dx;

    // Handle first endpoint with anti-aliasing
    int xend = round(x0);
    float yend = y0 + gradient * (xend - x0);
    float xgap = rfpart(x0 + 0.5);
    int xpxl1 = xend;
    int ypxl1 = (int)yend;
    if (steep) {
        plot(ypxl1, xpxl1, rfpart(yend) * xgap, color);
        plot(ypxl1 + 1, xpxl1, fpart(yend) * xgap, color);
    } else {
        plot(xpxl1, ypxl1, rfpart(yend) * xgap, color);
        plot(xpxl1, ypxl1 + 1, fpart(yend) * xgap, color);
    }
    // Iterate y by gradient for next step
    float intery = yend + gradient;

    // Handle second endpoint with anti-aliasing
    xend = round(x1);
    yend = y1 + gradient * (xend - x1);
    xgap = fpart(x1 + 0.5);
    int xpxl2 = xend;
    int ypxl2 = (int)yend;
    if (steep) {
        plot(ypxl2, xpxl2, rfpart(yend) * xgap, color);
        plot(ypxl2 + 1, xpxl2, fpart(yend) * xgap, color);
    } else {
        plot(xpxl2, ypxl2, rfpart(yend) * xgap, color);
        plot(xpxl2, ypxl2 + 1, fpart(yend) * xgap, color);
    }

    // Draw the main portion of the line with anti-aliasing
    for (int x = xpxl1 + 1; x <= xpxl2 - 1; x++) {
        if (steep) {
            plot((int)intery, x, rfpart(intery), color);
            plot((int)intery + 1, x, fpart(intery), color);
        } else {
            plot(x, (int)intery, rfpart(intery), color);
            plot(x, (int)intery + 1, fpart(intery), color);
        }
        intery += gradient; // Update y value for next iteration
    }
}

void measurePerformance(void (*lineAlgorithm)(int, int, int, int, ALLEGRO_COLOR), const char* algorithmName) {
    // Define parameters for drawing a set of radial lines from a center point
    int CX = 450;  // Center to make it not influence the drawing on screen
    int CY = 150;
    int R = 100;
    int NUM_LINES = 1000;  // Increase this for more precision
    float STEP = 2*3.14/NUM_LINES;

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < NUM_LINES; i++) {
        lineAlgorithm(CX, CY, CX + (int)(R*cos(STEP*i)), CY + (int)(R*sin(STEP*i)), al_map_rgb(255, 255, 255));  // Using white to make sure it doesn't visibly influence the drawing
    }

    auto end = std::chrono::high_resolution_clock::now();
    // Calculate the elapsed time in milliseconds
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    cout << algorithmName << " took: " << duration << " ms for " << NUM_LINES << " lines." << endl;
}
