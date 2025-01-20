#include <lvgl.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <string.h>

#define TFT_HOR_RES   320
#define TFT_VER_RES   240
#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10 * (LV_COLOR_DEPTH / 8))

// Touchscreen and LVGL setup
#define XPT2046_IRQ 36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK 25
#define XPT2046_CS 33

SPIClass touchscreenSpi = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);

uint16_t touchScreenMinimumX = 200, touchScreenMaximumX = 3700, touchScreenMinimumY = 240, touchScreenMaximumY = 3800;
uint32_t lastTick = 0;

// Onboard LEDs
#define CYD_LED_RED 4
#define CYD_LED_BLUE 17
#define CYD_LED_GREEN 16

#define RED_FILTER 1
#define GREEN_FILTER 0.3
#define BLUE_FILTER 0.6

// Global variables for LVGL objects
lv_obj_t *screen;
lv_obj_t *status_bar;
lv_obj_t *status_label;
lv_obj_t *content_box;
lv_obj_t *start_label;
lv_obj_t *end_label;
lv_obj_t *countdown_bar;
lv_obj_t *name_label;
lv_obj_t *surname_label;
lv_obj_t *time_label;
lv_obj_t *section_label;
lv_obj_t *room_label;
lv_style_t bar_style;      // Style for the countdown bar

int difference_time = 0; // 0 seconds
// Mockup times in seconds (24-hour format)
int start_time_hours = 13;
int start_time_minutes = 0;
int end_time_hours = 13;
int end_time_minutes = 2;

