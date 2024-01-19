/**
 *
 *          Simple Zwift BLE Remote Control with only 2 buttons
 *
 * Short:                     Zwift Control
 *
 * The code turns the ESP32 into a 2 button (with 3 states) Bluetooth LE controller. 
 * Optimized for use with Zwift game to remotely initiate specific game actions that
 * are mapped (by the Zwift app) to different keys of the regular PC-keyboard (Windows).
 *
 *              Tested with Adafruit Feather ESP32 V2 a.k.a. Huzzah
 *
 * The code uses the ESP32 NimBLE libraries see: https://github.com/h2zero/NimBLE-Arduino
 *
 * The code is using the ESP32 BLE Keyboard library see: https://github.com/T-vK/ESP32-BLE-Keyboard
 *
**/

// -------------------------------------------------------------------------------------------
// COMPILER DIRECTIVE to allow/suppress DEBUG messages that help debugging...
// Uncomment general "#define DEBUG" to activate
#define DEBUG
// Include these debug utility macros in all cases!
#include "DebugUtils.h"
// -------------------------------------------------------------------------------------------

// LiPo Battery connected ? -> uncomment
//#include "esp32BatteryLevel.h" 
// Battery level percentage value 0-100%
uint8_t batteryPercentage = 90; // Default value

// Without USE_NIMBLE defined --> not working on Windows 10 !!!!
#ifndef USE_NIMBLE
#define USE_NIMBLE
#endif    
#include <BleKeyboard.h>
BleKeyboard bleKeyboard("Zwift Control", "Espressif", batteryPercentage);
uint8_t buttonPressed = 0;

#include "esp32ButtonControl.h"

void setup() {
#ifdef DEBUG
    Serial.begin(115200);
    while (!Serial) delay(10); 
    Serial.flush();
    delay(1000); // Give Serial I/O time to settle
#endif
    DEBUG_PRINTLN("");
    DEBUG_PRINTLN("BLE Server with 2 Button Control for Zwift Game");
    DEBUG_PRINTLN("--------------  ESP32 dev board  --------------");
    delay(500);

// Configure and start the Battery service
  DEBUG_PRINTLN("Configure and start the Battery service");
// When "esp32BatteryLevel.h" is included -> BATTERY is defined
#ifdef BATTERY
    setupBatteryState();
    batteryPercentage = getBatteryState();
    DEBUG_PRINTF("LiPo Battery Setup - Level: %3d%%\n", batteryPercentage);
#else
    DEBUG_PRINTF("NO LiPo Battery! - Default level: %3d%%\n", batteryPercentage);
#endif
  bleKeyboard.setBatteryLevel(batteryPercentage); // Update present battery level percentage

  DEBUG_PRINTLN("Setup BLE and advertise \"Zwift Control\" like a HID keyboard!");
  bleKeyboard.begin();
  while(!bleKeyboard.isConnected()) delay(10); // wait for connection!
  DEBUG_PRINTLN("BLE HID keyboard is successfully connected!");
  setupButtonControl();
}

bool isTimeToNotifyBatteryLevel(void) {
  static unsigned long notifyDelayTime = 0;
  if(millis() >= notifyDelayTime) {
    notifyDelayTime = millis() + 30000; // Set next moment after 30 seconds
    return true;
  } 
  return false;
}

void loop() {
  buttonPressed = getButtonState();
  if(bleKeyboard.isConnected() & isTimeToNotifyBatteryLevel()) {
    // When "esp32BatteryLevel.h" is included -> BATTERY is defined      
    #ifdef BATTERY
    batteryPercentage = getBatteryState();
    #endif
    bleKeyboard.setBatteryLevel(batteryPercentage); // Update present battery level percentage
    DEBUG_PRINTF("Notify Battery Level: %3d%%\n", batteryPercentage);
  }  
  if(bleKeyboard.isConnected() & (buttonPressed != 0x00) ) {
    DEBUG_PRINTF("Sending Key pressed: [%d]\n", buttonPressed);
    bleKeyboard.press(buttonPressed);
    delay(10);
    if(buttonPressed == KEY_DOWN_ARROW) {
       delay(1350); // Zwift demands to wait before releasing, otherwise NO effect!
    }
    bleKeyboard.release(buttonPressed);    // this releases the button
    // Handle second set of keys that are valid only in workout mode
    if(buttonPressed == KEY_LEFT_ARROW) {
       bleKeyboard.press(KEY_PAGE_UP);     // Page Up      Increase FTP
       delay(10);
       bleKeyboard.release(KEY_PAGE_UP);   // this releases the button 
    }
    if(buttonPressed == KEY_RIGHT_ARROW) {
       bleKeyboard.press(KEY_PAGE_DOWN);   // Page Down    Decrease FTP
       delay(10);
       bleKeyboard.release(KEY_PAGE_DOWN); // this releases the button
    }
  }
}
