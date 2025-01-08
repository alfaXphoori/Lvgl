#include <lvgl.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <string.h>

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
lv_obj_t *screen;
lv_obj_t *qr;

// Function to create a QR code
void create_qr_code() {

    // Set the QR code colors using RGB
    lv_color_t bg_color = lv_color_make(255, 255, 255); // White background
    lv_color_t fg_color = lv_color_make(0, 0, 0);    // Blue foreground

    // Create the QR code object
    lv_obj_t *qr = lv_qrcode_create(lv_scr_act()); // Use the active screen
    lv_qrcode_set_size(qr, 150); // Set QR code size
    lv_qrcode_set_dark_color(qr, fg_color); // Set the QR code dark color (foreground)
    lv_qrcode_set_light_color(qr, bg_color); // Set the QR code light color (background)

    // Set QR code data
    const char *data = "https://lvgl.io";
    lv_qrcode_update(qr, data, strlen(data)); // Update the QR code with data
    lv_obj_center(qr); // Center the QR code on the screen

    // Add a border with foreground color
    lv_obj_set_style_border_color(qr, fg_color, 0);
    lv_obj_set_style_border_width(qr, 5, 0);

    // Create a label for the text below the QR code
    lv_obj_t *label = lv_label_create(lv_scr_act());
    lv_label_set_text(label, "Scan for Rent room");
    lv_obj_set_style_text_color(label, lv_color_make(0, 0, 0), 0); // Black text
    lv_obj_set_style_text_font(label, &lv_font_montserrat_14, 0); // Optional: Set font
    lv_obj_align_to(label, qr, LV_ALIGN_OUT_BOTTOM_MID, 0, 10); // Align below the QR code
}

void setup() {
    Serial.begin(115200);

    lv_init(); // initialize LVGL

    touchscreenSpi.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS); // start second SPI bus for touchscreen
    touchscreen.begin(touchscreenSpi); // touchscreen init
    touchscreen.setRotation(3); // adjust as necessary
    
    uint8_t* draw_buf = new uint8_t[DRAW_BUF_SIZE];
    lv_display_t * disp = lv_tft_espi_create(TFT_HOR_RES, TFT_VER_RES, draw_buf, DRAW_BUF_SIZE);

    screen = lv_scr_act(); // get the active screen object
    lv_obj_set_style_bg_color(screen, lv_color_white(), 0); // set the background of the active screen object to black

    lv_indev_t * indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, my_touchpad_read); // set the touchpad read function

    // set up the RGB-LED
    pinMode(CYD_LED_RED, OUTPUT);
    pinMode(CYD_LED_GREEN, OUTPUT);
    pinMode(CYD_LED_BLUE, OUTPUT);

    Serial.println("Setup done");

    // Create QR code
    create_qr_code();
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

    delay(1000);
}