// Touchscreen reading function
void my_touchpad_read(lv_indev_t *indev, lv_indev_data_t *data) {
    if (touchscreen.touched()) {
        TS_Point p = touchscreen.getPoint();
        data->point.x = map(p.x, touchScreenMinimumX, touchScreenMaximumX, 0, TFT_HOR_RES);
        data->point.y = map(p.y, touchScreenMinimumY, touchScreenMaximumY, 0, TFT_VER_RES);
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

// Create the status bar
void create_status_bar(lv_obj_t *parent) {
    status_bar = lv_obj_create(parent);
    lv_obj_set_size(status_bar, TFT_HOR_RES, 30);
    lv_obj_align(status_bar, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(status_bar, lv_color_make(255, 255, 255), 0);
    lv_obj_set_style_border_width(status_bar, 2, 0);
    lv_obj_set_style_border_color(status_bar, lv_color_make(180, 180, 180), 0);
    lv_obj_set_style_border_side(status_bar, LV_BORDER_SIDE_FULL, 0);

    lv_obj_clear_flag(status_bar, LV_OBJ_FLAG_SCROLLABLE);

    status_label = lv_label_create(status_bar);
    lv_label_set_text(status_label, "Status Bar");
    lv_obj_align(status_label, LV_ALIGN_CENTER, 0, 0);
}

// Create the content box
void create_content_box(lv_obj_t *parent) {
    content_box = lv_obj_create(parent);
    lv_obj_set_size(content_box, TFT_HOR_RES, TFT_VER_RES - 35); // Remaining height
    lv_obj_align(content_box, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(content_box, lv_color_make(255, 255, 255), 0); // White background
    lv_obj_set_style_border_width(content_box, 2, 0);
    lv_obj_set_style_border_color(content_box, lv_color_make(100, 100, 100), 0); // Darker gray border
    lv_obj_set_style_border_side(content_box, LV_BORDER_SIDE_FULL, 0);
    
    lv_obj_clear_flag(status_bar, LV_OBJ_FLAG_SCROLLABLE);
    
    // Add Name and Surname
    name_label = lv_label_create(content_box);
    lv_label_set_text(name_label, "Name:  Name");
    lv_obj_align(name_label, LV_ALIGN_TOP_LEFT, 10, 5);
    lv_obj_set_style_text_font(name_label, &lv_font_montserrat_18, 0); 

    surname_label = lv_label_create(content_box);
    lv_label_set_text(surname_label, "Surname:  Surname");
    lv_obj_align(surname_label, LV_ALIGN_TOP_LEFT, 10,30);
    lv_obj_set_style_text_font(surname_label, &lv_font_montserrat_16, 0); 

    // Add Section 
    section_label = lv_label_create(content_box);
    lv_label_set_text(section_label, "Section:  CE6541 ");
    lv_obj_align(section_label, LV_ALIGN_TOP_LEFT, 10, 55);
    lv_obj_set_style_text_font(section_label, &lv_font_montserrat_16, 0);

    //Add Room
    room_label = lv_label_create(content_box);
    lv_label_set_text(room_label, "Room: Co-Working Space 001");
    lv_obj_align(room_label, LV_ALIGN_TOP_LEFT, 10, 85);
    lv_obj_set_style_text_font(room_label, &lv_font_montserrat_18, 0);

    // Add Start: Time label
    start_label = lv_label_create(content_box);
    lv_label_set_text(start_label, "Start: 00:00");
    lv_obj_align(start_label, LV_ALIGN_TOP_LEFT, 10, 110);
    lv_obj_set_style_text_font(start_label, &lv_font_montserrat_18, 0); 

    // Add End: Time label
    end_label = lv_label_create(content_box);
    lv_label_set_text(end_label, "End: 00:00");
    lv_obj_align(end_label, LV_ALIGN_TOP_LEFT, 130, 110);
    lv_obj_set_style_text_font(end_label, &lv_font_montserrat_18, 0); 

    // Create a countdown bar
        // Initialize the style for the countdown bar
    lv_style_init(&bar_style);
    lv_style_set_bg_color(&bar_style, lv_color_make(0, 255, 0));  // Initial color: green
    lv_style_set_bg_grad_color(&bar_style, lv_color_make(0, 0, 255)); // Gradient to red BGR

    countdown_bar = lv_bar_create(screen);
    lv_obj_set_size(countdown_bar, 300, 20);    // Size of the bar
    lv_obj_align(countdown_bar, LV_ALIGN_BOTTOM_MID, 0, -30);  // Center align
    lv_bar_set_range(countdown_bar, 0, difference_time);    // Range from 0 to 100
    lv_obj_add_style(countdown_bar, &bar_style, LV_PART_INDICATOR);  // Apply the style to the bar indicator

    // Create a label to show remaining time
    time_label = lv_label_create(screen);
    lv_label_set_text(time_label, "Time: ");  // Initial text
    lv_obj_align(time_label, LV_ALIGN_BOTTOM_MID, 0, -10);  // Position below the bar

}

void calculate_difference_time() {
    difference_time = (end_time_hours - start_time_hours) * 60 * 60 + (end_time_minutes - start_time_minutes) * 60;
}

void update_countdown_bar() {
    static uint32_t current_time = 0; // Initialize the countdown timer
    static uint32_t last_update = millis(); // Record the last update time
    
    if (current_time < difference_time) {
        uint32_t now = millis();
        if (now - last_update >= 1000) { // Update once every second
            current_time++;
            int remaining_time = difference_time - current_time;

            // Update bar value
            lv_bar_set_value(countdown_bar, remaining_time, LV_ANIM_OFF);

            // Update label text
            // Update label text
            char time_text[16];
            int minutes = remaining_time / 60;  // Calculate minutes
            int seconds = remaining_time % 60; // Calculate remaining seconds
            snprintf(time_text, sizeof(time_text), "Time: %02d:%02d", minutes, seconds);
            lv_label_set_text(time_label, time_text);


             // Change bar color dynamically based on remaining time
            if (remaining_time > difference_time / 2) {
                lv_style_set_bg_color(&bar_style, lv_color_make(0, 255, 0));  // Green for >50% time
            } else if (remaining_time > difference_time / 4) {
                lv_style_set_bg_color(&bar_style, lv_color_make(0, 165, 255));  // Orange for 25%-50% time
            } else {
                lv_style_set_bg_color(&bar_style, lv_color_make(0, 0, 255));  // Red for <25% time
            }
            last_update = now;
        }
    } else {
        // Countdown finished
        lv_bar_set_value(countdown_bar, 0, LV_ANIM_OFF);
        lv_label_set_text(time_label, "Time's up!");
    }
}

void update_time_labels() {
    char start_text[16];
    snprintf(start_text, sizeof(start_text), "Start: %02d:%02d", start_time_hours, start_time_minutes);
    lv_label_set_text(start_label, start_text);

    char end_text[16];
    snprintf(end_text, sizeof(end_text), "End: %02d:%02d", end_time_hours, end_time_minutes);
    lv_label_set_text(end_label, end_text);
}

void setup() {
    Serial.begin(115200);

    lv_init();

    touchscreenSpi.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
    touchscreen.begin(touchscreenSpi);
    touchscreen.setRotation(3);

    uint8_t* draw_buf = new uint8_t[DRAW_BUF_SIZE];
    lv_display_t *disp = lv_tft_espi_create(TFT_HOR_RES, TFT_VER_RES, draw_buf, DRAW_BUF_SIZE);

    screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_black(), 0);

    lv_indev_t *indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, my_touchpad_read);

    pinMode(CYD_LED_RED, OUTPUT);
    pinMode(CYD_LED_GREEN, OUTPUT);
    pinMode(CYD_LED_BLUE, OUTPUT);
    
    calculate_difference_time();
    create_status_bar(screen);
    create_content_box(screen);

    Serial.println("Setup done");
}

void loop() {
    lv_tick_inc(millis() - lastTick);
    lastTick = millis();
    lv_timer_handler();

    update_countdown_bar();
    update_time_labels();

    delay(1000);
}