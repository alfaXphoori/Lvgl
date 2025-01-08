
## Using LVGL via PlatformIO IDE

1. Install PlatformIO IDE in extention VSCODE

2. Install Board ESP to PlatformIO
    - Open tab platforms
    - Select tab Embedded
    - Find ESP in Search Box
    - Install Espressif32 & Espressif8266

3. Add Custom Boards
    - Go Directory 
        - Example C:\Users\myname\.platformio\platforms\espressif32\boards
    - Download json file and add to directory
        - https://github.com/rzeldent/platformio-espressif32-sunton/blob/main/esp32-2432S028Rv2.json

4. Create New Project
    - Board: esp32-2432S028Rv2
    - Framework: Arduino
    
5. Add Library
    - Go Directory and Open platformio.ini
    - Add XPT2046 library
        - Add https://github.com/PaulStoffregen/XPT2046_Touchscreen.git
    - Add TFT_eSPI library
        - Open library tab
        - Find TFT_eSPI
        - Add to Project

6. Replace code in User_Setup.h
    - Go Directory .pio/libdeps/esp32-2432s028Rv2/TFT_eSPI
        - Replace code User_Setup.h

7. Test Display with library XPT2046 & TFT_eSPI
    - Raplace Code from Test_Lib_XPT_TFT.cpp to src/main.cpp

8. Build & Upload Code
    - Go File platformio.ini
        - Add   monitor_speed = 115200
                upload_port = COM12 
    - Build Project from Left bottom icon PlatformIO : Build
    - Upload Project from Left bottom icon PlatformIO : Upload

9. Add LVGL and Config
    - Add Library Lvgl
        - Open library tab
        - Find lvgl
        - Add to Project
    - Configuration
        - Create lv_conf.h in directory .pio/libdeps/esp32-2432S028Rv2/lvgl
        - Add code in lv_conf.h

10. Test Lvgl
    - Replace code form Lvgl_RGB.cpp to src/main.cpp
    - Build * Upload 

### Enjoy it.