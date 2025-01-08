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
lv_obj_t *countdown_bar;  // Countdown bar
lv_obj_t *time_label;     // Label to show remaining time
lv_style_t bar_style;      // Style for the countdown bar

int differece_time = 30; // 100 seconds

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

void create_countdown_bar(){
    // Initialize the style for the countdown bar
    lv_style_init(&bar_style);
    lv_style_set_bg_color(&bar_style, lv_color_make(0, 255, 0));  // Initial color: green
    lv_style_set_bg_grad_color(&bar_style, lv_color_make(0, 0, 255)); // Gradient to red

    // Create a countdown bar
    countdown_bar = lv_bar_create(screen);
    lv_obj_set_size(countdown_bar, 300, 20);    // Size of the bar
    lv_obj_align(countdown_bar, LV_ALIGN_CENTER, 0, 0);  // Center align
    lv_bar_set_range(countdown_bar, 0, differece_time);    // Range from 0 to 100
    lv_obj_add_style(countdown_bar, &bar_style, LV_PART_INDICATOR);  // Apply the style to the bar indicator

    // Create a label to show remaining time
    time_label = lv_label_create(screen);
    lv_label_set_text(time_label, "Time: 100s");  // Initial text
    lv_obj_align(time_label, LV_ALIGN_CENTER, 0, 30);  // Position below the bar
}

void update_countdown_bar() {
    static uint32_t current_time = 0; // Initialize the countdown timer
    static uint32_t last_update = millis(); // Record the last update time

    if (current_time < differece_time) {
        uint32_t now = millis();
        if (now - last_update >= 1000) { // Update once every second
            current_time++;
            int remaining_time = differece_time - current_time;

            // Update bar value
            lv_bar_set_value(countdown_bar, remaining_time, LV_ANIM_OFF);

            // Update label text
            char time_text[16];
            snprintf(time_text, sizeof(time_text), "Time: %ds", remaining_time);
            lv_label_set_text(time_label, time_text);

             // Change bar color dynamically based on remaining time
            if (remaining_time > differece_time / 2) {
                lv_style_set_bg_color(&bar_style, lv_color_make(0, 255, 0));  // Green for >50% time
            } else if (remaining_time > differece_time / 4) {
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
            
void setup() {
    Serial.begin(115200);

    lv_init();

    touchscreenSpi.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
    touchscreen.begin(touchscreenSpi);
    touchscreen.setRotation(3);

    uint8_t* draw_buf = new uint8_t[DRAW_BUF_SIZE];
    lv_display_t *disp = lv_tft_espi_create(TFT_HOR_RES, TFT_VER_RES, draw_buf, DRAW_BUF_SIZE);

    screen = lv_scr_act();
    lv_obj_set_style_bg_color(screen, lv_color_white(), 0);

    lv_indev_t *indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, my_touchpad_read);

    pinMode(CYD_LED_RED, OUTPUT);
    pinMode(CYD_LED_GREEN, OUTPUT);
    pinMode(CYD_LED_BLUE, OUTPUT);

    create_countdown_bar();

    Serial.println("Setup done");
}

void loop() {
    lv_tick_inc(millis() - lastTick);
    lastTick = millis();
    lv_timer_handler();

    update_countdown_bar();

    delay(1000);  // Update every second
}