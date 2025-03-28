
/*
The battery voltage is divided by 1/2 with 2 resistors and connected to the PIN_VBAT ADC port so that the voltage can be monitored.
The LiPo datasheet says nominally 3100 mV full scale AD conversion, but there is a large variation from chip to chip, actually ±10%. 
Fortunately, the calibrated correction value for each chip is written in the fuse area, and by using the function analogReadMilliVolts(), 
one gets the corrected voltage value. In addition, during communication in particular, spike-like errors occurred, which had to be averaged
over 16 readings to remove them.
*/

// Uncomment when appropriate
#define BATTERY 

// Battery level defines and "local" variables
// Tested with Adafruit Feather ESP32 V2
// https://learn.adafruit.com/adafruit-esp32-feather-v2/power-management-2#measuring-battery-3122383
// A13 is hardwired to measure battery level on the Adafruit Feather ESP32 V2,
// A0 custom wired XIAO ESP32S3 Sense 
const uint8_t PIN_VBAT = A13;  // Change pin ID to comply with your setup
float vbat_mv = 0.0;

// Raw battery ADC reading in milliVolts 
float readVBAT(void) {
  uint32_t Vbatt = 0;
  for(uint8_t i = 0; i < 16; i++) {
    Vbatt = Vbatt + analogReadMilliVolts(PIN_VBAT); // ADC with corrections applied!  
  }
  return 2*(float)Vbatt/16; // Corrected for 220K + 220K voltage divider on VBAT and averaged !
}

// Convert raw milliVolts to percentage (based on LIPO chemistry)
uint8_t mvToPercent(float mvolts) { 
// LiPoly voltage of a 3.7 battery is at Max: 4200 mV
// In pratice: My Lipo battery does not exceed 4100 mV fully charged!!
// Some correction of not linear load curve at lower levels
  if (mvolts < 3300)
    return 0;
  if (mvolts < 3600) { 
    mvolts -= 3300;
    return (uint8_t)mvolts/30; // max 10% loaded
  }
  return uint8_t(100*mvolts/4100);
}

uint8_t getBatteryState(void) {
    vbat_mv = readVBAT();               // Get raw ADC reading in milliVolts for battery level
    return mvToPercent(vbat_mv);        // Convert raw milliVolts to percentage (based on LIPO chemistry)
}

void setupBatteryState(void) {
    pinMode(PIN_VBAT, INPUT);           // ADC Battery Level
    // Default is 12 bits (range from 0 to 4096) for all chips except 
    // ESP32S3 where the default is 13 bits (range from 0 to 8191).
    // Set the resolution to 12 bits
    analogReadResolution(12);
}
