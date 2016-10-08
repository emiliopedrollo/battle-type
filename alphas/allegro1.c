#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>

void show_screen(){
    
    ALLEGRO_DISPLAY *display = NULL;

   if(!al_init()) {
      fprintf(stderr, "failed to initialize allegro!\n");
      return -1;
   }

   display = al_create_display(640, 480);
   if(!display) {
      fprintf(stderr, "failed to create display!\n");
      return -1;
   }

   al_clear_to_color(al_map_rgb(255,255,255));  
   al_flip_display();
   
   show_ship(display);
   
   al_rest(10.0);
   al_destroy_display(display);
   return 0;
    
}

void show_ship(ALLEGRO_DISPLAY *display){
    const char *filename;
    ALLEGRO_BITMAP *bitmap;
    ALLEGRO_TIMER *timer;
    ALLEGRO_EVENT_QUEUE *queue;
    bool redraw = true;
    double zoom = 1;
    float angle = 0;
    float cx,cy,dx,dy;
    double t0;
    double t1;
    
    filename = "resources/alpha/BSFShips1680x1050.jpg";
    
    al_install_mouse();
    al_install_keyboard();
    
    
    al_init_image_addon();
    
    
    al_set_window_title(display, filename);
    
    t0 = al_get_time();
    bitmap = al_load_bitmap(filename);
    t1 = al_get_time();
    
    if (!bitmap) {
        fprintf(stderr, "failed to create display!\n");
        return;
    }
    
    printf("Loading took %.4f seconds\n", t1 - t0);
    
    timer = al_create_timer(1.0 / 30); 
    queue = al_create_event_queue();
    al_register_event_source(queue, al_get_keyboard_event_source()); 
    al_register_event_source(queue, al_get_display_event_source(display));
    al_register_event_source(queue, al_get_timer_event_source(timer));
    al_start_timer(timer);
    
    while (1) {
        ALLEGRO_EVENT event;
        al_wait_for_event(queue, &event); // Wait for and get an event.
        if (event.type == ALLEGRO_EVENT_DISPLAY_ORIENTATION) {
            int o = event.display.orientation;
            if (o == ALLEGRO_DISPLAY_ORIENTATION_0_DEGREES) {
                printf("0 degrees\n");
            }
            else if (o == ALLEGRO_DISPLAY_ORIENTATION_90_DEGREES) {
                printf("90 degrees\n");
            }
            else if (o == ALLEGRO_DISPLAY_ORIENTATION_180_DEGREES) {
                printf("180 degrees\n");
            }
            else if (o == ALLEGRO_DISPLAY_ORIENTATION_270_DEGREES) {
                printf("270 degrees\n");
            }
            else if (o == ALLEGRO_DISPLAY_ORIENTATION_FACE_UP) {
                printf("Face up\n");
            }
            else if (o == ALLEGRO_DISPLAY_ORIENTATION_FACE_DOWN) {
                printf("Face down\n");
            }
        }
        if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
            break;
        /* Use keyboard to zoom image in and out.
         * 1: Reset zoom.
         * +: Zoom in 10%
         * -: Zoom out 10%
         * f: Zoom to width of window
         */
        if (event.type == ALLEGRO_EVENT_KEY_CHAR) {
            if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE)
                break; // Break the loop and quite on escape key.
            if (event.keyboard.unichar == '1')
                zoom = 1;
            if (event.keyboard.unichar == '+')
                zoom *= 1.1;
            if (event.keyboard.unichar == '-')
                zoom /= 1.1;
            if (event.keyboard.unichar == 'f')
                zoom = (double)al_get_display_width(display) /
                    al_get_bitmap_width(bitmap);
            if (event.keyboard.keycode == 82 && event.keyboard.modifiers == 512)
                dx -= 1;
            if (event.keyboard.keycode == 83 && event.keyboard.modifiers == 512)
                dx += 1;
            if (event.keyboard.keycode == 84 && event.keyboard.modifiers == 512)
                dy -= 1;
            if (event.keyboard.keycode == 85 && event.keyboard.modifiers == 512)
                dy += 1;
            
            if (event.keyboard.keycode == 82 && event.keyboard.modifiers == 513)
                angle -= 0.1;
            if (event.keyboard.keycode == 83 && event.keyboard.modifiers == 513)
                angle += 0.1;
            
            printf("Keycode: %d | Modifier %d\n",event.keyboard.keycode,event.keyboard.modifiers);
        }

        // Trigger a redraw on the timer event
        if (event.type == ALLEGRO_EVENT_TIMER)
            redraw = true;
            
        // Redraw, but only if the event queue is empty
        if (redraw && al_is_event_queue_empty(queue)) {
            redraw = false;
            // Clear so we don't get trippy artifacts left after zoom.
            al_clear_to_color(al_map_rgb_f(0, 0, 0));
            if (zoom == 1 && angle == 0)
                al_draw_bitmap(bitmap, dx, dy, 0);
            else
                al_draw_scaled_rotated_bitmap(
                    bitmap, 0, 0, dx, dy, zoom, zoom, angle, 0);
            al_flip_display();
        }
    }

    al_destroy_bitmap(bitmap);
    
}