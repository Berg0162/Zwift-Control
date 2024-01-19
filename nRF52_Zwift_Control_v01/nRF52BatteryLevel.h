

// Uncomment when appropriate
#define BATTERY 

// Battery level determination defines
const uint32_t vbat_pin = PIN_VBAT;         // A7 for feather nRF52832, A6 for nRF52840
#define VBAT_MV_PER_LSB   (0.73242188F)     // 3.0V ADC range and 12-bit ADC resolution = 3000mV/4096
#if NRF52840_XXAA
  #define VBAT_DIVIDER      (0.5F)          // 150K + 150K voltage divider on VBAT
  #define VBAT_DIVIDER_COMP (2.0F)          // Compensation factor for the VBAT divider
#else
  #define VBAT_DIVIDER      (0.71275837F)   // 2M + 0.806M voltage divider on VBAT = (2M / (0.806M + 2M))
  #define VBAT_DIVIDER_COMP (1.403F)        // Compensation factor for the VBAT divider
#endif
#define REAL_VBAT_MV_PER_LSB (VBAT_DIVIDER_COMP * VBAT_MV_PER_LSB)

// Raw ADC reading for Battery level
float vbat_mv = 0.0;

float readVBAT(void) {
  float raw;
  // Get the raw 12-bit, 0..3000mV ADC value
  raw = analogRead(vbat_pin);
  // Convert the raw value to compensated mv, taking the resistor-
  // divider into account (providing the actual LIPO voltage)
  // ADC range is 0..3000mV and resolution is 12-bit (0..4095)
  return raw * REAL_VBAT_MV_PER_LSB;
}

uint8_t mvToPercent(float mvolts) {
  if (mvolts < 3300)
    return 0;
  if (mvolts < 3600) {
    mvolts -= 3300;
    return mvolts/30;
  }
  mvolts -= 3600;
  return uint8_t(10+(mvolts*0.15F));  // that's mvolts /6.66666666
}

void setupBatteryState(void) {
  // Setup ADC pins of Feather nRF52 (Battery and/or PS2 Joy Stick)
  // Set the analog reference to 3.0V (default = 3.6V)
  analogReference(AR_INTERNAL_3_0);
  // Set the resolution to 12-bit (0..4095)
  analogReadResolution(12); // Can be 8, 10, 12 or 14
  // Let the ADC settle
  delay(1); 
}

uint8_t getBatteryState(void) {
  // Get raw ADC reading in MilliVolts for Battery level
  vbat_mv = readVBAT();
  // Convert from raw MilliVolts to percentage (based on LIPO chemistry)
 return mvToPercent(vbat_mv);
}
