/**
 *
 *          Simple Zwift BLE Remote Control with only 2 buttons
 *
 * Short:                     Zwift Control
 *
 * The code turns the nRF52 into a 2 button (with 3 states) Bluetooth LE controller. 
 * Optimized for use with Zwift game to remotely initiate specific game actions that
 * are mapped (by the Zwift app) to different keys of the regular PC-keyboard (Windows).
 *
 *              Tested with Adafruit Feather nRF52840 Express
 * 
**/

// -------------------------------------------------------------------------------------------
// COMPILER DIRECTIVE to allow/suppress DEBUG messages that help debugging...
// Uncomment general "#define DEBUG" to activate
#define DEBUG
// Include these debug utility macros in all cases!
#include "DebugUtils.h"
// -------------------------------------------------------------------------------------------

#include <bluefruit.h>

// LiPo Battery connected ? -> uncomment
//#include "nRF52BatteryLevel.h" 
// Battery level percentage value 0-100%
uint8_t batteryPercentage = 90; // Default value
BLEBas blebas;    // Battery helper class instance
BLEDis bledis;    // DIS (Device Information Service) helper class instance

BLEHidAdafruit bleKeyboard;
uint8_t buttonPressed;
#define KEYBOARD_MODIFIER 0
uint8_t keycodes[6] = { HID_KEY_NONE, HID_KEY_NONE , HID_KEY_NONE , HID_KEY_NONE , HID_KEY_NONE , HID_KEY_NONE };
#include "nRF52ButtonControl.h"

void setup() {
#ifdef DEBUG
    Serial.begin(115200);
    while (!Serial) delay(10); 
    Serial.flush();
    delay(1000); // Give Serial I/O time to settle
#endif
    DEBUG_PRINTLN("");
    DEBUG_PRINTLN("BLE Server with 2 Button Control for Zwift Game");
    DEBUG_PRINTLN("--------------  nRF52 dev board  --------------");
    delay(500);

// Configure and start the Battery service
  DEBUG_PRINTLN("Configure and start the Battery service");
// When "nRF52BatteryLevel.h" is included -> BATTERY is defined
#ifdef BATTERY
    setupBatteryState();
    batteryPercentage = getBatteryState();
    DEBUG_PRINTF("LiPo Battery Setup - Level: %3d%%\n", batteryPercentage);
#else
    DEBUG_PRINTF("NO LiPo Battery! - Default level: %3d%%\n", batteryPercentage);
#endif

  DEBUG_PRINTLN("Setup BLE and advertise \"Zwift Control\" like a HID keyboard!");
  Bluefruit.begin(1, 0); // begin (Peripheral = 1, Central = 0)
  Bluefruit.setTxPower(4); // Check bluefruit.h for supported values
  Bluefruit.setName("Zwift Control"); // Set BLE device name
  // Configure and Start Device Information Service
  bledis.setManufacturer("Adafruit Industries");
  bledis.setModel("Feather nRF52");
  bledis.begin();
  // Set first value of Battery Percentage to expose
  blebas.write(batteryPercentage);
  blebas.begin();

  /* Start BLE HID
   * Note: Apple requires BLE device must have min connection interval >= 20m
   * ( The smaller the connection interval the faster we could send data).
   * However for HID and MIDI device, Apple could accept min connection interval 
   * up to 11.25 ms. Therefore BLEHidAdafruit::begin() will try to set the min and max
   * connection interval to 11.25  ms and 15 ms respectively for best performance.
   */
  bleKeyboard.begin();
  // Set callback for set LED from central
  bleKeyboard.setKeyboardLedCallback(set_keyboard_led);

  /* Set connection interval (min, max) to your perferred value.
   * Note: It is already set by BLEHidAdafruit::begin() to 11.25ms - 15ms
   * min = 9*1.25=11.25 ms, max = 12*1.25= 15 ms 
   */
  /* Bluefruit.Periph.setConnInterval(9, 12); */
  // Setup and start advertising
  startAdv();

  while(!Bluefruit.connected()) delay(10); // wait for connection!
  DEBUG_PRINTLN("BLE HID keyboard is successfully connected!");
  setupButtonControl();
}

