//**************************************************************************
//
//  timeout.c
//    
//    Implements software timers. 
//    The user can regsister a custom function
//    for timeout callback functionality
//
// 
//  T.Weber 4.5.2006
//  SEBA-KMT Baunach, Gemany.
//**************************************************************************

//******************************** INCLUDES ********************************

#include "types.h"
#include "timeout.h"

//******************************** DEFINES *********************************

#define _TO_MAX_TIMERS  4 // maximum amount of possible timers

//******************************* VARIABLES ********************************

// the date of a registered software timer
typedef struct {
  u32 time;
  void(*cb_func)(void);
} TO_array;

TO_array TO_timers[_TO_MAX_TIMERS];

//******************************* FUNCTIONS ********************************

//**************************************************************************
// NAME:
//      TO_register
//
// DESC:
//      Installs a callback 
//
// PARAM: u32 time, void(*cb_func)(void)
//      Time in ticks untill the callback occurs.
//      Function to be called, when timeout occurs.
//
// RETVAL: u16
//      True if successful, false if failed.
//**************************************************************************
u16 TO_register(u32 time, void(*cb_func)(void))
{
  // DECLARATION
  int i;
  
  // INITIALIZATION
  i = 0;
  
  // RANGE CHECK
  if(cb_func == 0 || time == 0)
    return false;
  
  // PROGRAM PART
  TO_timers[i].time = time;
  TO_timers[i].cb_func = cb_func;
  
  return true;
}


//**************************************************************************
// NAME:
//      TO_timerISR
//
// PARAM:
//      none
//
// RETVAL:
//      none
//**************************************************************************
void TO_timerISR(void)
{
  // DECLARATION
  int i;

  // INITIALIZATION
  i = 0;

  // PROGRAM PART
  for(i = 0; i < _TO_MAX_TIMERS; i++) {
    // make sure timer is active
    if(TO_timers[i].time == 0)
      continue;
    // decrease timer
    TO_timers[i].time--;
    // if timeout occured, execute the callback function
    if(TO_timers[i].time == 0) {
      TO_timers[i].cb_func();
      TO_timers[i].cb_func = 0;
    }
  }
}



//**************************************************************************
// NAME:
//      TO_init
//
// PARAM:
//      none
//
// RETVAL:
//      none
//**************************************************************************
void TO_init(void)
{
  int i;
  
  for(i = 0; i < _TO_MAX_TIMERS; i++) {
    TO_timers[i].time = 0;
    TO_timers[i].cb_func = 0;
  }
}
