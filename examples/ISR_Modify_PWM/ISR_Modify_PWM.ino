/****************************************************************************************************************************
  ISR_Modify_PWM.ino
  For nRF52-based boards using Arduino mbed_nano core, such as Nano_33_BLE
  Written by Khoi Hoang

  Built by Khoi Hoang https://github.com/khoih-prog/nRF52_MBED_Slow_PWM
  Licensed under MIT license

  Now even you use all these new 16 ISR-based timers,with their maximum interval practically unlimited (limited only by
  unsigned long miliseconds), you just consume only one  nRF52-based timer and avoid conflicting with other cores' tasks.
  The accuracy is nearly perfect compared to software timers. The most important feature is they're ISR-based timers
  Therefore, their executions are not blocked by bad-behaving functions / tasks.
  This important feature is absolutely necessary for mission-critical tasks.
*****************************************************************************************************************************/

#if !( ARDUINO_ARCH_NRF52840 && TARGET_NAME == ARDUINO_NANO33BLE )
  #error This code is designed to run on nRF52-based Nano-33-BLE boards using mbed-RTOS platform! Please check your Tools->Board setting.
#endif

// These define's must be placed at the beginning before #include "ESP32_PWM.h"
// _PWM_LOGLEVEL_ from 0 to 4
// Don't define _PWM_LOGLEVEL_ > 0. Only for special ISR debugging only. Can hang the system.
#define _PWM_LOGLEVEL_                3

#define USING_MICROS_RESOLUTION       true    //false 

// To be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error
#include "nRF52_MBED_Slow_PWM.h"

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

#define HW_TIMER_INTERVAL_US      20L

volatile uint32_t startMicros = 0;

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

//////////////////////////////////////////////////////

#define USING_PWM_FREQUENCY     false //true

//////////////////////////////////////////////////////

// You can assign pins here. Be carefull to select good pin to use or crash
uint32_t PWM_Pin    = LED_BUILTIN;

// You can assign any interval for any timer here, in Hz
float PWM_Freq1   = 200.0f;   //1.0f;
// You can assign any interval for any timer here, in Hz
float PWM_Freq2   = 100.0f;   //2.0f;

// You can assign any interval for any timer here, in microseconds
uint32_t PWM_Period1 = 1000000 / PWM_Freq1;
// You can assign any interval for any timer here, in microseconds
uint32_t PWM_Period2 = 1000000 / PWM_Freq2;

// You can assign any duty_cycle for any PWM here, from 0-100
float PWM_DutyCycle1  = 1.0f;     //50.0f;
// You can assign any duty_cycle for any PWM here, from 0-100
float PWM_DutyCycle2  = 5.55f;    //90.0f;

// Channel number used to identify associated channel
int channelNum;

////////////////////////////////////////////////

void setup()
{
  Serial.begin(115200);

  while (!Serial && millis() < 5000);

  delay(2000);

  Serial.print(F("\nStarting ISR_Modify_PWM on "));
  Serial.println(BOARD_NAME);
  Serial.println(NRF52_MBED_SLOW_PWM_VERSION);

  // Interval in microsecs
  if (ITimer.attachInterruptInterval(HW_TIMER_INTERVAL_US, TimerHandler))
  {
    startMicros = micros();
    Serial.print(F("Starting ITimer OK, micros() = "));
    Serial.println(startMicros);
  }
  else
    Serial.println(F("Can't set ITimer. Select another freq. or timer"));

  // Just to demonstrate, don't use too many ISR Timers if not absolutely necessary
  // You can use up to 16 timer for each ISR_PWM
  //void setPWM(uint32_t pin, uint32_t frequency, uint32_t dutycycle
  // , timer_callback_p StartCallback = nullptr, timer_callback_p StopCallback = nullptr)
  Serial.print(F("Using PWM Freq = "));
  Serial.print(PWM_Freq1);
  Serial.print(F(", PWM DutyCycle = "));
  Serial.println(PWM_DutyCycle1);

#if USING_PWM_FREQUENCY

  // You can use this with PWM_Freq in Hz
  ISR_PWM.setPWM(PWM_Pin, PWM_Freq1, PWM_DutyCycle1);

#else
#if USING_MICROS_RESOLUTION
  // Or using period in microsecs resolution
  channelNum = ISR_PWM.setPWM_Period(PWM_Pin, PWM_Period1, PWM_DutyCycle1);
#else
  // Or using period in millisecs resolution
  channelNum = ISR_PWM.setPWM_Period(PWM_Pin, PWM_Period1 / 1000, PWM_DutyCycle1);
#endif
#endif
}

////////////////////////////////////////////////

void changePWM()
{
  static uint8_t count = 1;

  float PWM_Freq;
  float PWM_DutyCycle;

  if (count++ % 2)
  {
    PWM_Freq        = PWM_Freq2;
    PWM_DutyCycle   = PWM_DutyCycle2;
  }
  else
  {
    PWM_Freq        = PWM_Freq1;
    PWM_DutyCycle   = PWM_DutyCycle1;
  }

  // You can use this with PWM_Freq in Hz
  if (!ISR_PWM.modifyPWMChannel(channelNum, PWM_Pin, PWM_Freq, PWM_DutyCycle))
  {
    Serial.print(F("modifyPWMChannel error for PWM_Period"));
  }
}

////////////////////////////////////////////////

void changingPWM()
{
  static ulong changingPWM_timeout = 0;

  static ulong current_millis;

#define CHANGING_PWM_INTERVAL    10000L

  current_millis = millis();

  // changePWM every CHANGING_PWM_INTERVAL (10) seconds.
  if ( (current_millis > changingPWM_timeout) )
  {
    if (changingPWM_timeout > 0)
      changePWM();

    changingPWM_timeout = current_millis + CHANGING_PWM_INTERVAL;
  }
}

////////////////////////////////////////////////

void loop()
{
  changingPWM();
}
