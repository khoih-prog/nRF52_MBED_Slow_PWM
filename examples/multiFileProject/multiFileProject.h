/****************************************************************************************************************************
  multiFileProject.h

  For nRF52-based boards using Arduino mbed_nano core, such as Nano_33_BLE
  Written by Khoi Hoang

  Built by Khoi Hoang https://github.com/khoih-prog/nRF52_MBED_Slow_PWM
  Licensed under MIT license
*****************************************************************************************************************************/

// To demo how to include files in multi-file Projects

#pragma once

#define USING_MICROS_RESOLUTION       true    //false 

// Default is true, uncomment to false
//#define CHANGING_PWM_END_OF_CYCLE     false

// Can be included as many times as necessary, without `Multiple Definitions` Linker Error
#include "nRF52_MBED_Slow_PWM.hpp"
