/**
 *
 *          Simple Zwift NimBLE Remote Control with only 2 buttons
 *
 * Short:                     Zwift-Control
 *
 * The code turns the ESP32 into a 2 button (with 3 states) NimBLE controller. 
 * Optimized for use with Zwift game to remotely initiate specific game actions that
 * are mapped (by the Zwift app) to different keys of the regular PC-keyboard (Windows).
 *
 *              Tested with:
 *                          Adafruit Feather ESP32 V2 a.k.a. Huzzah
 *                          LilyGo ESP32S3 T-Display
 *                          Seeed Studion XIAO ESP32S3 Sense
 *
 * The code uses the ESP32 NimBLE libraries see: https://github.com/h2zero/NimBLE-Arduino version 2.x
 *
 * The code is using the ESP32 NIMBLE Keyboard library see: https://github.com/Berg0162/ESP32-NIMBLE-Keyboard
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
#include "esp32BatteryLevel.h" 
// Battery level percentage value 0-100%
uint8_t batteryPercentage = 90; // Default value

#include <NimBleKeyboard.h>
BleKeyboard zwiftKeyboard("Zwift-Control", "Espressif", batteryPercentage);
uint8_t buttonPressed = 0;
bool IsActiveSession = false;

#include "esp32ButtonControl.h"

void setup() {
#ifdef DEBUG
    Serial.begin(115200);
    while (!Serial) delay(10); 
    Serial.flush();
    delay(1000); // Give Serial I/O time to settle
#endif
    DEBUG_PRINTLN("");
    DEBUG_PRINTLN("NimBLE 2.x Server 2 Button Control for Zwift Game");
    DEBUG_PRINTLN("---------------  ESP32 dev board  ---------------");
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
  zwiftKeyboard.setBatteryLevel(batteryPercentage); // Update present battery level percentage

  DEBUG_PRINTLN("Setup NimBLE and advertise \"Zwift-Control\" HID keyboard!");
  zwiftKeyboard.begin();
  while(!zwiftKeyboard.isConnected()) delay(10); // wait for connection!
  DEBUG_PRINTLN("Zwift-Control keyboard is successfully connected!");
  IsActiveSession = true;
  setupButtonControl();
}

#ifdef BATTERY
bool isTimeToNotifyBatteryLevel(void) {
  static unsigned long notifyDelayTime = 0;
  if(millis() >= notifyDelayTime) {
    notifyDelayTime = millis() + 30000; // Set next moment after 30 seconds
    return true;
  } 
  return false;
}
#endif

void loop() {
  if(!zwiftKeyboard.isConnected()) {
    if(IsActiveSession) {
        DEBUG_PRINTLN(" --> Zwift-Control Keyboard Disconnected: Advertising Again!");
        IsActiveSession = false;
    }
    delay(100);
    return;
  }
  if(!IsActiveSession) {
    DEBUG_PRINTLN(" --> Zwift-Control Keyboard Reconnected!");
    IsActiveSession = true;
  }

  buttonPressed = getButtonState();
#ifdef BATTERY     // When "esp32BatteryLevel.h" is included -> BATTERY is defined   
  if(zwiftKeyboard.isConnected() & isTimeToNotifyBatteryLevel()) {
    batteryPercentage = getBatteryState();
    zwiftKeyboard.setBatteryLevel(batteryPercentage); // Update present battery level percentage
    DEBUG_PRINTF("Notify Battery Level: %3d%%\n", batteryPercentage);
  } 
#endif 
  if(zwiftKeyboard.isConnected() & (buttonPressed != 0x00) ) {
    DEBUG_PRINTF("Sending Key pressed: [%d]\n", buttonPressed);
    zwiftKeyboard.press(buttonPressed);
    delay(10);
    if(buttonPressed == KEY_DOWN_ARROW) {
       delay(1350); // Zwift demands to wait before releasing, otherwise NO effect!
    }
    zwiftKeyboard.release(buttonPressed);    // this releases the button
    // Handle second set of keys that are valid only in workout mode
    if(buttonPressed == KEY_LEFT_ARROW) {
       zwiftKeyboard.press(KEY_PAGE_UP);     // Page Up      Increase FTP
       delay(10);
       zwiftKeyboard.release(KEY_PAGE_UP);   // this releases the button 
    }
    if(buttonPressed == KEY_RIGHT_ARROW) {
       zwiftKeyboard.press(KEY_PAGE_DOWN);   // Page Down    Decrease FTP
       delay(10);
       zwiftKeyboard.release(KEY_PAGE_DOWN); // this releases the button
    }
  }
}