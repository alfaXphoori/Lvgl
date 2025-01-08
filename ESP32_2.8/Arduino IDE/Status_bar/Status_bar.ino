#include <lvgl.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>

#define TFT_HOR_RES   320
#define TFT_VER_RES   240
#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10 * (LV_COLOR_DEPTH / 8))

// touchscreen and LVGL global setup
#define XPT2046_IRQ 36
#define XPT2046_MOSI 32
#define XPT2046_MISO 39
#define XPT2046_CLK 25
#define XPT2046_CS 33

SPIClass touchscreenSpi = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);

uint16_t touchScreenMinimumX = 200, touchScreenMaximumX = 3700, touchScreenMinimumY = 240, touchScreenMaximumY = 3800;
uint32_t lastTick = 0;

void my_touchpad_read(lv_indev_t * indev, lv_indev_data_t * data) {
    if (touchscreen.touched()) {
        TS_Point p = touchscreen.getPoint();
        data->point.x = map(p.x, touchScreenMinimumX, touchScreenMaximumX, 0, TFT_HOR_RES);
        data->point.y = map(p.y, touchScreenMinimumY, touchScreenMaximumY, 0, TFT_VER_RES);
        data->state = LV_INDEV_STATE_PRESSED;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

// onboard LEDs
#define CYD_LED_RED 4
#define CYD_LED_BLUE 17
#define CYD_LED_GREEN 16

#define RED_FILTER 1
#define GREEN_FILTER 0.3
#define BLUE_FILTER 0.6

// Global variables for labels
lv_obj_t *status_label;
lv_obj_t *datetime_label;
lv_obj_t *wlan_label;
lv_obj_t *lock_label;

void create_status_bar() {
// Create a main screen
    lv_obj_t *screen = lv_scr_act();

    // Create a status bar container
    lv_obj_t *status_bar = lv_obj_create(screen);
    lv_obj_set_size(status_bar, lv_disp_get_hor_res(NULL), 30); // Full width, 30px height
    lv_obj_align(status_bar, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_set_style_bg_color(status_bar, lv_color_make(255, 255, 255), 0); // White background

    // Disable scrolling
    lv_obj_clear_flag(status_bar, LV_OBJ_FLAG_SCROLLABLE); // Completely remove scrolling

    // Add a label for the status text
    status_label = lv_label_create(status_bar);
    lv_label_set_text(status_label, LV_SYMBOL_POWER);
    //lv_label_set_style_text_color(status_label, lv_color_make(0, 0, 0), 0); // Black text
    lv_obj_align(status_label, LV_ALIGN_LEFT_MID, 5, 0);

    // Add a battery indicator
    wlan_label = lv_label_create(status_bar);
    lv_label_set_text(wlan_label, LV_SYMBOL_WIFI);
    //lv_label_set_style_text_color(wlan_label, lv_color_make(0, 0, 0), 0); // Black text
    lv_obj_align(wlan_label, LV_ALIGN_LEFT_MID, 30, 0);

    // Add a lock indicator
    lock_label = lv_label_create(status_bar);
    lv_label_set_text(lock_label, LV_SYMBOL_OK);
    //lv_label_set_style_text_color(wlan_label, lv_color_make(0, 0, 0), 0); // Black text
    lv_obj_align(lock_label, LV_ALIGN_RIGHT_MID, -15, 0);


    // Add a date and time label
    datetime_label = lv_label_create(status_bar);
    lv_label_set_text(datetime_label, "00/00/00 00:00:00"); // Placeholder text
    lv_obj_set_style_text_color(datetime_label, lv_color_make(0, 0, 0), 0); // Black text
    lv_obj_align(datetime_label, LV_ALIGN_CENTER, 0, 0); // Align in the center
}

// Function to update the date and time
void update_datetime() {
    // Mockup date and time
    int year = 2024;  // Mockup year
    int month = 12;   // Mockup month
    int day = 25;     // Mockup day
    int hour = 10;    // Mockup hour
    int minute = 30;  // Mockup minute
    int second = 45;  // Mockup second

    char buffer[20];
    snprintf(buffer, sizeof(buffer), "%02d/%02d/%02d %02d:%02d:%02d",
             day, month, year % 100,
             hour, minute, second);
    lv_label_set_text(datetime_label, buffer); // Update the label text
}

void setup() {
    Serial.begin(115200);

    lv_init(); // initialize LVGL

    touchscreenSpi.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS); // start second SPI bus for touchscreen
    touchscreen.begin(touchscreenSpi); // touchscreen init
    touchscreen.setRotation(3); // adjust as necessary
    
    uint8_t* draw_buf = new uint8_t[DRAW_BUF_SIZE];
    lv_display_t * disp = lv_tft_espi_create(TFT_HOR_RES, TFT_VER_RES, draw_buf, DRAW_BUF_SIZE);

    lv_obj_t *scr = lv_scr_act(); // get the active screen object
    lv_obj_set_style_bg_color(scr, lv_color_black(), 0); // set the background of the active screen object to black

    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, my_touchpad_read); // set the touchpad read function

    // set up the RGB-LED
    pinMode(CYD_LED_RED, OUTPUT);
    pinMode(CYD_LED_GREEN, OUTPUT);
    pinMode(CYD_LED_BLUE, OUTPUT);

    Serial.println("Setup done");

    create_status_bar();
}

void loop() {
    lv_tick_inc(millis() - lastTick); // update the tick timer
    lastTick = millis();
    lv_timer_handler(); // update the LVGL UI

    // set the RGB-LED colors using the global sliderColor variable
    // the value if inverted - 0 makes the light glow maximally and 255 makes the light the darkest possible
    // a filter is applied to balance the colors
    analogWrite(CYD_LED_RED, 255-(255 *RED_FILTER));
    analogWrite(CYD_LED_GREEN, 255-(0 *GREEN_FILTER));
    analogWrite(CYD_LED_BLUE, 255-(0 *BLUE_FILTER));

    update_datetime();    
    delay(1000);
}