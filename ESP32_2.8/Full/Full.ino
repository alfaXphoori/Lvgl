#include <lvgl.h>
#include <TFT_eSPI.h>
#include <XPT2046_Touchscreen.h>
#include <string.h>
#include <time.h>

// Touchscreen pins
#define XPT2046_IRQ 36   // T_IRQ
#define XPT2046_MOSI 32  // T_DIN
#define XPT2046_MISO 39  // T_OUT
#define XPT2046_CLK 25   // T_CLK
#define XPT2046_CS 33    // T_CS

SPIClass touchscreenSPI = SPIClass(VSPI);
XPT2046_Touchscreen touchscreen(XPT2046_CS, XPT2046_IRQ);

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 320

#define DRAW_BUF_SIZE (SCREEN_WIDTH * SCREEN_HEIGHT / 10 * (LV_COLOR_DEPTH / 8))
uint32_t draw_buf[DRAW_BUF_SIZE / 4];

// Touchscreen coordinates: (x, y) and pressure (z)
int x, y, z;

// GUI elements
lv_obj_t *status_bar;
lv_obj_t *date_label;
lv_obj_t *time_label;
lv_obj_t *wifi_status_icon;

// Log function for LVGL
void log_print(lv_log_level_t level, const char *buf) {
    LV_UNUSED(level);
    Serial.println(buf);
    Serial.flush();
}

// Touchscreen read function for LVGL
void touchscreen_read(lv_indev_t *indev, lv_indev_data_t *data) {
    if (touchscreen.tirqTouched() && touchscreen.touched()) {
        TS_Point p = touchscreen.getPoint();
        x = map(p.x, 200, 3700, 1, SCREEN_WIDTH);
        y = map(p.y, 240, 3800, 1, SCREEN_HEIGHT);
        z = p.z;

        data->state = LV_INDEV_STATE_PRESSED;
        data->point.x = x;
        data->point.y = y;
    } else {
        data->state = LV_INDEV_STATE_RELEASED;
    }
}

// Function to update the time and date on the status bar
void update_date_time_labels() {
    // Get the current time (replace with a real-time clock or NTP if available)
    time_t now = time(nullptr);
    struct tm *timeinfo = localtime(&now);

    // Format the date as DD-MM-YYYY
    char date_str[16];
    snprintf(date_str, sizeof(date_str), "%02d-%02d-%04d", timeinfo->tm_mday, timeinfo->tm_mon + 1, timeinfo->tm_year + 1900);
    lv_label_set_text(date_label, date_str); // Update date label

    // Format the time as HH:MM:SS
    char time_str[16];
    snprintf(time_str, sizeof(time_str), "%02d:%02d:%02d", timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    lv_label_set_text(time_label, time_str); // Update time label
}

// Function to update WiFi status on the status bar
void update_wifi_status() {
    // Set WiFi status icon (dummy example, replace with real WiFi status check)
    bool wifi_connected = true; // Replace with actual WiFi connection check
    if (wifi_connected) {
        lv_label_set_text(wifi_status_icon, LV_SYMBOL_WIFI " Connected");
    } else {
        lv_label_set_text(wifi_status_icon, LV_SYMBOL_CLOSE " Disconnected");
    }
}

// Create the status bar
void create_status_bar() {
    // Create a container for the status bar
    status_bar = lv_obj_create(lv_scr_act());
    lv_obj_set_size(status_bar, SCREEN_WIDTH, 30); // Full width, height of 30
    lv_obj_set_style_bg_color(status_bar, lv_color_hex(0x000000), LV_PART_MAIN);
    lv_obj_align(status_bar, LV_ALIGN_TOP_MID, 0, 0);
    lv_obj_remove_style(status_bar, NULL, LV_PART_SCROLLBAR | LV_STATE_ANY);

    // Add a date label to the status bar
    date_label = lv_label_create(status_bar);
    lv_label_set_text(date_label, "00-00-0000"); // Default date
    lv_obj_align(date_label, LV_ALIGN_LEFT_MID, 10, 0);
    lv_obj_set_style_text_color(date_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN); // Set text color to white

    // Add a time label to the status bar
    time_label = lv_label_create(status_bar);
    lv_label_set_text(time_label, "00:00:00"); // Default time
    lv_obj_align(time_label, LV_ALIGN_CENTER, 0, 0);
    lv_obj_set_style_text_color(time_label, lv_color_hex(0xFFFFFF), LV_PART_MAIN); // Set text color to white

    // Add a WiFi status icon to the status bar
    wifi_status_icon = lv_label_create(status_bar);
    lv_label_set_text(wifi_status_icon, LV_SYMBOL_WIFI " Disconnected");
    lv_obj_align(wifi_status_icon, LV_ALIGN_RIGHT_MID, -10, 0);
    lv_obj_set_style_text_color(wifi_status_icon, lv_color_hex(0xFFFFFF), LV_PART_MAIN); // Set text color to white
}

// Create GUI elements
void create_gui() {
    create_status_bar();
}

void setup() {
    Serial.begin(115200);
    Serial.printf("LVGL Version: %d.%d.%d\n", lv_version_major(), lv_version_minor(), lv_version_patch());

    // Initialize LVGL
    lv_init();
    lv_log_register_print_cb(log_print);

    // Initialize touchscreen
    touchscreenSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
    touchscreen.begin(touchscreenSPI);
    touchscreen.setRotation(2);

    // Initialize TFT display
    lv_disp_t *disp = (lv_disp_t *)lv_tft_espi_create(SCREEN_WIDTH, SCREEN_HEIGHT, draw_buf, sizeof(draw_buf)); // Cast to lv_disp_t*
    lv_disp_set_rotation(disp, LV_DISP_ROTATION_270);

    // Initialize touchscreen input for LVGL
    lv_indev_t *indev = lv_indev_create();
    lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(indev, touchscreen_read);

    // Create the GUI
    create_gui();
}

void loop() {
    lv_timer_handler();  // Handle LVGL tasks
    lv_tick_inc(5);      // Notify LVGL of time elapsed

    // Periodically update the status bar
    update_date_time_labels();
    update_wifi_status();

    delay(1000); // Update every second
}
