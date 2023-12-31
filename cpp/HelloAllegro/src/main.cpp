// ---------------------------- Includes -------------------------- //

// Include for console logging.
#include <iostream>
using namespace std;

// Use mathematics routines
#include <math.h>

// Include Allegro headers.
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_color.h>

#include <cmath>

// ---------------------------- Global variables -------------------------- //
ALLEGRO_DISPLAY*    display;
ALLEGRO_EVENT_QUEUE* event_queue;
ALLEGRO_EVENT       event;

// ---------------------------- Forward declarations -------------------------- //
void init();
void deinit();
void event_loop();
void draw();
void drawTriangle(int x, int y);

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
    display = al_create_display(640, 480);
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
        // Draw the scene
        draw();
   }
}

// ---------------------------- Drawing routines -------------------------- //
// Where all the drawing is performed.

void draw() {
    al_clear_to_color(al_map_rgb(0,0,0)); // Clear the screen from last frame

    drawTriangle(320, 240);

    al_flip_display(); //Double-buffering flip.
}
/**
 * Task: Find correct vertex coordinates and draw the triangle.
 * With Allegro you can call a one line command to draw a triangle.
 * See: https://www.allegro.cc/manual/5/al_draw_filled_triangle
 */
void drawTriangle(int x, int y) {
    float edgeLength = 250;
    float height = sqrt(pow(edgeLength, 2) - pow((edgeLength / 2), 2));

    float x1 = x - edgeLength / 2;
    float y1 = y + height / 3;

    float x2 = x + edgeLength / 2;
    float y2 = y1;

    float x3 = x;
    float y3 = y - 2 * height / 3;

    al_draw_filled_triangle(x1, y1, x2, y2, x3, y3, al_map_rgb(255, 0, 0));

    cout << "Vertices: (" << x1 << ", " << y1 << "), (" << x2 << ", " << y2 << "), (" << x3 << ", " << y3 << ")\n";
    cout << "Centroid: " << (x1+x2+x3)/3 << "," << (y1+y2+y3)/3 << "\n";

}
