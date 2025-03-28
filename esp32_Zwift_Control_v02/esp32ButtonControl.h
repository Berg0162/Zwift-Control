
// -----------------------------------------------------------------------------------
//      Map and handle specific key functions for Zwift game actions
// -----------------------------------------------------------------------------------

// Two Keys/Buttons settings connected to
#define PIN_BUTTON1    A0   // GPIO26 internal pullup -> ESP32 Feather V2 
#define PIN_BUTTON2    A1   // GPIO25 internal pullup -> ESP32 Feather V2
//#define PIN_BUTTON1    A4   // GPIO8 external pullup -> XIAO ESP32S3 Sense 
//#define PIN_BUTTON2    A9   // GPIO5 external pullup -> XIAO ESP32S3 Sense 
//#define PIN_BUTTON1     0   // LilyGo ESP32S3 T-Display, builtin button -> use with internal pullup
//#define PIN_BUTTON2    14   // LilyGo ESP32S3 T-Display, builtin button -> use with internal pullup

// Key to value/function mapping
#define KEY_SPACE 0xB4  // Not defined in the Key list of Ble Keyboard header file (?)
/*                                      Key list value       Name         Zwift Action       
                                        KEY_ESC;          // Escape       Show End Ride Screen
                                        KEY_RETURN;       // Return/Enter Confirm choice/Select
                                        KEY_PAGE_UP;      // Page Up      Increase FTP
                                        KEY_PAGE_DOWN;    // Page Down    Decrease FTP
                                        KEY_UP_ARROW;     // Up arrow     Display Action Bar
*/
const uint8_t singleClickButton1Value = KEY_LEFT_ARROW;   // Left arrow   Turn left at intersection
const uint8_t singleClickButton2Value = KEY_RIGHT_ARROW;  // Right arrow  Turn right at intersection

const uint8_t doubleClickButton1Value = KEY_SPACE;        // Space        Power Up
//const uint8_t doubleClickButton2Value = KEY_DOWN_ARROW;   // Down arrow   Make an U-turn
const uint8_t doubleClickButton2Value = KEY_UP_ARROW;     // Up arrow     Display Action Bar

const uint8_t longPressStopButton1Value = 49;             // 49-57 1-9    Change your Camera View 1 - 9 views
//const uint8_t longPressStopButton2Value = KEY_TAB;        // Tab          Skip Workout Step
const uint8_t longPressStopButton2Value = KEY_RETURN;     // Return/Enter Confirm choice/Select

#include <OneButton.h>
/* https://www.arduino.cc/reference/en/libraries/onebutton/
 * Initialize the OneButton library.
 * OneButton::OneButton(const int pin, const boolean activeLow, const bool pullupActive)
 * @param pin           -> The pin to be used for input from a momentary button.
 * @param activeLow     -> Set to true when the input level is LOW when the button is pressed, Default is true.
 * @param pullupActive  -> Activate the internal pullup when available. Default is true.
 */
// Setup button pushed input level is LOW and use internal PullUp resistor 
OneButton zwiftButton1(PIN_BUTTON1, true, true);
OneButton zwiftButton2(PIN_BUTTON2, true, true);

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
  static uint8_t camViewNum  = longPressStopButton1Value; // Initial start with 2
  if(camViewNum < 57) camViewNum++;
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
#ifdef DEBUG
  Serial.setDebugOutput(true); 
#endif

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
