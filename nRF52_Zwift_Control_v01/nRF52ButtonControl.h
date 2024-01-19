
// -----------------------------------------------------------------------------------
//      Map and handle specific key functions for Zwift game actions
// -----------------------------------------------------------------------------------

/* ------------------------------------------------------------------------------------------------
 * Warning I/O Pins have identical position but different naming depending on the processor board
 * I/O Pin declarations for connection to push buttons
*/ 
// Two Keys/Buttons settings connected to
#if defined(ARDUINO_NRF52840_FEATHER) 
  #define NRF52_PIN_BUTTON1  A0   // --> A0/P0.04 connected to Right button
  #define NRF52_PIN_BUTTON2  A1   // --> A1/P0.05 connected to Left button
#endif
#if defined(ARDUINO_NRF52832_FEATHER) 
  #define NRF52_PIN_BUTTON1  2    // --> A0/P0.02 connected to Right button
  #define NRF52_PIN_BUTTON2  3    // --> A1/P0.03 connected to Left button
#endif
//-------------------------------------------------------------------------------------------------

// Key to value/function mapping according to Adafruit_TinyUSB_ArduinoCore/tinyusb/src/class/hid.h
/*                                      Key list value       Name         Zwift Action       
                                        HID_KEY_ESC;          // Escape       Show End Ride Screen
                                        HID_KEY_RETURN;       // Return/Enter Confirm choice/Select
                                        HID_KEY_PAGE_UP;      // Page Up      Increase FTP
                                        HID_KEY_PAGE_DOWN;    // Page Down    Decrease FTP
                                        HID_KEY_ARROW_UP;     // Up arrow     Display Action Bar
*/
const uint8_t singleClickButton1Value = HID_KEY_ARROW_LEFT;   // Left arrow   Turn left at intersection
const uint8_t singleClickButton2Value = HID_KEY_ARROW_RIGHT;  // Right arrow  Turn right at intersection

const uint8_t doubleClickButton1Value = HID_KEY_SPACE;        // Space        Power Up
const uint8_t doubleClickButton2Value = HID_KEY_ARROW_DOWN;   // Down arrow   Make an U-turn

const uint8_t longPressStopButton1Value = HID_KEY_1;          // 1-9          Change your Camera View 1 - 9 views
const uint8_t maxCamViewNum =             HID_KEY_9;
const uint8_t longPressStopButton2Value = HID_KEY_TAB;        // Tab          Skip Workout Step

#include <OneButton.h>
/* https://www.arduino.cc/reference/en/libraries/onebutton/
 * Initialize the OneButton library.
 * OneButton::OneButton(const int pin, const boolean activeLow, const bool pullupActive)
 * @param pin           -> The pin to be used for input from a momentary button.
 * @param activeLow     -> Set to true when the input level is LOW when the button is pressed, Default is true.
 * @param pullupActive  -> Activate the internal pullup when available. Default is true.
 */
// Setup button pushed input level is LOW and use internal PullUp resistor 
OneButton zwiftButton1(NRF52_PIN_BUTTON1, true, true);
OneButton zwiftButton2(NRF52_PIN_BUTTON2, true, true);

#define LONGPRESSDELAY 500      // The time delay before LongPressStart is called (to eliminate accidental holding down)
uint8_t _buttonPressed = 0;     // "Local" variable that holds key pressed value

// Define callback functions that handle 3 different "click" states
void singleClickButton1(void) {
  _buttonPressed = singleClickButton1Value;
  DEBUG_PRINTLN("singleClick() on pin #1");
} 

void doubleClickButton1(void) {
  _buttonPressed = doubleClickButton1Value;
  DEBUG_PRINTLN("doubleClick() on pin #1");
} 

void longPressStartButton1(void) {
  DEBUG_PRINTLN("pressStart() on pin #1");
} 

void longPressStopButton1(void) {  
  static uint8_t camViewNum  = longPressStopButton1Value-1;
  if(camViewNum < maxCamViewNum) camViewNum++;
  else camViewNum = longPressStopButton1Value;
  _buttonPressed = camViewNum;
  DEBUG_PRINTLN("pressStop() on pin #1");
}

/*
void duringLongPressButton1() {
// DEBUG_PRINTLN("Button 1 longPress");
} 
*/

void singleClickButton2(void) {
  _buttonPressed = singleClickButton2Value; 
  DEBUG_PRINTLN("singleClick() on pin #2");
} // singleClick

void doubleClickButton2(void) {
  _buttonPressed = doubleClickButton2Value;
  DEBUG_PRINTLN("doubleClick() on pin #2");
}

void longPressStartButton2(void) {
  DEBUG_PRINTLN("pressStart() on pin #2");
}

void longPressStopButton2(void) {
  _buttonPressed = longPressStopButton2Value;
  DEBUG_PRINTLN("pressStop() on pin #2");
}

/*
void duringLongPressButton2() {
//  DEBUG_PRINTLN("Button 2 longPress");
} 
*/

void setupButtonControl(void){
  DEBUG_PRINTLN("Configure HCI device: 2 buttons -> 3 states!");

  // Set the callBack functions
  zwiftButton1.attachClick(singleClickButton1);
  zwiftButton1.attachDoubleClick(doubleClickButton1);

  zwiftButton1.setPressMs(LONGPRESSDELAY); 
  zwiftButton1.attachLongPressStart(longPressStartButton1);
  zwiftButton1.attachLongPressStop(longPressStopButton1);
  //zwiftButton1.attachDuringLongPress(duringLongPressButton1);

  zwiftButton2.attachClick(singleClickButton2);
  zwiftButton2.attachDoubleClick(doubleClickButton2);

  zwiftButton2.setPressMs(LONGPRESSDELAY); 
  zwiftButton2.attachLongPressStart(longPressStartButton2);
  zwiftButton2.attachLongPressStop(longPressStopButton2);
  //zwiftButton2.attachDuringLongPress(duringLongPressButton2);
}

uint8_t getButtonState(void) { 
  _buttonPressed = 0x00; 
  zwiftButton1.tick();
  zwiftButton2.tick();
  return _buttonPressed;
}
