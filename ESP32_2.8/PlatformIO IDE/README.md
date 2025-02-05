# 🚀 Using LVGL with ESP32 via PlatformIO in VS Code

This guide walks you through setting up **LVGL** on an **ESP32** board using **PlatformIO** in **VS Code**. Follow the steps below to get your display up and running!

## 📌 Prerequisites

- **VS Code** installed  
- **PlatformIO IDE** extension installed  
- **ESP32 development board** (e.g., `esp32-2432S028Rv2`)  

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

## 🏰️ Step 3: Add Custom Board

1. Navigate to the PlatformIO boards directory:  
   ```plaintext
   C:\Users\myname\.platformio\platforms\espressif32\boards
   ```  
2. Download the **board configuration file**:  
   - [esp32-2432S028Rv2.json](https://github.com/rzeldent/platformio-espressif32-sunton/blob/main/esp32-2432S028Rv2.json)  
3. Move the `.json` file into the **boards** directory  

---

## 📂 Step 4: Create a New Project

1. Open **PlatformIO** and create a new project  
2. Select:  
   - **Board** → `esp32-2432S028Rv2`  
   - **Framework** → `Arduino`  

---

## 📌 Step 5: Add Required Libraries

### ✅ XPT2046 Touchscreen Library

- Open **platformio.ini**  
- Add the library:  
  ```ini
  lib_deps =
    https://github.com/PaulStoffregen/XPT2046_Touchscreen.git
  ```  

### ✅ TFT_eSPI Display Library

- Open the **Library** tab  
- Search for **TFT_eSPI**  
- Add it to the project  

---

## 🔧 Step 6: Configure TFT_eSPI

1. Navigate to the **TFT_eSPI** library directory:  
   ```plaintext
   .pio/libdeps/esp32-2432s028Rv2/TFT_eSPI
   ```  
2. Replace the **User_Setup.h** file with your custom configuration  

---

## 🖥️ Step 7: Test Display with XPT2046 & TFT_eSPI

1. Replace the code in `src/main.cpp` with **Test_Lib_XPT_TFT.cpp**  
2. Save the file  

---

## 🛠️ Step 8: Build & Upload Code

1. Open **platformio.ini**  
2. Add the following lines:  
   ```ini
   monitor_speed = 115200
   upload_port = COM12
   ```  
3. Click the **Build** button in PlatformIO (bottom left)  
4. Click **Upload** to flash the firmware  

---

## 📲 Step 9: Add LVGL & Configure

1. Open the **Library** tab  
2. Search for **lvgl**  
3. Add it to the project  

### 🔧 LVGL Configuration

1. Navigate to:  
   ```plaintext
   .pio/libdeps/esp32-2432S028Rv2/lvgl
   ```  
2. Create a new file **lv_conf.h**  
3. Add the necessary **LVGL configuration code**  

---

## 📄 Step 10: Move Demo & Example Files

1. Navigate to:  
   ```plaintext
   .pio/libdeps/esp32-2432S028Rv2/lvgl
   ```  
2. Move the **demo and example** files to the `src` directory  

---

## 🏁 Step 11: Test LVGL

1. Replace the code in `src/main.cpp` with **Lvgl_RGB.cpp**  
2. **Build & Upload** the project  

---

## 🎉 Enjoy Your LVGL-Powered ESP32 Display! 🚀  

Now your ESP32 board is running **LVGL**, and you can start building interactive UI components for your projects!  

🔹 **Happy Coding!** 🎨💡  

