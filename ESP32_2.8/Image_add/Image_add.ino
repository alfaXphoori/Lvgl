#include <lvgl.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <string.h>
#include <image.h>

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

// Global variables for LVGL objects
lv_obj_t *screen;
lv_obj_t *status_bar;
lv_obj_t *status_label;
lv_obj_t *content_box;
lv_obj_t *text_box;
lv_obj_t *img_cat;
lv_obj_t *img_catc;

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
    lv_label_set_text(status_label, "Status Bar ");
    lv_obj_align(status_label, LV_ALIGN_TOP_MID, 0, -10);
}

// Create the first content box
void create_content_box(lv_obj_t *parent) {
    content_box = lv_obj_create(parent);
    lv_obj_set_size(content_box, TFT_HOR_RES, TFT_VER_RES - 35); // Remaining height
    lv_obj_align(content_box, LV_ALIGN_BOTTOM_MID, 0, 0);
    lv_obj_set_style_bg_color(content_box, lv_color_make(255, 255, 255), 0);

    lv_obj_clear_flag(content_box, LV_OBJ_FLAG_SCROLLABLE);

    LV_IMAGE_DECLARE(my_image);
    img_cat = lv_image_create(lv_screen_active());
    lv_image_set_src(img_cat, &my_image);
    lv_obj_align(img_cat, LV_ALIGN_CENTER, 0, 0);
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

    Serial.println("Setup done");
}

void loop() {
    lv_tick_inc(millis() - lastTick);
    lastTick = millis();
    lv_timer_handler();

    delay(500);  // Small delay for responsiveness
}
