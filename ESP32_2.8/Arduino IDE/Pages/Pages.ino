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
uint32_t lastPageChange = 0;  // Tracks the last page change time
uint32_t countdown_time = 10; // Countdown time in seconds

// Onboard LEDs
#define CYD_LED_RED 4
#define CYD_LED_BLUE 17
#define CYD_LED_GREEN 16

// Global variables for LVGL objects
lv_obj_t *screen;
lv_obj_t *status_bar;
lv_obj_t *status_label;
lv_obj_t *countdown_label;
lv_obj_t *content_box;
lv_obj_t *content_box_2;
lv_obj_t *content_box_3; // Content box for Page 3
lv_obj_t *text_box_1;
lv_obj_t *text_box_2;
lv_obj_t *text_box_3; // Text for Page 3
lv_obj_t *toggle_button;

int current_page = 1; // Tracks the current page (1, 2, or 3)

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

    lv_obj_clear_flag(status_bar, LV_OBJ_FLAG_SCROLLABLE);

    status_label = lv_label_create(status_bar);
    lv_label_set_text(status_label, "Page 1 Active");
    lv_obj_align(status_label, LV_ALIGN_LEFT_MID, 10, 0);

    countdown_label = lv_label_create(status_bar);
    lv_label_set_text(countdown_label, "Switch in: 10s");
    lv_obj_align(countdown_label, LV_ALIGN_RIGHT_MID, -10, 0);
}

// Create the first content box
void create_content_box(lv_obj_t *parent) {
    content_box = lv_obj_create(parent);
    lv_obj_set_size(content_box, TFT_HOR_RES, TFT_VER_RES - 35); // Remaining height
    lv_obj_align(content_box, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(content_box, lv_color_make(255, 255, 255), 0);

    lv_obj_clear_flag(content_box, LV_OBJ_FLAG_SCROLLABLE);

    text_box_1 = lv_label_create(content_box);
    lv_label_set_text(text_box_1, "Page 1");
    lv_obj_align(text_box_1, LV_ALIGN_CENTER, 0, 0); 
}

// Create the second content box
void create_content_box_2(lv_obj_t *parent) {
    content_box_2 = lv_obj_create(parent);
    lv_obj_set_size(content_box_2, TFT_HOR_RES, TFT_VER_RES - 35); // Remaining height
    lv_obj_align(content_box_2, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(content_box_2, lv_color_make(240, 240, 240), 0);

    lv_obj_clear_flag(content_box_2, LV_OBJ_FLAG_SCROLLABLE);

    text_box_2 = lv_label_create(content_box_2);
    lv_label_set_text(text_box_2, "Page 2");
    lv_obj_align(text_box_2, LV_ALIGN_CENTER, 0, 0); 

    lv_obj_add_flag(content_box_2, LV_OBJ_FLAG_HIDDEN); // Hide initially
}

// Create the third content box
void create_content_box_3(lv_obj_t *parent) {
    content_box_3 = lv_obj_create(parent);
    lv_obj_set_size(content_box_3, TFT_HOR_RES, TFT_VER_RES - 35); // Remaining height
    lv_obj_align(content_box_3, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(content_box_3, lv_color_make(200, 255, 200), 0); // Light green background

    lv_obj_clear_flag(content_box_3, LV_OBJ_FLAG_SCROLLABLE);

    text_box_3 = lv_label_create(content_box_3);
    lv_label_set_text(text_box_3, "Page 3");
    lv_obj_align(text_box_3, LV_ALIGN_CENTER, 0, 0);

    lv_obj_add_flag(content_box_3, LV_OBJ_FLAG_HIDDEN); // Hide initially
}

// Toggle between content boxes
void toggle_content() {
    // Hide all pages
    lv_obj_add_flag(content_box, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(content_box_2, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(content_box_3, LV_OBJ_FLAG_HIDDEN);

    // Show the appropriate page
    if (current_page == 1) {
        lv_obj_clear_flag(content_box, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text(status_label, "Page 1 Active");
    } else if (current_page == 2) {
        lv_obj_clear_flag(content_box_2, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text(status_label, "Page 2 Active");
    } else if (current_page == 3) {
        lv_obj_clear_flag(content_box_3, LV_OBJ_FLAG_HIDDEN);
        lv_label_set_text(status_label, "Page 3 Active");
    }

    // Move to the next page
    current_page = (current_page % 3) + 1; // Loop through 1 -> 2 -> 3 -> 1
    lastPageChange = millis(); // Reset the countdown timer
}

// Automatically switch pages every countdown time
void auto_page_switch_with_countdown() {
    uint32_t now = millis();
    uint32_t elapsed = (now - lastPageChange) / 1000; // Calculate elapsed time in seconds
    int remaining_time = countdown_time - elapsed;

    // Update the countdown label
    char countdown_text[20];
    snprintf(countdown_text, sizeof(countdown_text), "Switch in: %ds", remaining_time);
    lv_label_set_text(countdown_label, countdown_text);

    if (remaining_time <= 0) {
        toggle_content();  // Switch pages
    }
}

// Create a button to manually toggle pages
void create_toggle_button(lv_obj_t *parent) {
    toggle_button = lv_btn_create(parent);
    lv_obj_set_size(toggle_button, 100, 30);
    lv_obj_align(toggle_button, LV_ALIGN_BOTTOM_MID, 0, -20);
    lv_obj_set_style_bg_color(toggle_button, lv_color_make(255, 0, 0), 0); 

    lv_obj_t *btn_label = lv_label_create(toggle_button);
    lv_label_set_text(btn_label, "Toggle");
    lv_obj_center(btn_label);

    lv_obj_add_event_cb(toggle_button, [](lv_event_t * e) {
        toggle_content();
    }, LV_EVENT_CLICKED, NULL);
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
    
    create_status_bar(screen);
    create_content_box(screen);
    create_content_box_2(screen);
    create_content_box_3(screen);
    create_toggle_button(screen);

    Serial.println("Setup done");
}

void loop() {
    lv_tick_inc(millis() - lastTick);
    lastTick = millis();
    lv_timer_handler();

    auto_page_switch_with_countdown();  // Call the auto page switch function

    delay(10);  // Small delay for responsiveness
}
