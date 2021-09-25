/****************************************************************************************************************************
  nRF52_MBED_Slow_PWM_ISR.hpp
  For nRF52-based boards using Arduino mbed_nano core, such as Nano_33_BLE
  Written by Khoi Hoang

  Built by Khoi Hoang https://github.com/khoih-prog/nRF52_MBED_Slow_PWM
  Licensed under MIT license

  Now even you use all these new 16 ISR-based timers,with their maximum interval practically unlimited (limited only by
  unsigned long miliseconds), you just consume only one nRF52-based timer and avoid conflicting with other cores' tasks.
  The accuracy is nearly perfect compared to software timers. The most important feature is they're ISR-based timers
  Therefore, their executions are not blocked by bad-behaving functions / tasks.
  This important feature is absolutely necessary for mission-critical tasks.

  Version: 1.0.0

  Version Modified By   Date      Comments
  ------- -----------  ---------- -----------
  1.0.0   K.Hoang      25/09/2021 Initial coding for nRF52-based boards using Arduino mbed_nano core, such as Nano_33_BLE
*****************************************************************************************************************************/

#pragma once

#ifndef NRF52_MBED_SLOW_PWM_ISR_HPP
#define NRF52_MBED_SLOW_PWM_ISR_HPP

#include <string.h>

/////////////////////////////////////////////////// 

uint64_t timeNow()
{
#if USING_MICROS_RESOLUTION  
  return ( (uint64_t) micros() );
#else
  return ( (uint64_t) millis() );
#endif    
}
  
/////////////////////////////////////////////////// 

NRF52_MBED_Slow_PWM_ISR::NRF52_MBED_Slow_PWM_ISR()
  : numChannels (-1)
{
}

///////////////////////////////////////////////////

void NRF52_MBED_Slow_PWM_ISR::init() 
{
  uint64_t currentTime = timeNow();
   
  for (uint8_t channelNum = 0; channelNum < MAX_NUMBER_CHANNELS; channelNum++) 
  {
    memset((void*) &PWM[channelNum], 0, sizeof (PWM_t));
    PWM[channelNum].prevTime = currentTime;
    PWM[channelNum].pin      = INVALID_NRF52_MBED_PIN;
  }
  
  numChannels = 0;
}

///////////////////////////////////////////////////

void NRF52_MBED_Slow_PWM_ISR::run() 
{    
  uint64_t currentTime = timeNow();

  for (uint8_t channelNum = 0; channelNum < MAX_NUMBER_CHANNELS; channelNum++) 
  {
    // If enabled => check
    // start period / dutyCycle => digitalWrite HIGH
    // end dutyCycle =>  digitalWrite LOW
    if (PWM[channelNum].enabled) 
    {
      if ( (uint32_t) (currentTime - PWM[channelNum].prevTime) <= PWM[channelNum].onTime ) 
      {              
        if (!PWM[channelNum].pinHigh)
        {
          digitalWrite(PWM[channelNum].pin, HIGH);
          PWM[channelNum].pinHigh = true;
          
          // callbackStart
          if (PWM[channelNum].callbackStart != nullptr)
          {
            (*(timer_callback) PWM[channelNum].callbackStart)();
          }
        }
      }
      else if ( (uint32_t) (currentTime - PWM[channelNum].prevTime) < PWM[channelNum].period ) 
      {
        if (PWM[channelNum].pinHigh)
        {
          digitalWrite(PWM[channelNum].pin, LOW);
          PWM[channelNum].pinHigh = false;
          
          // callback when PWM pulse stops (LOW)
          if (PWM[channelNum].callbackStop != nullptr)
          {
            (*(timer_callback) PWM[channelNum].callbackStop)();
          }
        }
      }
      //else 
      else if ( (uint32_t) (currentTime - PWM[channelNum].prevTime) >= PWM[channelNum].period )   
      {
        PWM[channelNum].prevTime = currentTime;
      }      
    }
  }
}


///////////////////////////////////////////////////

// find the first available slot
// return -1 if none found
int NRF52_MBED_Slow_PWM_ISR::findFirstFreeSlot() 
{
  // all slots are used
  if (numChannels >= MAX_NUMBER_CHANNELS) 
  {
    return -1;
  }

  // return the first slot with no callbackStart (i.e. free)
  for (uint8_t channelNum = 0; channelNum < MAX_NUMBER_CHANNELS; channelNum++) 
  {
    if ( (PWM[channelNum].period == 0) && !PWM[channelNum].enabled )
    {
      return channelNum;
    }
  }

  // no free slots found
  return -1;
}

///////////////////////////////////////////////////