void startAdv(void)
{  
  // Advertising packet
  Bluefruit.Advertising.addFlags(BLE_GAP_ADV_FLAGS_LE_ONLY_GENERAL_DISC_MODE);
  Bluefruit.Advertising.addTxPower();
  Bluefruit.Advertising.addAppearance(BLE_APPEARANCE_HID_KEYBOARD);
  
  // Include BLE HID service
  Bluefruit.Advertising.addService(bleKeyboard);

  // There is enough room for the dev name in the advertising packet
  Bluefruit.Advertising.addName();
  
  /* Start Advertising
   * - Enable auto advertising if disconnected
   * - Interval:  fast mode = 20 ms, slow mode = 152.5 ms
   * - Timeout for fast mode is 30 seconds
   * - Start(timeout) with timeout = 0 will advertise forever (until connected)
   * 
   * For recommended advertising interval
   * https://developer.apple.com/library/content/qa/qa1931/_index.html   
   */
  Bluefruit.Advertising.restartOnDisconnect(true);
  Bluefruit.Advertising.setInterval(32, 244);    // in unit of 0.625 ms
  Bluefruit.Advertising.setFastTimeout(30);      // number of seconds in fast mode
  Bluefruit.Advertising.start(0);                // 0 = Don't stop advertising after n seconds
}

/**
 * Callback invoked when received Set LED from central.
 * Must be set previously with setKeyboardLedCallback()
 *
 * The LED bit map is as follows: (also defined by KEYBOARD_LED_* )
 *    Kana (4) | Compose (3) | ScrollLock (2) | CapsLock (1) | Numlock (0)
 */
void set_keyboard_led(uint16_t conn_handle, uint8_t led_bitmap)
{
  (void) conn_handle;
  
  // light up Red Led if any bits is set
  if ( led_bitmap )
  {
    ledOn( LED_RED );
  }
  else
  {
    ledOff( LED_RED );
  }
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
  if(Bluefruit.connected() & isTimeToNotifyBatteryLevel()) {
    // When "nRF52BatteryLevel.h" is included -> BATTERY is defined      
    #ifdef BATTERY
    batteryPercentage = getBatteryState();
    #endif
    blebas.notify(batteryPercentage); // Update present battery level percentage
    DEBUG_PRINTF("Notify Battery Level: %3d%%\n", batteryPercentage);
  }
  if(Bluefruit.connected() & (buttonPressed != 0x00) ) {
    DEBUG_PRINTF("Sending Key pressed: [%d]\n", buttonPressed);
    keycodes[0] = buttonPressed;                  // Fill keycodes array
    bleKeyboard.keyboardReport(KEYBOARD_MODIFIER, keycodes);
    if(buttonPressed == HID_KEY_ARROW_DOWN) {     // Down arrow   Make an U-turn
       delay(1350); // Zwift demands to wait before releasing, otherwise NO effect!
    } else delay(5);
    bleKeyboard.keyRelease();                     // this releases the button
    // Handle second set of keys that are valid only in workout mode
    if(buttonPressed == HID_KEY_ARROW_LEFT) {     // Fill keycodes array with second key value
       keycodes[0] = HID_KEY_PAGE_UP;             // Page Up      Increase FTP
       bleKeyboard.keyboardReport(KEYBOARD_MODIFIER, keycodes);
       delay(5);
       bleKeyboard.keyRelease();                  // this releases the button 
    }
    if(buttonPressed == HID_KEY_ARROW_RIGHT) {    // Fill keycodes array with second key value
       keycodes[0] = HID_KEY_PAGE_DOWN;           // Page Down    Decrease FTP
       bleKeyboard.keyboardReport(KEYBOARD_MODIFIER, keycodes);
       delay(5);
       bleKeyboard.keyRelease();                  // this releases the button
    }
  }
}