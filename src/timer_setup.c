/*
 * timer_setup.c
 *
 *  Created on: Nov 1, 2016
 *      Author: Praveen
 */

#include "timer_setup.h"

int comp1=500;

/**************************************************************************//**
 * @brief  LETIMER_setup
 * Configures and starts the LETIMER0
 *****************************************************************************/
void timer_setup(void)
{
  /* Enable necessary clocks */
  CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
  CMU_ClockEnable(cmuClock_CORELE, true);
  CMU_ClockEnable(cmuClock_LETIMER0, true);
  CMU_ClockEnable(cmuClock_GPIO, true);

  /* Configure PD6 and PD7 as push pull so the
     LETIMER can override them */
  GPIO_PinModeSet(gpioPortD, 6, gpioModePushPull, 0);
  GPIO_PinModeSet(gpioPortD, 7, gpioModePushPull, 0);
  GPIO_PinModeSet(gpioPortB, 11, gpioModePushPull, 0);
//  GPIO_PinModeSet(gpioPortD, 7, gpioModePushPull, 0);
  //INCLUDE ENABLE PIN
  /* Set initial compare values for COMP0 and COMP1
     COMP1 keeps it's value and is used as TOP value
     for the LETIMER.
     COMP1 gets decremented through the program execution
     to generate a different PWM duty cycle */
  LETIMER_CompareSet(LETIMER0, 0, COMPMAX);
  LETIMER_CompareSet(LETIMER0, 1, 0);

  /* Repetition values must be nonzero so that the outputs
     return switch between idle and active state */
  LETIMER_RepeatSet(LETIMER0, 0, 0x01);
  LETIMER_RepeatSet(LETIMER0, 1, 0x01);

  /* Route LETIMER to location 0 (PD6 and PD7) and enable outputs */
  LETIMER0->ROUTE = LETIMER_ROUTE_OUT0PEN  |LETIMER_ROUTE_OUT1PEN| LETIMER_ROUTE_LOCATION_LOC1;

  /* Set configurations for LETIMER 0 */
  const LETIMER_Init_TypeDef letimerInit =
  {
  .enable         = true,                   /* Start counting when init completed. */
  .debugRun       = false,                  /* Counter shall not keep running during debug halt. */
  .rtcComp0Enable = false,                  /* Don't start counting on RTC COMP0 match. */
  .rtcComp1Enable = false,                  /* Don't start counting on RTC COMP1 match. */
  .comp0Top       = true,                   /* Load COMP0 register into CNT when counter underflows. COMP0 is used as TOP */
  .bufTop         = false,                  /* Don't load COMP1 into COMP0 when REP0 reaches 0. */
  .out0Pol        = 0,                      /* Idle value for output 0. */
  .out1Pol        = 0,                      /* Idle value for output 1. */
  .ufoa0          = letimerUFOAPwm,         /* PWM output on output 0 */
  .ufoa1          = letimerUFOAPulse,       /* Pulse output on output 1*/
  .repMode        = letimerRepeatFree       /* Count until stopped */
  };

  /* Initialize LETIMER */
  LETIMER_Init(LETIMER0, &letimerInit);
}
