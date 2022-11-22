/****************************************************************************************************************************
  multiFileProject.ino

  For nRF52-based boards using Arduino mbed_nano core, such as Nano_33_BLE
  Written by Khoi Hoang

  Built by Khoi Hoang https://github.com/khoih-prog/nRF52_MBED_Slow_PWM
  Licensed under MIT license
*****************************************************************************************************************************/

// To demo how to include files in multi-file Projects

#if !( ARDUINO_ARCH_NRF52840 && TARGET_NAME == ARDUINO_NANO33BLE )
  #error This code is designed to run on nRF52-based Nano-33-BLE boards using mbed-RTOS platform! Please check your Tools->Board setting.
#endif

#define NRF52_MBED_SLOW_PWM_VERSION_MIN_TARGET      F("megaAVR_SLOW_PWM v1.2.2")
#define NRF52_MBED_SLOW_PWM_VERSION_MIN             1002002

#include "multiFileProject.h"

// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include "nRF52_MBED_Slow_PWM.h"

void setup()
{
  Serial.begin(115200);

  while (!Serial && millis() < 5000);

  Serial.println("\nStart multiFileProject on ");
  Serial.println(BOARD_NAME);
  Serial.println(NRF52_MBED_SLOW_PWM_VERSION);

#if defined(NRF52_MBED_SLOW_PWM_VERSION_MIN)

  if (NRF52_MBED_SLOW_PWM_VERSION_INT < NRF52_MBED_SLOW_PWM_VERSION_MIN)
  {
    Serial.print("Warning. Must use this example on Version equal or later than : ");
    Serial.println(NRF52_MBED_SLOW_PWM_VERSION_MIN_TARGET);
  }

#endif
}

void loop()
{
  // put your main code here, to run repeatedly:
}
