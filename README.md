# nRF52_MBED_Slow_PWM Library

[![arduino-library-badge](https://www.ardu-badge.com/badge/nRF52_MBED_Slow_PWM.svg?)](https://www.ardu-badge.com/nRF52_MBED_Slow_PWM)
[![GitHub release](https://img.shields.io/github/release/khoih-prog/nRF52_MBED_Slow_PWM.svg)](https://github.com/khoih-prog/nRF52_MBED_Slow_PWM/releases)
[![GitHub](https://img.shields.io/github/license/mashape/apistatus.svg)](https://github.com/khoih-prog/nRF52_MBED_Slow_PWM/blob/main/LICENSE)
[![contributions welcome](https://img.shields.io/badge/contributions-welcome-brightgreen.svg?style=flat)](#Contributing)
[![GitHub issues](https://img.shields.io/github/issues/khoih-prog/nRF52_MBED_Slow_PWM.svg)](http://github.com/khoih-prog/nRF52_MBED_Slow_PWM/issues)

<a href="https://www.buymeacoffee.com/khoihprog6" target="_blank"><img src="https://cdn.buymeacoffee.com/buttons/v2/default-yellow.png" alt="Buy Me A Coffee" style="height: 60px !important;width: 217px !important;" ></a>

---
---

## Table of Contents

* [Important Change from v1.2.0](#Important-Change-from-v120)
* [Why do we need this nRF52_MBED_Slow_PWM library](#why-do-we-need-this-nRF52_MBED_Slow_PWM-library)
  * [Features](#features)
  * [Why using ISR-based PWM is better](#why-using-isr-based-pwm-is-better)
  * [Currently supported Boards](#currently-supported-boards)
  * [Important Notes about ISR](#important-notes-about-isr)
* [Changelog](changelog.md)
* [Prerequisites](#prerequisites)
* [Installation](#installation)
  * [Use Arduino Library Manager](#use-arduino-library-manager)
  * [Manual Install](#manual-install)
  * [VS Code & PlatformIO](#vs-code--platformio)
* [HOWTO Fix `Multiple Definitions` Linker Error](#howto-fix-multiple-definitions-linker-error)
* [Usage](#usage)
  * [1. Init Hardware Timer](#1-init-hardware-timer)
  * [2. Set PWM Frequency, dutycycle, attach irqCallbackStartFunc and irqCallbackStopFunc functions](#2-Set-PWM-Frequency-dutycycle-attach-irqCallbackStartFunc-and-irqCallbackStopFunc-functions)
* [Examples](#examples)
  * [ 1. ISR_16_PWMs_Array](examples/ISR_16_PWMs_Array)
  * [ 2. ISR_16_PWMs_Array_Complex](examples/ISR_16_PWMs_Array_Complex)
  * [ 3. ISR_16_PWMs_Array_Simple](examples/ISR_16_PWMs_Array_Simple)
  * [ 4. ISR_Changing_PWM](examples/ISR_Changing_PWM)
  * [ 5. ISR_Modify_PWM](examples/ISR_Modify_PWM)
  * [ 6. multiFileProject](examples/multiFileProject). **New**
* [Example ISR_16_PWMs_Array_Complex](#Example-ISR_16_PWMs_Array_Complex)
* [Debug Terminal Output Samples](#debug-terminal-output-samples)
  * [1. ISR_16_PWMs_Array_Complex on Nano 33 BLE](#1-ISR_16_PWMs_Array_Complex-on-Nano-33-BLE)
  * [2. ISR_16_PWMs_Array on Nano 33 BLE](#2-ISR_16_PWMs_Array-on-Nano-33-BLE)
  * [3. ISR_16_PWMs_Array_Simple on Nano 33 BLE](#3-ISR_16_PWMs_Array_Simple-on-Nano-33-BLE)
  * [4. ISR_Modify_PWM on Nano 33 BLE](#4-ISR_Modify_PWM-on-Nano-33-BLE)
  * [5. ISR_Changing_PWM on Nano 33 BLE](#5-ISR_Changing_PWM-on-Nano-33-BLE)
* [Debug](#debug)
* [Troubleshooting](#troubleshooting)
* [Issues](#issues)
* [TO DO](#to-do)
* [DONE](#done)
* [Contributions and Thanks](#contributions-and-thanks)
* [Contributing](#contributing)
* [License](#license)
* [Copyright](#copyright)

---
---

### Important Change from v1.2.0

Please have a look at [HOWTO Fix `Multiple Definitions` Linker Error](#howto-fix-multiple-definitions-linker-error)

As more complex calculation and check **inside ISR** are introduced from v1.2.0, there is possibly some crash depending on use-case.

You can modify to use larger `HW_TIMER_INTERVAL_US`, (from current 10uS), according to your board and use-case if crash happens.


```
// Don't change these numbers to make higher Timer freq. System can hang
#define HW_TIMER_INTERVAL_US      10L
```

---
---

### Why do we need this [nRF52_MBED_Slow_PWM library](https://github.com/khoih-prog/nRF52_MBED_Slow_PWM)

### Features

This library enables you to use ISR-based PWM channels on an nRF52-based board using Arduino-mbed mbed_nano core such as Nano-33-BLE to create and output PWM any GPIO pin. Because this library doesn't use the powerful hardware-controlled PWM with limitations, the maximum PWM frequency is currently limited at **1000Hz**, which is still suitable for many real-life applications. Now you can also modify PWM settings on-the-fly.

---

This library enables you to use Interrupt from Hardware Timers on nRF52_MBED-based boards to create and output PWM to pins. It now supports 16 ISR-based synchronized PWM channels, while consuming only 1 Hardware Timer. PWM interval can be very long (uint32_t millisecs). The most important feature is they're ISR-based PWM channels. Therefore, their executions are not blocked by bad-behaving functions or tasks. This important feature is absolutely necessary for mission-critical tasks. These hardware PWM channels, using interrupt, still work even if other functions are blocking. Moreover, they are much more precise (certainly depending on clock frequency accuracy) than other software PWM using millis() or micros(). That's necessary if you need to measure some data requiring better accuracy.

As **Hardware Timers are rare, and very precious assets** of any board, this library now enables you to use up to **16 ISR-based synchronized PWM channels, while consuming only 1 Hardware Timer**. Timers' interval is very long (**ulong millisecs**).

Now with these new **16 ISR-based PWM-channels**, the maximum interval is **practically unlimited** (limited only by unsigned long miliseconds) while **the accuracy is nearly perfect** compared to software PWM channels. 

The most important feature is they're ISR-based PWM channels. Therefore, their executions are **not blocked by bad-behaving functions / tasks**. This important feature is absolutely necessary for mission-critical tasks. 

The [**ISR_16_PWMs_Array_Complex**](examples/ISR_16_PWMs_Array_Complex) example will demonstrate the nearly perfect accuracy, compared to software PWM, by printing the actual period / duty-cycle in `microsecs` of each of PWM-channels.

Being ISR-based PWM, their executions are not blocked by bad-behaving functions / tasks, such as connecting to WiFi, Internet or Blynk services. You can also have many `(up to 16)` PWM channels to use.

This non-being-blocked important feature is absolutely necessary for mission-critical tasks.

You'll see `software-based` SimpleTimer is blocked while system is connecting to WiFi / Internet / Blynk, as well as by blocking task 
in loop(), using delay() function as an example. The elapsed time then is very unaccurate

---

#### Why using ISR-based PWM is better

Imagine you have a system with a **mission-critical** function, measuring water level and control the sump pump or doing something much more important. You normally use a software timer to poll, or even place the function in loop(). But what if another function is **blocking** the loop() or setup().

So your function **might not be executed, and the result would be disastrous.**

You'd prefer to have your function called, no matter what happening with other functions (busy loop, bug, etc.).

The correct choice is to use a Hardware Timer with **Interrupt** to call your function.

These hardware PWM channels, using interrupt, still work even if other functions are blocking. Moreover, they are much more **precise** (certainly depending on clock frequency accuracy) than other software PWM channels using millis() or micros(). That's necessary if you need to measure some data requiring better accuracy.

Functions using normal software PWM channels, relying on loop() and calling millis(), won't work if the loop() or setup() is blocked by certain operation. For example, certain function is blocking while it's connecting to WiFi or some services.

The catch is **your function is now part of an ISR (Interrupt Service Routine), and must be lean / mean, and follow certain rules.** More to read on:

[**HOWTO Attach Interrupt**](https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/)

---

### Currently supported Boards

1. **nRF52_MBED-based boards** such as Nano-33-BLE or Nano-33-BLE_Sense, etc., using [`Arduino mbed_nano core 2.5.2+`](https://github.com/arduino/ArduinoCore-mbed) or [`Arduino mbed core v1.3.2-`](https://github.com/arduino/ArduinoCore-mbed/releases/tag/1.3.2)

---

#### Important Notes about ISR

1. Inside the attached function, **delay() won’t work and the value returned by millis() will not increment.** Serial data received while in the function may be lost. You should declare as **volatile any variables that you modify within the attached function.**

2. Typically global variables are used to pass data between an ISR and the main program. To make sure variables shared between an ISR and the main program are updated correctly, declare them as volatile.

---
---

## Prerequisites

 1. [`Arduino IDE 1.8.19+` for Arduino](https://github.com/arduino/Arduino). [![GitHub release](https://img.shields.io/github/release/arduino/Arduino.svg)](https://github.com/arduino/Arduino/releases/latest)
 2. [`Arduino mbed_nano core 2.5.2+`](https://github.com/arduino/ArduinoCore-mbed) for NRF52-based board using mbed-RTOS such as Nano-33-BLE if you don't use `NRF_TIMER_1`. [![GitHub release](https://img.shields.io/github/release/arduino/ArduinoCore-mbed.svg)](https://github.com/arduino/ArduinoCore-mbed/releases/latest)
 3. [`Arduino mbed core v1.3.2-`](https://github.com/arduino/ArduinoCore-mbed/releases/tag/1.3.2) for NRF52-based board using mbed-RTOS such as Nano-33-BLE if you'd like to use `NRF_TIMER_1`. [![GitHub release](https://img.shields.io/github/release/arduino/ArduinoCore-mbed.svg)](https://github.com/arduino/ArduinoCore-mbed/releases/latest)
 
 4. To use with certain example
   - [`SimpleTimer library`](https://github.com/jfturcot/SimpleTimer) for [ISR_16_PWMs_Array_Complex example](examples/ISR_16_PWMs_Array_Complex).
---
---

## Installation

### Use Arduino Library Manager

The best and easiest way is to use `Arduino Library Manager`. Search for [**nRF52_MBED_Slow_PWM**](https://github.com/khoih-prog/nRF52_MBED_Slow_PWM), then select / install the latest version.
You can also use this link [![arduino-library-badge](https://www.ardu-badge.com/badge/nRF52_MBED_Slow_PWM.svg?)](https://www.ardu-badge.com/nRF52_MBED_Slow_PWM) for more detailed instructions.

### Manual Install

Another way to install is to:

1. Navigate to [**nRF52_MBED_Slow_PWM**](https://github.com/khoih-prog/nRF52_MBED_Slow_PWM) page.
2. Download the latest release `nRF52_MBED_Slow_PWM-master.zip`.
3. Extract the zip file to `nRF52_MBED_Slow_PWM-master` directory 
4. Copy whole `nRF52_MBED_Slow_PWM-master` folder to Arduino libraries' directory such as `~/Arduino/libraries/`.

### VS Code & PlatformIO

1. Install [VS Code](https://code.visualstudio.com/)
2. Install [PlatformIO](https://platformio.org/platformio-ide)
3. Install [**nRF52_MBED_Slow_PWM** library](https://platformio.org/lib/show/12877/nRF52_MBED_Slow_PWM) by using [Library Manager](https://platformio.org/lib/show/12877/nRF52_MBED_Slow_PWM/installation). Search for **nRF52_MBED_Slow_PWM** in [Platform.io Author's Libraries](https://platformio.org/lib/search?query=author:%22Khoi%20Hoang%22)
4. Use included [platformio.ini](platformio/platformio.ini) file from examples to ensure that all dependent libraries will installed automatically. Please visit documentation for the other options and examples at [Project Configuration File](https://docs.platformio.org/page/projectconf.html)


---
---

### HOWTO Fix `Multiple Definitions` Linker Error

The current library implementation, using `xyz-Impl.h` instead of standard `xyz.cpp`, possibly creates certain `Multiple Definitions` Linker error in certain use cases.

You can include this `.hpp` file

```
// Can be included as many times as necessary, without `Multiple Definitions` Linker Error
#include "nRF52_MBED_Slow_PWM.hpp"    //https://github.com/khoih-prog/nRF52_MBED_Slow_PWM
```

in many files. But be sure to use the following `.h` file **in just 1 `.h`, `.cpp` or `.ino` file**, which must **not be included in any other file**, to avoid `Multiple Definitions` Linker Error

```
// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include "nRF52_MBED_Slow_PWM.h"      //https://github.com/khoih-prog/nRF52_MBED_Slow_PWM
```

Check the new [**multiFileProject** example](examples/multiFileProject) for a `HOWTO` demo.

Have a look at the discussion in [Different behaviour using the src_cpp or src_h lib #80](https://github.com/khoih-prog/ESPAsync_WiFiManager/discussions/80)

---
---


## Usage

For [`Arduino mbed_nano core 2.5.2+`](https://github.com/arduino/ArduinoCore-mbed), you can only select NRF52 Hardware Timer NRF_TIMER_3-NRF_TIMER_4 (3 to 4). If you select the already-used NRF_TIMER_0-2, it'll be auto modified to use NRF_TIMER_3

For [`Arduino mbed core v1.3.2-`](https://github.com/arduino/ArduinoCore-mbed/releases/tag/1.3.2), you can only select NRF52 Hardware Timer NRF_TIMER_1, NRF_TIMER_3-NRF_TIMER_4 (1, 3 and 4). But for the sake of compatibility, if you select the NRF_TIMER_0-2, it'll be auto modified to use NRF_TIMER_3

Before using any Timer, you have to make sure the Timer has not been used by any other purpose.


#### 1. Init Hardware Timer

```
// Init NRF52 timer NRF_TIMER3
NRF52_MBED_Timer ITimer(NRF_TIMER_3);

// Init nRF52_Slow_PWM, each can service 16 different ISR-based PWM channels
NRF52_MBED_Slow_PWM ISR_PWM;
```

#### 2. Set PWM Frequency, dutycycle, attach irqCallbackStartFunc and irqCallbackStopFunc functions

```
void irqCallbackStartFunc()
{

}

void irqCallbackStopFunc()
{

}

void setup()
{
  ....
  
  // You can use this with PWM_Freq in Hz
  ISR_PWM.setPWM(PWM_Pin, PWM_Freq, PWM_DutyCycle, irqCallbackStartFunc, irqCallbackStopFunc);
                   
  ....                 
}  
```

---
---

### Examples: 

 1. [ISR_16_PWMs_Array](examples/ISR_16_PWMs_Array)
 2. [ISR_16_PWMs_Array_Complex](examples/ISR_16_PWMs_Array_Complex)
 3. [ISR_16_PWMs_Array_Simple](examples/ISR_16_PWMs_Array_Simple)
 4. [ISR_Changing_PWM](examples/ISR_Changing_PWM)
 5. [ISR_Modify_PWM](examples/ISR_Modify_PWM)
 6. [**multiFileProject**](examples/multiFileProject) **New**

 
---
---

### Example [ISR_16_PWMs_Array_Complex](examples/ISR_16_PWMs_Array_Complex)

```
#if !( ARDUINO_ARCH_NRF52840 && TARGET_NAME == ARDUINO_NANO33BLE )
  #error This code is designed to run on nRF52-based Nano-33-BLE boards using mbed-RTOS platform! Please check your Tools->Board setting.
#endif

// These define's must be placed at the beginning before #include "ESP32_PWM.h"
// _PWM_LOGLEVEL_ from 0 to 4
// Don't define _PWM_LOGLEVEL_ > 0. Only for special ISR debugging only. Can hang the system.
#define _PWM_LOGLEVEL_      4

#define USING_MICROS_RESOLUTION       true    //false 

// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include "nRF52_MBED_Slow_PWM.h"

#include <SimpleTimer.h>              // https://github.com/jfturcot/SimpleTimer

#define LED_OFF             HIGH
#define LED_ON              LOW

#ifndef LED_BUILTIN
  #define LED_BUILTIN       25
#endif

#ifndef LED_BLUE
  #define LED_BLUE          10
#endif

#ifndef LED_RED
  #define LED_RED           11
#endif

#define HW_TIMER_INTERVAL_US      10L

volatile uint64_t startMicros = 0;

// For mbed nRF52, you can only select NRF52 Hardware Timer NRF_TIMER_3-NRF_TIMER_4 (3 to 4)
// If you select the already-used NRF_TIMER_0-2, it'll be auto modified to use NRF_TIMER_3

// Init NRF52 timer NRF_TIMER3
NRF52_MBED_Timer ITimer(NRF_TIMER_3);

// Init nRF52_Slow_PWM, each can service 16 different ISR-based PWM channels
NRF52_MBED_Slow_PWM ISR_PWM;

//////////////////////////////////////////////////////

void TimerHandler()
{ 
  ISR_PWM.run();
}

/////////////////////////////////////////////////

#define NUMBER_ISR_PWMS         16

#define PIN_D0      0
#define PIN_D1      1
#define PIN_D2      2
#define PIN_D3      3
#define PIN_D4      4
#define PIN_D5      5
#define PIN_D6      6
#define PIN_D7      7
#define PIN_D8      8
#define PIN_D9      9
#define PIN_D10     10
#define PIN_D11     11
#define PIN_D12     12

typedef void (*irqCallback)  ();

//////////////////////////////////////////////////////

#define USE_COMPLEX_STRUCT      true

#define USING_PWM_FREQUENCY     true

//////////////////////////////////////////////////////

#if USE_COMPLEX_STRUCT

typedef struct
{
  uint32_t      PWM_Pin;
  irqCallback   irqCallbackStartFunc;
  irqCallback   irqCallbackStopFunc;

#if USING_PWM_FREQUENCY  
  float         PWM_Freq;
#else  
  uint32_t      PWM_Period;
#endif
  
  float         PWM_DutyCycle;
  
  uint64_t      deltaMicrosStart;
  uint64_t      previousMicrosStart;

  uint64_t      deltaMicrosStop;
  uint64_t      previousMicrosStop;
  
} ISR_PWM_Data;

// In nRF52, avoid doing something fancy in ISR, for example Serial.print()
// The pure simple Serial.prints here are just for demonstration and testing. Must be eliminate in working environment
// Or you can get this run-time error / crash

void doingSomethingStart(int index);

void doingSomethingStop(int index);

#else   // #if USE_COMPLEX_STRUCT

volatile unsigned long deltaMicrosStart    [] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
volatile unsigned long previousMicrosStart [] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

volatile unsigned long deltaMicrosStop     [] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
volatile unsigned long previousMicrosStop  [] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };

// You can assign pins here. Be carefull to select good pin to use or crash, e.g pin 6-11
uint32_t PWM_Pin[] =
{
   LED_BUILTIN, LED_BLUE, LED_RED, PIN_D0, PIN_D1,  PIN_D2,  PIN_D3,  PIN_D4,
        PIN_D5,   PIN_D6,  PIN_D7, PIN_D8, PIN_D9, PIN_D10, PIN_D11, PIN_D12
};

// You can assign any interval for any timer here, in microseconds
uint32_t PWM_Period[] =
{
  1000000L,   500000L,   333333L,   250000L,   200000L,   166667L,   142857L,   125000L,
   111111L,   100000L,    66667L,    50000L,    40000L,   33333L,     25000L,    20000L
};

// You can assign any interval for any timer here, in Hz
float PWM_Freq[] =
{
  1.0f,  2.0f,  3.0f,  4.0f,  5.0f,  6.0f,  7.0f,  8.0f,
  9.0f, 10.0f, 15.0f, 20.0f, 25.0f, 30.0f, 40.0f, 50.0f
};

// You can assign any interval for any timer here, in milliseconds
float PWM_DutyCycle[] =
{
   5.0, 10.0, 20.0, 30.0, 40.0, 45.0, 50.0, 55.0,
  60.0, 65.0, 70.0, 75.0, 80.0, 85.0, 90.0, 95.0
};


void doingSomethingStart(int index)
{
  unsigned long currentMicros  = micros();

  deltaMicrosStart[index]    = currentMicros - previousMicrosStart[index];
  previousMicrosStart[index] = currentMicros;
}

void doingSomethingStop(int index)
{
  unsigned long currentMicros  = micros();

  // Count from start to stop PWM pulse
  deltaMicrosStop[index]    = currentMicros - previousMicrosStart[index];
  previousMicrosStop[index] = currentMicros;
}

#endif    // #if USE_COMPLEX_STRUCT

////////////////////////////////////
// Shared
////////////////////////////////////

void doingSomethingStart0()
{
  doingSomethingStart(0);
}

void doingSomethingStart1()
{
  doingSomethingStart(1);
}

void doingSomethingStart2()
{
  doingSomethingStart(2);
}

void doingSomethingStart3()
{
  doingSomethingStart(3);
}

void doingSomethingStart4()
{
  doingSomethingStart(4);
}

void doingSomethingStart5()
{
  doingSomethingStart(5);
}

void doingSomethingStart6()
{
  doingSomethingStart(6);
}

void doingSomethingStart7()
{
  doingSomethingStart(7);
}

void doingSomethingStart8()
{
  doingSomethingStart(8);
}

void doingSomethingStart9()
{
  doingSomethingStart(9);
}

void doingSomethingStart10()
{
  doingSomethingStart(10);
}

void doingSomethingStart11()
{
  doingSomethingStart(11);
}

void doingSomethingStart12()
{
  doingSomethingStart(12);
}

void doingSomethingStart13()
{
  doingSomethingStart(13);
}

void doingSomethingStart14()
{
  doingSomethingStart(14);
}

void doingSomethingStart15()
{
  doingSomethingStart(15);
}

//////////////////////////////////////////////////////

void doingSomethingStop0()
{
  doingSomethingStop(0);
}

void doingSomethingStop1()
{
  doingSomethingStop(1);
}

void doingSomethingStop2()
{
  doingSomethingStop(2);
}

void doingSomethingStop3()
{
  doingSomethingStop(3);
}

void doingSomethingStop4()
{
  doingSomethingStop(4);
}

void doingSomethingStop5()
{
  doingSomethingStop(5);
}

void doingSomethingStop6()
{
  doingSomethingStop(6);
}

void doingSomethingStop7()
{
  doingSomethingStop(7);
}

void doingSomethingStop8()
{
  doingSomethingStop(8);
}

void doingSomethingStop9()
{
  doingSomethingStop(9);
}

void doingSomethingStop10()
{
  doingSomethingStop(10);
}

void doingSomethingStop11()
{
  doingSomethingStop(11);
}

void doingSomethingStop12()
{
  doingSomethingStop(12);
}

void doingSomethingStop13()
{
  doingSomethingStop(13);
}

void doingSomethingStop14()
{
  doingSomethingStop(14);
}

void doingSomethingStop15()
{
  doingSomethingStop(15);
}

//////////////////////////////////////////////////////

#if USE_COMPLEX_STRUCT

  #if USING_PWM_FREQUENCY
  
  ISR_PWM_Data curISR_PWM_Data[] =
  {
    // pin, irqCallbackStartFunc, irqCallbackStopFunc, PWM_Freq, PWM_DutyCycle, deltaMicrosStart, previousMicrosStart, deltaMicrosStop, previousMicrosStop
    { LED_BUILTIN,  doingSomethingStart0,    doingSomethingStop0,    1,   5, 0, 0, 0, 0 },
    { LED_BLUE,     doingSomethingStart1,    doingSomethingStop1,    2,  10, 0, 0, 0, 0 },
    { LED_RED,      doingSomethingStart2,    doingSomethingStop2,    3,  20, 0, 0, 0, 0 },
    { PIN_D0,       doingSomethingStart3,    doingSomethingStop3,    4,  30, 0, 0, 0, 0 },
    { PIN_D1,       doingSomethingStart4,    doingSomethingStop4,    5,  40, 0, 0, 0, 0 },
    { PIN_D2,       doingSomethingStart5,    doingSomethingStop5,    6,  45, 0, 0, 0, 0 },
    { PIN_D3,       doingSomethingStart6,    doingSomethingStop6,    7,  50, 0, 0, 0, 0 },
    { PIN_D4,       doingSomethingStart7,    doingSomethingStop7,    8,  55, 0, 0, 0, 0 },
    { PIN_D5,       doingSomethingStart8,    doingSomethingStop8,    9,  60, 0, 0, 0, 0 },
    { PIN_D6,       doingSomethingStart9,    doingSomethingStop9,   10,  65, 0, 0, 0, 0 },
    { PIN_D7,       doingSomethingStart10,   doingSomethingStop10,  15,  70, 0, 0, 0, 0 },
    { PIN_D8,       doingSomethingStart11,   doingSomethingStop11,  20,  75, 0, 0, 0, 0 },
    { PIN_D9,       doingSomethingStart12,   doingSomethingStop12,  25,  80, 0, 0, 0, 0 },
    { PIN_D10,      doingSomethingStart13,   doingSomethingStop13,  30,  85, 0, 0, 0, 0 },
    { PIN_D11,      doingSomethingStart14,   doingSomethingStop14,  40,  90, 0, 0, 0, 0 },
    { PIN_D12,      doingSomethingStart15,   doingSomethingStop15,  50,  95, 0, 0, 0, 0 }
  };
  
  #else   // #if USING_PWM_FREQUENCY
  
  ISR_PWM_Data curISR_PWM_Data[] =
  {
    // pin, irqCallbackStartFunc, irqCallbackStopFunc, PWM_Period, PWM_DutyCycle, deltaMicrosStart, previousMicrosStart, deltaMicrosStop, previousMicrosStop
    { LED_BUILTIN,  doingSomethingStart0,     doingSomethingStop0,   1000000L,  5, 0, 0, 0, 0 },
    { LED_BLUE,     doingSomethingStart1,     doingSomethingStop1,    500000L, 10, 0, 0, 0, 0 },
    { LED_RED,      doingSomethingStart2,     doingSomethingStop2,    333333L, 20, 0, 0, 0, 0 },
    { PIN_D0,       doingSomethingStart3,     doingSomethingStop3,    250000L, 30, 0, 0, 0, 0 },
    { PIN_D1,       doingSomethingStart4,     doingSomethingStop4,    200000L, 40, 0, 0, 0, 0 },
    { PIN_D2,       doingSomethingStart5,     doingSomethingStop5,    166667L, 45, 0, 0, 0, 0 },
    { PIN_D3,       doingSomethingStart6,     doingSomethingStop6,    142857L, 50, 0, 0, 0, 0 },
    { PIN_D4,       doingSomethingStart7,     doingSomethingStop7,    125000L, 55, 0, 0, 0, 0 },
    { PIN_D5,       doingSomethingStart8,     doingSomethingStop8,    111111L, 60, 0, 0, 0, 0 },
    { PIN_D6,       doingSomethingStart9,     doingSomethingStop9,    100000L, 65, 0, 0, 0, 0 },
    { PIN_D7,       doingSomethingStart10,    doingSomethingStop10,    66667L, 70, 0, 0, 0, 0 },
    { PIN_D8,       doingSomethingStart11,    doingSomethingStop11,    50000L, 75, 0, 0, 0, 0 },
    { PIN_D9,       doingSomethingStart12,    doingSomethingStop12,    40000L, 80, 0, 0, 0, 0 },
    { PIN_D10,      doingSomethingStart13,    doingSomethingStop13,    33333L, 85, 0, 0, 0, 0 },
    { PIN_D11,      doingSomethingStart14,    doingSomethingStop14,    25000L, 90, 0, 0, 0, 0 },
    { PIN_D12,      doingSomethingStart15,    doingSomethingStop15,    20000L, 95, 0, 0, 0, 0 }
  };
  
  #endif  // #if USING_PWM_FREQUENCY

void doingSomethingStart(int index)
{
  unsigned long currentMicros  = micros();

  curISR_PWM_Data[index].deltaMicrosStart    = currentMicros - curISR_PWM_Data[index].previousMicrosStart;
  curISR_PWM_Data[index].previousMicrosStart = currentMicros;
}

void doingSomethingStop(int index)
{
  unsigned long currentMicros  = micros();

  //curISR_PWM_Data[index].deltaMicrosStop     = currentMicros - curISR_PWM_Data[index].previousMicrosStop;
  // Count from start to stop PWM pulse
  curISR_PWM_Data[index].deltaMicrosStop     = currentMicros - curISR_PWM_Data[index].previousMicrosStart;
  curISR_PWM_Data[index].previousMicrosStop  = currentMicros;
}

#else   // #if USE_COMPLEX_STRUCT

irqCallback irqCallbackStartFunc[] =
{
  doingSomethingStart0,  doingSomethingStart1,  doingSomethingStart2,  doingSomethingStart3,
  doingSomethingStart4,  doingSomethingStart5,  doingSomethingStart6,  doingSomethingStart7,
  doingSomethingStart8,  doingSomethingStart9,  doingSomethingStart10, doingSomethingStart11,
  doingSomethingStart12, doingSomethingStart13, doingSomethingStart14, doingSomethingStart15
};

irqCallback irqCallbackStopFunc[] =
{
  doingSomethingStop0,  doingSomethingStop1,  doingSomethingStop2,  doingSomethingStop3,
  doingSomethingStop4,  doingSomethingStop5,  doingSomethingStop6,  doingSomethingStop7,
  doingSomethingStop8,  doingSomethingStop9,  doingSomethingStop10, doingSomethingStop11,
  doingSomethingStop12, doingSomethingStop13, doingSomethingStop14, doingSomethingStop15
};

#endif    // #if USE_COMPLEX_STRUCT

//////////////////////////////////////////////////////

#define SIMPLE_TIMER_MS        2000L

// Init SimpleTimer
SimpleTimer simpleTimer;

// Here is software Timer, you can do somewhat fancy stuffs without many issues.
// But always avoid
// 1. Long delay() it just doing nothing and pain-without-gain wasting CPU power.Plan and design your code / strategy ahead
// 2. Very long "do", "while", "for" loops without predetermined exit time.
void simpleTimerDoingSomething2s()
{
  static unsigned long previousMicrosStart = startMicros;

  unsigned long currMicros = micros();

  Serial.print(F("SimpleTimer (ms): ")); Serial.print(SIMPLE_TIMER_MS);
  Serial.print(F(", us : ")); Serial.print(currMicros);
  Serial.print(F(", Dus : ")); Serial.println(currMicros - previousMicrosStart);

  for (uint16_t i = 0; i < NUMBER_ISR_PWMS; i++)
  {
#if USE_COMPLEX_STRUCT
    Serial.print(F("PWM Channel : ")); Serial.print(i);
    Serial.print(F(", programmed Period (us): "));

  #if USING_PWM_FREQUENCY
    Serial.print(1000000 / curISR_PWM_Data[i].PWM_Freq);
  #else
    Serial.print(curISR_PWM_Data[i].PWM_Period);
  #endif
    
    Serial.print(F(", actual : ")); Serial.print(curISR_PWM_Data[i].deltaMicrosStart);
    
    Serial.print(F(", programmed DutyCycle : ")); 

    Serial.print(curISR_PWM_Data[i].PWM_DutyCycle);
    
    Serial.print(F(", actual : ")); Serial.println((float) curISR_PWM_Data[i].deltaMicrosStop * 100.0f / curISR_PWM_Data[i].deltaMicrosStart);
    
#else

    Serial.print(F("PWM Channel : ")); Serial.print(i);
    
  #if USING_PWM_FREQUENCY
    Serial.print(1000000 / PWM_Freq[i]);
  #else
    Serial.print(PWM_Period[i]);
  #endif
  
    Serial.print(F(", programmed Period (us): ")); Serial.print(PWM_Period[i]);
    Serial.print(F(", actual : ")); Serial.print(deltaMicrosStart[i]);

    Serial.print(F(", programmed DutyCycle : ")); 
  
    Serial.print(PWM_DutyCycle[i]);
      
    Serial.print(F(", actual : ")); Serial.println( (float) deltaMicrosStop[i] * 100.0f / deltaMicrosStart[i]);
#endif
  }

  previousMicrosStart = currMicros;
}

void setup()
{
  Serial.begin(115200);
  while (!Serial);

  delay(2000);

  Serial.print(F("\nStarting ISR_16_PWMs_Array_Complex on ")); Serial.println(BOARD_NAME);
  Serial.println(NRF52_MBED_SLOW_PWM_VERSION);

  // Interval in microsecs
  if (ITimer.attachInterruptInterval(HW_TIMER_INTERVAL_US, TimerHandler))
  {
    startMicros = micros();
    Serial.print(F("Starting ITimer OK, micros() = ")); Serial.println(startMicros);
  }
  else
    Serial.println(F("Can't set ITimer. Select another freq. or timer"));

  startMicros = micros();

  // Just to demonstrate, don't use too many ISR Timers if not absolutely necessary
  // You can use up to 16 timer for each ISR_PWM
  
  for (uint16_t i = 0; i < NUMBER_ISR_PWMS; i++)
  {
#if USE_COMPLEX_STRUCT
    curISR_PWM_Data[i].previousMicrosStart = startMicros;
    //ISR_PWM.setInterval(curISR_PWM_Data[i].PWM_Period, curISR_PWM_Data[i].irqCallbackStartFunc);

    //void setPWM(uint32_t pin, float frequency, float dutycycle
    // , timer_callback_p StartCallback = nullptr, timer_callback_p StopCallback = nullptr)

  #if USING_PWM_FREQUENCY
    // You can use this with PWM_Freq in Hz
    ISR_PWM.setPWM(curISR_PWM_Data[i].PWM_Pin, curISR_PWM_Data[i].PWM_Freq, curISR_PWM_Data[i].PWM_DutyCycle, 
                   curISR_PWM_Data[i].irqCallbackStartFunc, curISR_PWM_Data[i].irqCallbackStopFunc);
  #else
    // Or You can use this with PWM_Period in us
    ISR_PWM.setPWM_Period(curISR_PWM_Data[i].PWM_Pin, curISR_PWM_Data[i].PWM_Period, curISR_PWM_Data[i].PWM_DutyCycle, 
                          curISR_PWM_Data[i].irqCallbackStartFunc, curISR_PWM_Data[i].irqCallbackStopFunc);
  #endif
  
#else
    previousMicrosStart[i] = micros();
    
  #if USING_PWM_FREQUENCY
    // You can use this with PWM_Freq in Hz
    ISR_PWM.setPWM(PWM_Pin[i], PWM_Freq[i], PWM_DutyCycle[i], irqCallbackStartFunc[i], irqCallbackStopFunc[i]);
  #else
    // Or You can use this with PWM_Period in us
    ISR_PWM.setPWM_Period(PWM_Pin[i], PWM_Period[i], PWM_DutyCycle[i], irqCallbackStartFunc[i], irqCallbackStopFunc[i]);
  #endif 
   
#endif
  }

  // You need this timer for non-critical tasks. Avoid abusing ISR if not absolutely necessary.
  simpleTimer.setInterval(SIMPLE_TIMER_MS, simpleTimerDoingSomething2s);
}

#define BLOCKING_TIME_MS      10000L

void loop()
{
  // This unadvised blocking task is used to demonstrate the blocking effects onto the execution and accuracy to Software timer
  // You see the time elapse of ISR_PWM still accurate, whereas very unaccurate for Software Timer
  // The time elapse for 2000ms software timer now becomes 3000ms (BLOCKING_TIME_MS)
  // While that of ISR_PWM is still prefect.
  delay(BLOCKING_TIME_MS);

  // You need this Software timer for non-critical tasks. Avoid abusing ISR if not absolutely necessary
  // You don't need to and never call ISR_PWM.run() here in the loop(). It's already handled by ISR timer.
  simpleTimer.run();
}
```
---
---

### Debug Terminal Output Samples

### 1. ISR_16_PWMs_Array_Complex on Nano 33 BLE

The following is the sample terminal output when running example [ISR_16_PWMs_Array_Complex](examples/ISR_16_PWMs_Array_Complex) to demonstrate how to use multiple PWM channels with complex callback functions, the accuracy of ISR Hardware PWM-channels, **especially when system is very busy**.  The ISR PWM-channels is **running exactly according to corresponding programmed periods and duty-cycles**


```
Starting ISR_16_PWMs_Array_Complex on Nano 33 BLE
NRF52_MBED_Slow_PWM v1.2.0
[PWM] Timer =  NRF_TIMER3 , Timer Clock (Hz) =  16000000.00
[PWM] Frequency =  100000.00 , _count =  160
Starting ITimer OK, micros() = 2800208
Channel : 0	    Period : 1000000		OnTime : 50000	Start_Time : 2802391
Channel : 1	    Period : 500000		OnTime : 50000	Start_Time : 2805669
Channel : 2	    Period : 333333		OnTime : 66666	Start_Time : 2809019
Channel : 3	    Period : 250000		OnTime : 75000	Start_Time : 2812245
Channel : 4	    Period : 200000		OnTime : 80000	Start_Time : 2815525
Channel : 5	    Period : 166666		OnTime : 74999	Start_Time : 2818972
Channel : 6	    Period : 142857		OnTime : 71428	Start_Time : 2822317
Channel : 7	    Period : 125000		OnTime : 68750	Start_Time : 2825720
Channel : 8	    Period : 111111		OnTime : 66666	Start_Time : 2829165
Channel : 9	    Period : 100000		OnTime : 65000	Start_Time : 2832514
Channel : 10	    Period : 66666		OnTime : 46666	Start_Time : 2835951
Channel : 11	    Period : 50000		OnTime : 37500	Start_Time : 2839484
Channel : 12	    Period : 40000		OnTime : 32000	Start_Time : 2843002
Channel : 13	    Period : 33333		OnTime : 28333	Start_Time : 2846504
Channel : 14	    Period : 25000		OnTime : 22500	Start_Time : 2850118
Channel : 15	    Period : 20000		OnTime : 19000	Start_Time : 2853663
SimpleTimer (ms): 2000, us : 12855209, Dus : 10053055
PWM Channel : 0, programmed Period (us): 1000000.00, actual : 1000016, programmed DutyCycle : 5.00, actual : 5.00
PWM Channel : 1, programmed Period (us): 500000.00, actual : 500034, programmed DutyCycle : 10.00, actual : 10.00
PWM Channel : 2, programmed Period (us): 333333.34, actual : 333344, programmed DutyCycle : 20.00, actual : 20.00
PWM Channel : 3, programmed Period (us): 250000.00, actual : 250023, programmed DutyCycle : 30.00, actual : 29.99
PWM Channel : 4, programmed Period (us): 200000.00, actual : 200012, programmed DutyCycle : 40.00, actual : 40.00
PWM Channel : 5, programmed Period (us): 166666.67, actual : 166689, programmed DutyCycle : 45.00, actual : 44.98
PWM Channel : 6, programmed Period (us): 142857.14, actual : 142882, programmed DutyCycle : 50.00, actual : 49.98
PWM Channel : 7, programmed Period (us): 125000.00, actual : 125016, programmed DutyCycle : 55.00, actual : 54.97
PWM Channel : 8, programmed Period (us): 111111.11, actual : 111133, programmed DutyCycle : 60.00, actual : 59.97
PWM Channel : 9, programmed Period (us): 100000.00, actual : 100009, programmed DutyCycle : 65.00, actual : 64.99
PWM Channel : 10, programmed Period (us): 66666.66, actual : 66672, programmed DutyCycle : 70.00, actual : 69.95
PWM Channel : 11, programmed Period (us): 50000.00, actual : 50031, programmed DutyCycle : 75.00, actual : 74.91
PWM Channel : 12, programmed Period (us): 40000.00, actual : 40005, programmed DutyCycle : 80.00, actual : 79.97
PWM Channel : 13, programmed Period (us): 33333.33, actual : 33377, programmed DutyCycle : 85.00, actual : 84.88
PWM Channel : 14, programmed Period (us): 25000.00, actual : 25001, programmed DutyCycle : 90.00, actual : 89.88
PWM Channel : 15, programmed Period (us): 20000.00, actual : 20003, programmed DutyCycle : 95.00, actual : 94.93
SimpleTimer (ms): 2000, us : 22957454, Dus : 10102245
PWM Channel : 0, programmed Period (us): 1000000.00, actual : 1000011, programmed DutyCycle : 5.00, actual : 5.00
PWM Channel : 1, programmed Period (us): 500000.00, actual : 500024, programmed DutyCycle : 10.00, actual : 9.99
PWM Channel : 2, programmed Period (us): 333333.34, actual : 333349, programmed DutyCycle : 20.00, actual : 19.99
PWM Channel : 3, programmed Period (us): 250000.00, actual : 250015, programmed DutyCycle : 30.00, actual : 29.99
PWM Channel : 4, programmed Period (us): 200000.00, actual : 200028, programmed DutyCycle : 40.00, actual : 39.98
PWM Channel : 5, programmed Period (us): 166666.67, actual : 166678, programmed DutyCycle : 45.00, actual : 44.98
PWM Channel : 6, programmed Period (us): 142857.14, actual : 142854, programmed DutyCycle : 50.00, actual : 49.98
PWM Channel : 7, programmed Period (us): 125000.00, actual : 125008, programmed DutyCycle : 55.00, actual : 54.98
PWM Channel : 8, programmed Period (us): 111111.11, actual : 111143, programmed DutyCycle : 60.00, actual : 59.95
PWM Channel : 9, programmed Period (us): 100000.00, actual : 100003, programmed DutyCycle : 65.00, actual : 64.99
PWM Channel : 10, programmed Period (us): 66666.66, actual : 66676, programmed DutyCycle : 70.00, actual : 69.95
PWM Channel : 11, programmed Period (us): 50000.00, actual : 50030, programmed DutyCycle : 75.00, actual : 74.89
PWM Channel : 12, programmed Period (us): 40000.00, actual : 40008, programmed DutyCycle : 80.00, actual : 79.93
PWM Channel : 13, programmed Period (us): 33333.33, actual : 33375, programmed DutyCycle : 85.00, actual : 84.88
PWM Channel : 14, programmed Period (us): 25000.00, actual : 25040, programmed DutyCycle : 90.00, actual : 89.82
PWM Channel : 15, programmed Period (us): 20000.00, actual : 19995, programmed DutyCycle : 95.00, actual : 94.91
SimpleTimer (ms): 2000, us : 33060835, Dus : 10103381
PWM Channel : 0, programmed Period (us): 1000000.00, actual : 1000000, programmed DutyCycle : 5.00, actual : 5.00
PWM Channel : 1, programmed Period (us): 500000.00, actual : 500029, programmed DutyCycle : 10.00, actual : 10.00
PWM Channel : 2, programmed Period (us): 333333.34, actual : 333354, programmed DutyCycle : 20.00, actual : 19.99
PWM Channel : 3, programmed Period (us): 250000.00, actual : 249993, programmed DutyCycle : 30.00, actual : 30.00
PWM Channel : 4, programmed Period (us): 200000.00, actual : 200017, programmed DutyCycle : 40.00, actual : 40.00
PWM Channel : 5, programmed Period (us): 166666.67, actual : 166676, programmed DutyCycle : 45.00, actual : 45.00
PWM Channel : 6, programmed Period (us): 142857.14, actual : 142867, programmed DutyCycle : 50.00, actual : 49.98
PWM Channel : 7, programmed Period (us): 125000.00, actual : 125008, programmed DutyCycle : 55.00, actual : 54.97
PWM Channel : 8, programmed Period (us): 111111.11, actual : 111122, programmed DutyCycle : 60.00, actual : 59.97
PWM Channel : 9, programmed Period (us): 100000.00, actual : 100020, programmed DutyCycle : 65.00, actual : 64.96
PWM Channel : 10, programmed Period (us): 66666.66, actual : 66652, programmed DutyCycle : 70.00, actual : 70.00
PWM Channel : 11, programmed Period (us): 50000.00, actual : 50024, programmed DutyCycle : 75.00, actual : 74.90
PWM Channel : 12, programmed Period (us): 40000.00, actual : 40028, programmed DutyCycle : 80.00, actual : 79.92
PWM Channel : 13, programmed Period (us): 33333.33, actual : 33343, programmed DutyCycle : 85.00, actual : 84.90
PWM Channel : 14, programmed Period (us): 25000.00, actual : 25001, programmed DutyCycle : 90.00, actual : 89.95
PWM Channel : 15, programmed Period (us): 20000.00, actual : 20061, programmed DutyCycle : 95.00, actual : 94.81
```

---

### 2. ISR_16_PWMs_Array on Nano 33 BLE

The following is the sample terminal output when running example [**ISR_16_PWMs_Array**](examples/ISR_16_PWMs_Array) on **nRF52_MBED-based Nano 33 BLE** to demonstrate how to use multiple PWM channels with simple callback functions.

```
Starting ISR_16_PWMs_Array on Nano 33 BLE
NRF52_MBED_Slow_PWM v1.2.0
[PWM] Timer =  NRF_TIMER3 , Timer Clock (Hz) =  16000000.00
[PWM] Frequency =  100000.00 , _count =  160
Starting ITimer OK, micros() = 2802084
Channel : 0	    Period : 1000000		OnTime : 50000	Start_Time : 2804151
Channel : 1	    Period : 500000		OnTime : 50000	Start_Time : 2807308
Channel : 2	    Period : 333333		OnTime : 66666	Start_Time : 2810368
Channel : 3	    Period : 250000		OnTime : 75000	Start_Time : 2813604
Channel : 4	    Period : 200000		OnTime : 80000	Start_Time : 2816788
Channel : 5	    Period : 166666		OnTime : 74999	Start_Time : 2820089
Channel : 6	    Period : 142857		OnTime : 71428	Start_Time : 2823211
Channel : 7	    Period : 125000		OnTime : 68750	Start_Time : 2826543
Channel : 8	    Period : 111111		OnTime : 66666	Start_Time : 2829799
Channel : 9	    Period : 100000		OnTime : 65000	Start_Time : 2833222
Channel : 10	    Period : 66666		OnTime : 46666	Start_Time : 2836654
Channel : 11	    Period : 50000		OnTime : 37500	Start_Time : 2839972
Channel : 12	    Period : 40000		OnTime : 32000	Start_Time : 2843511
Channel : 13	    Period : 33333		OnTime : 28333	Start_Time : 2846912
Channel : 14	    Period : 25000		OnTime : 22500	Start_Time : 2850523
Channel : 15	    Period : 20000		OnTime : 19000	Start_Time : 2853974
```

---

### 3. ISR_16_PWMs_Array_Simple on Nano 33 BLE

The following is the sample terminal output when running example [**ISR_16_PWMs_Array_Simple**](examples/ISR_16_PWMs_Array_Simple) on **nRF52_MBED-based Nano 33 BLE** to demonstrate how to use multiple PWM channels.

```
Starting ISR_16_PWMs_Array_Simple on Nano 33 BLE
NRF52_MBED_Slow_PWM v1.2.0
[PWM] Timer =  NRF_TIMER3 , Timer Clock (Hz) =  16000000.00
[PWM] Frequency =  100000.00 , _count =  160
Starting ITimer OK, micros() = 3202330
Channel : 0	    Period : 1000000		OnTime : 50000	Start_Time : 3204240
Channel : 1	    Period : 500000		OnTime : 50000	Start_Time : 3207337
Channel : 2	    Period : 333333		OnTime : 66666	Start_Time : 3210465
Channel : 3	    Period : 250000		OnTime : 75000	Start_Time : 3213666
Channel : 4	    Period : 200000		OnTime : 80000	Start_Time : 3216872
Channel : 5	    Period : 166666		OnTime : 74999	Start_Time : 3220014
Channel : 6	    Period : 142857		OnTime : 71428	Start_Time : 3223261
Channel : 7	    Period : 125000		OnTime : 68750	Start_Time : 3226495
Channel : 8	    Period : 111111		OnTime : 66666	Start_Time : 3229851
Channel : 9	    Period : 100000		OnTime : 65000	Start_Time : 3233165
Channel : 10	    Period : 66666		OnTime : 46666	Start_Time : 3236525
Channel : 11	    Period : 50000		OnTime : 37500	Start_Time : 3239882
Channel : 12	    Period : 40000		OnTime : 32000	Start_Time : 3243149
Channel : 13	    Period : 33333		OnTime : 28333	Start_Time : 3246655
Channel : 14	    Period : 25000		OnTime : 22500	Start_Time : 3250177
Channel : 15	    Period : 20000		OnTime : 19000	Start_Time : 3253635
```

---

### 4. ISR_Modify_PWM on Nano 33 BLE

The following is the sample terminal output when running example [ISR_Modify_PWM](examples/ISR_Modify_PWM) on **nRF52_MBED-based Nano 33 BLE** to demonstrate how to modify PWM settings on-the-fly without deleting the PWM channel

```
Starting ISR_Modify_PWM on Nano 33 BLE
NRF52_MBED_Slow_PWM v1.2.0
[PWM] Timer =  NRF_TIMER3 , Timer Clock (Hz) =  16000000.00
[PWM] Frequency =  50000.00 , _count =  320
Starting ITimer OK, micros() = 2703294
Using PWM Freq = 1.00, PWM DutyCycle = 50.00
Channel : 0	    Period : 1000000		OnTime : 500000	Start_Time : 2705932
Channel : 0	New Period : 500000		OnTime : 450000	Start_Time : 12706044
Channel : 0	New Period : 1000000		OnTime : 500000	Start_Time : 22706410
```

---

### 5. ISR_Changing_PWM on Nano 33 BLE

The following is the sample terminal output when running example [ISR_Changing_PWM](examples/ISR_Changing_PWM) on **nRF52_MBED-based Nano 33 BLE** to demonstrate how to modify PWM settings on-the-fly by deleting the PWM channel and reinit the PWM channel

```
Starting ISR_Changing_PWM on Nano 33 BLE
NRF52_MBED_Slow_PWM v1.2.0
[PWM] Timer =  NRF_TIMER3 , Timer Clock (Hz) =  16000000.00
[PWM] Frequency =  50000.00 , _count =  320
Starting ITimer OK, micros() = 3024808
Using PWM Freq = 1.00, PWM DutyCycle = 50.00
Channel : 0	    Period : 1000000		OnTime : 500000	Start_Time : 3027194
Using PWM Freq = 2.00, PWM DutyCycle = 90.00
Channel : 0	    Period : 500000		OnTime : 450000	Start_Time : 13035894
Using PWM Freq = 1.00, PWM DutyCycle = 50.00
Channel : 0	    Period : 1000000		OnTime : 500000	Start_Time : 23048753
```

---
---

### Debug

Debug is enabled by default on Serial.

You can also change the debugging level `_PWM_LOGLEVEL_` from 0 to 4

```cpp
// Don't define _PWM_LOGLEVEL_ > 0. Only for special ISR debugging only. Can hang the system.
#define _PWM_LOGLEVEL_     0
```

---

### Troubleshooting

If you get compilation errors, more often than not, you may need to install a newer version of the core for Arduino boards.

Sometimes, the library will only work if you update the board core to the latest version because I am using newly added functions.


---
---

### Issues

Submit issues to: [nRF52_MBED_Slow_PWM issues](https://github.com/khoih-prog/nRF52_MBED_Slow_PWM/issues)

---

## TO DO

1. Search for bug and improvement.
2. Similar features for remaining Arduino boards

---

## DONE

1. Basic hardware multi-channel PWM for **nRF52_MBED-based Nano-33-BLE or Nano-33-BLE_Sense, etc.** using [`Arduino mbed_nano core 2.5.2+`](https://github.com/arduino/ArduinoCore-mbed) or [`Arduino mbed core v1.3.2-`](https://github.com/arduino/ArduinoCore-mbed/releases/tag/1.3.2)
2. Add Table of Contents
3. Add functions to modify PWM settings on-the-fly
4. Fix `multiple-definitions` linker error
5. Optimize library code by using `reference-passing` instead of `value-passing`
6. Add support to [`MegaCoreX megaAVR core`](https://github.com/MCUdude/MegaCoreX) core, including ATmega4809, ATmega4808, ATmega3209, ATmega3208, ATmega1609, ATmega1608, ATmega809 and ATmega808
7. Improve accuracy by using `float`, instead of `uint32_t` for `dutycycle`
8. DutyCycle to be optionally updated at the end current PWM period instead of immediately.

---
---

### Contributions and Thanks

Many thanks for everyone for bug reporting, new feature suggesting, testing and contributing to the development of this library.


---

## Contributing

If you want to contribute to this project:
- Report bugs and errors
- Ask for enhancements
- Create issues and pull requests
- Tell other people about this library

---

### License

- The library is licensed under [MIT](https://github.com/khoih-prog/nRF52_MBED_Slow_PWM/blob/main/LICENSE)

---

## Copyright

Copyright 2021- Khoi Hoang


