/****************************************************************************************************************************
  nRF52_MBED_Slow_PWM_Impl.h
  For nRF52-based boards using Arduino mbed_nano core, such as Nano_33_BLE
  Written by Khoi Hoang

  Built by Khoi Hoang https://github.com/khoih-prog/nRF52_MBED_Slow_PWM
  Licensed under MIT license

  Now even you use all these new 16 ISR-based timers,with their maximum interval practically unlimited (limited only by
  unsigned long miliseconds), you just consume only one nRF52-based timer and avoid conflicting with other cores' tasks.
  The accuracy is nearly perfect compared to software timers. The most important feature is they're ISR-based timers
  Therefore, their executions are not blocked by bad-behaving functions / tasks.
  This important feature is absolutely necessary for mission-critical tasks.

  Version: 1.2.2

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.0.0   K.Hoang      25/09/2021 Initial coding for nRF52-based boards using Arduino mbed_nano core, such as Nano_33_BLE
  1.1.0   K Hoang      10/11/2021 Add functions to modify PWM settings on-the-fly
  1.2.0   K.Hoang      07/02/2022 Fix `multiple-definitions` linker error. Improve accuracy. Optimize code. Fix bug
  1.2.1   K Hoang      03/03/2022 Fix `DutyCycle` and `New Period` display bugs. Display warning only when debug level > 3
  1.2.2   K Hoang      26/10/2022 Add support to SEEED_XIAO_NRF52840 and SEEED_XIAO_NRF52840_SENSE using mbed
*****************************************************************************************************************************/

/*
  nRF52 has 5 Hardware TIMERs: NRF_TIMER0-NRF_TIMER4
  NRF_TIMER0 is used by the soft device, NRF_TIMER1-NRF_TIMER4 are available

  Defined in file ./adafruit/hardware/nrf52/0.21.0/cores/nRF5/nordic/nrfx/mdk/nrf52.h

  #define NRF_TIMER0_BASE             0x40008000UL
  #define NRF_TIMER1_BASE             0x40009000UL
  #define NRF_TIMER2_BASE             0x4000A000UL
  #define NRF_TIMER3_BASE             0x4001A000UL
  #define NRF_TIMER4_BASE             0x4001B000UL
  ...
  #define NRF_TIMER0                  ((NRF_TIMER_Type*)         NRF_TIMER0_BASE)
  #define NRF_TIMER1                  ((NRF_TIMER_Type*)         NRF_TIMER1_BASE)
  #define NRF_TIMER2                  ((NRF_TIMER_Type*)         NRF_TIMER2_BASE)
  #define NRF_TIMER3                  ((NRF_TIMER_Type*)         NRF_TIMER3_BASE)
  #define NRF_TIMER4                  ((NRF_TIMER_Type*)         NRF_TIMER4_BASE)

  ===============================================================================

  Defined in ./adafruit/hardware/nrf52/0.21.0/cores/nRF5/nordic/nrfx/hal/nrf_timer.h

  Timer prescalers
  typedef enum
  {
      NRF_TIMER_FREQ_16MHz = 0, ///< Timer frequency 16 MHz.
      NRF_TIMER_FREQ_8MHz,      ///< Timer frequency 8 MHz.
      NRF_TIMER_FREQ_4MHz,      ///< Timer frequency 4 MHz.
      NRF_TIMER_FREQ_2MHz,      ///< Timer frequency 2 MHz.
      NRF_TIMER_FREQ_1MHz,      ///< Timer frequency 1 MHz.
      NRF_TIMER_FREQ_500kHz,    ///< Timer frequency 500 kHz.
      NRF_TIMER_FREQ_250kHz,    ///< Timer frequency 250 kHz.
      NRF_TIMER_FREQ_125kHz,    ///< Timer frequency 125 kHz.
      NRF_TIMER_FREQ_62500Hz,   ///< Timer frequency 62500 Hz.
      NRF_TIMER_FREQ_31250Hz    ///< Timer frequency 31250 Hz.
  } nrf_timer_frequency_t;

*/

#pragma once

#ifndef NRF52_MBED_SLOW_PWM_IMPL_H
#define NRF52_MBED_SLOW_PWM_IMPL_H

// Timer 0 is used by the soft device
// Timer 2 is used by the mbed-RTOS
// only Timer 1, 3 and 4 are available
extern "C" void TIMER1_IRQHandler_v()
{
  if (nRF52Timers[1])
  {
    nRF52Timers[1]->detachInterrupt();

    (*(nRF52Timers[1]->getCallback()))();

    nRF52Timers[1]->enableTimer();
  }
}

extern "C" void TIMER3_IRQHandler_v()
{
  if (nRF52Timers[3])
  {
    nRF52Timers[3]->detachInterrupt();

    (*(nRF52Timers[3]->getCallback()))();

    nRF52Timers[3]->enableTimer();
  }
}

extern "C" void TIMER4_IRQHandler_v()
{
  if (nRF52Timers[4])
  {
    nRF52Timers[4]->detachInterrupt();

    (*(nRF52Timers[4]->getCallback()))();

    nRF52Timers[4]->enableTimer();
  }
}

#endif    // NRF52_MBED_SLOW_PWM_IMPL_H

