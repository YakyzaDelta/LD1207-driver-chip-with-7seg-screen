# ESP32 VFD Clock with millis() Timer

![VFD Display](LD1207/Screenshot%202026-07-20%20183320.jpg)

## Overview
A simple digital clock implementation for ESP32 using a Vacuum Fluorescent Display (VFD) with the LD1207 driver chip. The clock uses `millis()` for timekeeping without blocking delays, making it efficient and responsive.

## Features
- ✅ **Accurate timekeeping** using ESP32's `millis()` function
- ✅ **VFD display support** with LD1207 driver
- ✅ **Blinking colon** (blinks every second)
- ✅ **Animated snake pattern** on the first grid segment
- ✅ **24-hour time format**
- ✅ **Non-blocking operation** - no `delay()` used

## Hardware Requirements
- ESP32 development board
- VFD display module with LD1207 driver
- 3.3V/5V logic level compatible (ESP32 uses 3.3V logic)

### Pin Connections
| ESP32 Pin | VFD Module | Description |
|-----------|------------|-------------|
| GPIO 23   | DIN        | Data input  |
| GPIO 18   | CLK        | Clock signal|
| GPIO 19   | STB        | Strobe signal|

**Note:** The pins can be customized in the code by modifying the following definitions:

#define VFD_in  23  // Data input
#define VFD_clk 18  // Clock
#define VFD_stb 19  // Strobe

Animation Pattern
The snake animation uses an 8-frame sequence:

cpp
const unsigned char snake[] = {
    0b00011000, 0b00001100, 0b01000100, 0b01100000, 
    0b00100001, 0b00000011, 0b01000010, 0b01010000
};
The animation runs at 8 frames per second for smooth visual effects.

Customization
Change Time Format
To switch to 12-hour format, modify the hour increment logic:

cpp
if (hrs >= 12) {
    hrs = 0; // Reset at 12 instead of 24
}
Adjust Brightness
Display brightness is controlled by the command:

cpp
send_command((0b10001000) | 7); // 7 = maximum brightness
Valid brightness values: 0-7 (0 = dimmest, 7 = brightest)

Modify Snake Animation
To change the animation pattern, edit the snake[] array and adjust the frame rate:

cpp
int snakeIndex = (millis() / (1000 / 8)) % 8; // Change 8 to desired FPS
Serial Monitor
The code includes Serial.begin(115200) for debugging. You can add print statements to monitor time:

cpp
Serial.printf("Time: %02d:%02d:%02d\n", hrs, mins, secs);
Installation
PlatformIO
Create a new ESP32 project

Copy the code to src/main.cpp

Connect your VFD module according to the pinout

Build and upload

Arduino IDE
Install ESP32 board support

Create a new sketch

Paste the code

Select your ESP32 board

Upload the sketch

Dependencies
No external libraries required

Uses only built-in Arduino/ESP32 functions

Hardware Notes
Voltage Levels: ESP32 uses 3.3V logic. Ensure your VFD module is compatible or use level shifters

Power Supply: VFD displays typically require 5V supply. Check your module's specifications

Current Draw: VFD modules can draw significant current (100-500mA). Use an adequate power supply

Troubleshooting
Issue	Possible Solution
Display not showing	Check power supply and pin connections
Wrong time displayed	Verify pin definitions match your wiring
Flickering display	Reduce brightness or check power supply stability
No animation	Verify the snake array and frame counter logic
License
This project is open-source and available for personal and commercial use.

Contributing
Feel free to submit issues, feature requests, or pull requests to improve the code.

Credits
Created for ESP32 with VFD display modules using the LD1207 driver.
