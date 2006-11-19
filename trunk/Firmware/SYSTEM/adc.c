//*----------------------------------------------------------------------------
//*         ATMEL Microcontroller Software Support  -  ROUSSET  -
//*----------------------------------------------------------------------------
//* The software is delivered "AS IS" without warranty or condition of any
//* kind, either express, implied or statutory. This includes without
//* limitation any warranty or condition with respect to merchantability or
//* fitness for any particular purpose, or against the infringements of
//* intellectual property rights of others.
//*----------------------------------------------------------------------------
//* File Name           : main.c
//* Object              : main application written in C
//* Creation            : S.CADENE   01/September/2004
//* Vers                : V1.0
//* Aim                 : What the minimum program is to perform
//*----------------------------------------------------------------------------

#include "adc.h"
#include "config.h"

/* ADC field definition for the Mode Register: Reminder
                       TRGEN    => Selection bewteen Software or hardware start of conversion
                       TRGSEL   => Relevant if previous field is set on hardware triggering
                       LOWRES   => 10-bit result if ths bit is cleared 0
                       SLEEP    => normal mode if ths is cleared
                       PRESCAL  => ADCclock = MCK / [(PRESCAL + 1)*2]
                       STARTUP  => Startup Time = [(STARTUP + 1)*8] / ADCclock
                       SHTIM    => Tracking time = (SHTIM + 1) / ADCclock
 */
#define   TRGEN    (0x0)    // Software triggering
#define   TRGSEL   (0x0)    // Without effect
#define   LOWRES   (0x0)    // 10-bit result output
#define   SLEEP    (0x0)    // Normal Mode
#define   PRESCAL  (0x9)    // Max value
#define   STARTUP  (0x7)    // This time period must be higher than 20 µs and not 20 ms
#define   SHTIM    (0x3)    // Must be higher than 3 ADC clock cycles but depends on output
                            // impedance of the analog driver to the ADC input


// the moving average buffer for all ADC sources 


//**************************************************************************
// NAME: ADC_init
//
// DESC: Initialises the AT91 ADC part
//      
// PARAM: void
//      
// RETVAL: void
//         TRUE if successful, FALSE if failed.
//**************************************************************************
void ADC_init(t_ADC_channel* cs)
{
  //***** DECLARATION *****
  int i;
  u32 running_mode;
  //***** INITIALZATION *****
  //***** RANGE CHECK *****
  if(cs == NULL)
    return;
  //***** PROGRAM *****
  /* First step: Enable ADC's Clock at PMC level.*/
  AT91F_PMC_EnablePeriphClock ( AT91C_BASE_PMC, 1 << AT91C_ID_ADC ) ;
  
  /* Second step: Set up by using ADC Mode register */
  running_mode = ((SHTIM << 24) | (STARTUP << 16) | (PRESCAL << 8) | (SLEEP << 5) | (LOWRES <<4) | (TRGSEL << 1) | (TRGEN ));
  AT91F_ADC_CfgModeReg (AT91C_BASE_ADC, running_mode) ;
  
  /* Third Step: Select the active channel */
  AT91F_ADC_EnableChannel (AT91C_BASE_ADC, (1<<cs->channel));
  // clear channel structure data  
  for(i = 0; i < _ADC_MAVG_POINTS; i++) {
     cs->buffer[i] = 0;
  }
  cs->mavgp = 0;
}


//**************************************************************************
// NAME: ADC_start_conversion
//
// DESC: Starts the ADC conversion
//      
// PARAM: void
//      
// RETVAL: void
//         TRUE if successful, FALSE if failed.
//**************************************************************************
void ADC_start_conversion(void)
{
  //***** DECLARATION *****
  //***** INITIALZATION *****
  //***** RANGE CHECK *****
  //***** PROGRAM *****
  /* Fourth Step: Start the conversion */
  AT91F_ADC_StartConversion (AT91C_BASE_ADC);
}


//**************************************************************************
// NAME: ADC_get_result
//
// DESC: 
//      
// PARAM: const u8 channel
//      
// RETVAL: u16
//         TRUE if successful, FALSE if failed.
//**************************************************************************
u16 ADC_get_result(const t_ADC_channel* cs)
{
  u16 result;
  //***** DECLARATION *****
  //***** INITIALZATION *****
  //***** RANGE CHECK *****
  if(cs == NULL)
    return 0;
  //***** PROGRAM *****
  /* Fifth Step: Waiting Stop Of conversion by pulling */
  while (!((AT91F_ADC_GetStatus (AT91C_BASE_ADC)) & (1 << cs->channel)));

  /* Sixth Step: Read the result */
  result = AT91F_ADC_GetLastConvertedData(AT91C_BASE_ADC);
  return result;
}


//**************************************************************************
// NAME: ADC_get_mavg
//
// DESC: 
//      
// PARAM: u8 channel
//      
// RETVAL: void
//         TRUE if successful, FALSE if failed.
//**************************************************************************
u16 ADC_get_mavg(t_ADC_channel* cs)
{
  //***** DECLARATION *****
  u16 result;
  u32 sum;
  int i;
  //***** INITIALZATION *****
  sum = 0;
  //***** RANGE CHECK *****
  if(cs == NULL)
    return 0;
  //***** PROGRAM *****
  result = ADC_get_result(cs);
  
  //put current result into buffer
  cs->buffer[cs->mavgp++] = result;
  if(cs->mavgp >= _ADC_MAVG_POINTS)
    cs->mavgp = 0;
  // calculate average
  for(i = 0; i < _ADC_MAVG_POINTS; i++) {
    sum += cs->buffer[i];
  }
  sum /= _ADC_MAVG_POINTS;
  cs->value = sum;
  return sum;
}


//* End
