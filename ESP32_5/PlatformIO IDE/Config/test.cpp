#include <Arduino.h>
#include <esp32_smartdisplay.h>

void lv_device_info(void)
{
    /* Change the active screen's background color */
    lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0x003a57), LV_PART_MAIN);

    
    /* Create the first label for board name */
    lv_obj_t * label_name = lv_label_create(lv_screen_active());
    lv_label_set_text(label_name, "Board: ESP32 5\" Display");
    lv_obj_set_style_text_color(label_name, lv_color_hex(0xffffff), LV_PART_MAIN);
    lv_obj_align(label_name, LV_ALIGN_CENTER, 0, -20);  // Center it with slight upward shift

    /* Create the second label for CPU info */
    lv_obj_t * label_cpu = lv_label_create(lv_screen_active());
    lv_label_set_text(label_cpu, "CPU: ESP32-S3, CPU Freq: 240 MHz, 2 core(s)");
    lv_obj_set_style_text_color(label_cpu, lv_color_hex(0xffffff), LV_PART_MAIN);
    lv_obj_align(label_cpu, LV_ALIGN_CENTER, 0, 20);  // Center it below the first label
}

void setup()
{
  delay(1000);

  Serial.begin(115200);
  Serial.setDebugOutput(true);
  log_i("Board: %s", BOARD_NAME);
  log_i("CPU: %s rev%d, CPU Freq: %d Mhz, %d core(s)", ESP.getChipModel(), ESP.getChipRevision(), getCpuFrequencyMhz(), ESP.getChipCores());
  log_i("Free heap: %d bytes", ESP.getFreeHeap());
  log_i("Free PSRAM: %d bytes", ESP.getPsramSize());
  log_i("SDK version: %s", ESP.getSdkVersion());
  smartdisplay_init();
  lv_device_info();

  auto display = lv_display_get_default();
  //lv_display_set_rotation(display, LV_DISPLAY_ROTATION_90);
  //lv_display_set_rotation(display, LV_DISPLAY_ROTATION_180);
  lv_display_set_rotation(display, LV_DISPLAY_ROTATION_270);
}

auto lv_last_tick = millis();

void loop()
{
    auto const now = millis();
    // Update the ticker
    lv_tick_inc(now - lv_last_tick);
    lv_last_tick = now;
    // Update the UI
    lv_timer_handler();
}