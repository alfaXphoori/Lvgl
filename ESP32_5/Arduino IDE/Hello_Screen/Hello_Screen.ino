//=====================================================================
// HARD              : ESP32_8048S050
//  Display          : 5" 800x480 RGB LCD Touch GT911
//  Dev environment  : Arduino IDE 2.3.4
//  Library          : GFX Library Arduino 1.5.0
//  Board            : "ESP32S3 Dev Module"
//  USB CDC On Boot  : "Enabled"
//  CPU Frequency    : "240MHz (WiFi/BT)"
//  Core Debug Level : "None"
//  USB DFE On Boot  : "Disable"
//  Erase All Flash before Sketch Upload : "Disable"
//  Events Run On    : "Core 1"
//  Flash Mode       : "QIO 120MHz"
//  Flash Size       : "16MB (128Mb)"
//  JTAG Adapter     : "Disabled"
//  Arduino Runs On  : "Core 1"
//  USB Firmware MSC On Boot : "Disable"
//  Partition Scheme : "16MB Flash (2MB APP/12.5MB FATFS)"
//  PSRAM            : "OPI PSRAM"
//  Upload Mode      : "UART0/Hardware CDC"
//  Upload Speed     : "921600"
//  USB Mode         : "Hardware CDC and JTAG"
//  Zigbee Mode      : "Disabled"
//=====================================================================

#include <Arduino_GFX_Library.h>
 Arduino_ESP32RGBPanel *rgbpanel = new Arduino_ESP32RGBPanel(
     40 /* DE */, 41 /* VSYNC */, 39 /* HSYNC */, 42 /* PCLK */,
     45 /* R0 */, 48 /* R1 */, 47 /* R2 */, 21 /* R3 */, 14 /* R4 */,
     5 /* G0 */, 6 /* G1 */, 7 /* G2 */, 15 /* G3 */, 16 /* G4 */, 4 /* G5 */,
     8 /* B0 */, 3 /* B1 */, 46 /* B2 */, 9 /* B3 */, 1 /* B4 */,
     0 /* hsync_polarity */, 8 /* hsync_front_porch */, 4 /* hsync_pulse_width */, 8 /* hsync_back_porch */,
     0 /* vsync_polarity */, 8 /* vsync_front_porch */, 4 /* vsync_pulse_width */, 8 /* vsync_back_porch */,
     1 /* pclk_active_neg */, 16000000 /* prefer_speed */);
 Arduino_RGB_Display *gfx = new Arduino_RGB_Display(
     800 /* width */, 480 /* height */, rgbpanel, 0 /* rotation */, true /* auto_flush */);

void setup(void){
  Serial.begin(115200);                    //
  gfx->begin();                            //
  pinMode(2,OUTPUT); digitalWrite(2,HIGH); // Back Light ON
  gfx->setRotation(1);
  gfx->fillScreen(BLACK);

  gfx->setCursor(10, 10);
  gfx->setTextColor(RED);
  gfx->println("Hello World!");
  delay(5000); // 5 seconds
}

void loop(){
    gfx->setCursor(random(gfx->width()), random(gfx->height()));
    gfx->setTextColor(random(0xffff), random(0xffff));
    gfx->setTextSize(random(6) /* x scale */, random(6) /* y scale */, random(2) /* pixel_margin */);
    gfx->println("Hello World!");

    delay(1000); // 1 second
}