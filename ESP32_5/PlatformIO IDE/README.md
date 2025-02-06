# 🚀 Using LVGL with ESP32 via PlatformIO in VS Code

This guide walks you through setting up **LVGL** on an **ESP32** board using **PlatformIO** in **VS Code**. Follow the steps below to get your display up and running!

## 📌 Prerequisites

- **VS Code** installed  
- **PlatformIO IDE** extension installed  
- **ESP32-8048S050C Sunton**  

---

## ⚙️ Step 1: Install PlatformIO in VS Code

1. Open **VS Code**  
2. Install the **PlatformIO IDE** extension  

---

## 🛠️ Step 2: Install ESP Board Support in PlatformIO

1. Open the **Platforms** tab in PlatformIO  
2. Select the **Embedded** tab  
3. Search for **ESP** in the search box  
4. Install:  
   - **Espressif32**  
   - **Espressif8266**  

---

## 🏰 Step 3: Add Custom Board

1. Navigate to the **PlatformIO boards directory** (for macOS 15.3 (24D60)):  
   ```plaintext
   /Users/myname/.platformio/platforms/espressif32/boards
   ```  
2. Download the **board configuration file**:  
   - [esp32-8048S050C.json](https://github.com/rzeldent/platformio-espressif32-sunton/blob/main/esp32-8048S050C.json)  
3. Move the `.json` file into the **boards** directory  

---

## 📂 Step 4: Create a New Project

1. Open **PlatformIO** and create a new project  
2. Select:  
   - **Board** → `esp32-8048S050`  
   - **Framework** → `Arduino`  

---

## 📌 Step 5: Add Required Libraries

### ✅ Add Required Library

1. Open **platformio.ini**  
2. Add the following dependency:  
   ```ini
   lib_deps = https://github.com/rzeldent/esp32-smartdisplay.git
   ```  

---

## 🔧 Step 6: Add Build Flags

1. Open **platformio.ini**  
2. Add the following build flags:  
   ```ini
   build_flags =
    -Ofast
    -Wall
    '-D BOARD_NAME="${this.board}"'
    '-D CORE_DEBUG_LEVEL=ARDUHAL_LOG_LEVEL_INFO'
    #'-D CORE_DEBUG_LEVEL=ARDUHAL_LOG_LEVEL_DEBUG'
    #'-D CORE_DEBUG_LEVEL=ARDUHAL_LOG_LEVEL_VERBOSE'
    '-D LV_CONF_PATH=${platformio.include_dir}/lv_conf.h'
   ```

---

## 📝 Step 7: Create `lv_config.h` File

1. Navigate to the **include** directory  
2. Create a new file named **lv_config.h**  
3. Add the necessary configuration (code available in the Config Directory)  

---

## 🔄 Step 8: Test Your Setup

1. Navigate to the **src** directory  
2. Copy the contents of `test.cpp` into `main.cpp`  
3. Build and upload the project to your **ESP32**  

---

## 🎉 Enjoy Your LVGL-Powered ESP32 Display! 🚀  

Now your ESP32 board is running **LVGL**, and you can start building interactive UI components for your projects!  

🔹 **Happy Coding!** 🎨💡