int NRF52_MBED_Slow_PWM_ISR::setupPWMChannel(uint32_t pin, uint32_t period, uint32_t dutycycle, void* cbStartFunc, void* cbStopFunc)
{
  int channelNum;
  
  // Invalid input, such as period = 0, etc
  if ( (period == 0) || (dutycycle > 100) )
  {
    PWM_LOGERROR("Error: Invalid period or dutycycle");
    return -1;
  }

  if (numChannels < 0) 
  {
    init();
  }
 
  channelNum = findFirstFreeSlot();
  
  if (channelNum < 0) 
  {
    return -1;
  }

  PWM[channelNum].pin           = pin;
  PWM[channelNum].period        = period;
  PWM[channelNum].onTime        = ( period * dutycycle ) / 100;
  
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
  PWM[channelNum].pinHigh       = true;
  
  PWM[channelNum].callbackStart = cbStartFunc;
  PWM[channelNum].callbackStop  = cbStopFunc;
  
  PWM_LOGDEBUG0("Channel : "); PWM_LOGDEBUG0(channelNum); PWM_LOGDEBUG0("\tPeriod : "); PWM_LOGDEBUG0(PWM[channelNum].period);
  PWM_LOGDEBUG0("\t\tOnTime : "); PWM_LOGDEBUG0(PWM[channelNum].onTime); PWM_LOGDEBUG0("\tStart_Time : "); PWM_LOGDEBUGLN0(PWM[channelNum].prevTime);
 
  numChannels++;
  
  PWM[channelNum].enabled      = true;
  
  return channelNum;
}

///////////////////////////////////////////////////

void NRF52_MBED_Slow_PWM_ISR::deleteChannel(unsigned channelNum) 
{
  if (channelNum >= MAX_NUMBER_CHANNELS) 
  {
    return;
  }

  // nothing to delete if no timers are in use
  if (numChannels == 0) 
  {
    return;
  }

  // don't decrease the number of timers if the specified slot is already empty (zero period, invalid)
  if ( (PWM[channelNum].pin != INVALID_NRF52_MBED_PIN) && (PWM[channelNum].period != 0) )
  {
    memset((void*) &PWM[channelNum], 0, sizeof (PWM_t));
    
    PWM[channelNum].pin = INVALID_NRF52_MBED_PIN;
    
    // update number of timers
    numChannels--;
  }
}

///////////////////////////////////////////////////

void NRF52_MBED_Slow_PWM_ISR::restartChannel(unsigned channelNum) 
{
  if (channelNum >= MAX_NUMBER_CHANNELS) 
  {
    return;
  }

  PWM[channelNum].prevTime = timeNow();
}

///////////////////////////////////////////////////

bool NRF52_MBED_Slow_PWM_ISR::isEnabled(unsigned channelNum) 
{
  if (channelNum >= MAX_NUMBER_CHANNELS) 
  {
    return false;
  }

  return PWM[channelNum].enabled;
}

///////////////////////////////////////////////////

void NRF52_MBED_Slow_PWM_ISR::enable(unsigned channelNum) 
{
  if (channelNum >= MAX_NUMBER_CHANNELS) 
  {
    return;
  }

  PWM[channelNum].enabled = true;
}

///////////////////////////////////////////////////

void NRF52_MBED_Slow_PWM_ISR::disable(unsigned channelNum) 
{
  if (channelNum >= MAX_NUMBER_CHANNELS) 
  {
    return;
  }

  PWM[channelNum].enabled = false;
}

///////////////////////////////////////////////////

void NRF52_MBED_Slow_PWM_ISR::enableAll() 
{
  // Enable all timers with a callbackStart assigned (used)

  for (uint8_t channelNum = 0; channelNum < MAX_NUMBER_CHANNELS; channelNum++) 
  {
    if (PWM[channelNum].period != 0)
    {
      PWM[channelNum].enabled = true;
    }
  }
}

///////////////////////////////////////////////////

void NRF52_MBED_Slow_PWM_ISR::disableAll() 
{
  // Disable all timers with a callbackStart assigned (used)
  for (uint8_t channelNum = 0; channelNum < MAX_NUMBER_CHANNELS; channelNum++) 
  {
    if (PWM[channelNum].period != 0)
    {
      PWM[channelNum].enabled = false;
    }
  }
}

///////////////////////////////////////////////////

void NRF52_MBED_Slow_PWM_ISR::toggle(unsigned channelNum) 
{
  if (channelNum >= MAX_NUMBER_CHANNELS) 
  {
    return;
  }

  PWM[channelNum].enabled = !PWM[channelNum].enabled;
}

///////////////////////////////////////////////////

unsigned NRF52_MBED_Slow_PWM_ISR::getnumChannels() 
{
  return numChannels;
}

#endif    // NRF52_MBED_SLOW_PWM_ISR_HPP
