/*
 * CMU.c
 *
 *  Created on: Oct 28, 2016
 *      Author: Praveen
 */
#include "CMU.h"

void CMU_setup()//Reference main_letimer_rtc_pulses.c-> LETIMER_Setup(void)
{
	/* Ensure core frequency has been updated */
	  SystemCoreClockUpdate();

	  /* Select clock source for HF clock. */
	   CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFRCO);
	   /* Select clock source for LFA clock. */
	   CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
	   /* Disable clock source for LFB clock. */
	   CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_Disabled);

	 CMU_ClockEnable(cmuClock_HFPER, true);
	 /* Enable clock for ACMP0. */
	  CMU_ClockEnable(cmuClock_ACMP0, 1);
	  /* Enable clock for ACMP1. */
	  CMU_ClockEnable(cmuClock_ACMP1, 1);
	  /* Enable CORELE clock. */
	  CMU_ClockEnable(cmuClock_CORELE, 1);

	/* Enable clock for TIMER0 module */
	CMU_ClockEnable(cmuClock_TIMER0, true);

	CMU_ClockEnable(cmuClock_ADC0, true);
	CMU_ClockEnable(cmuClock_DMA, true);
	CMU_ClockEnable(cmuClock_I2C1,true);
	CMU_ClockEnable(cmuClock_LEUART0,true);

	  /* Enable clock on RTC. */
	  CMU_ClockEnable(cmuClock_RTC, true);

	  /* Enable clock divider for RTC. */
	  CMU_ClockDivSet(cmuClock_RTC, cmuClkDiv_32768);
  /* Enable clock for LESENSE. */
  CMU_ClockEnable(cmuClock_LESENSE, true);
  /* Set clock divider for LESENSE. */
  CMU_ClockDivSet(cmuClock_LESENSE, cmuClkDiv_1);

	if(block_sleep_mode== EM3)
	{
		CMU_ClockSelectSet(cmuClock_LFA,cmuSelect_ULFRCO);
		//CMU_ClockSelectSet(cmuClock_LFB,cmuSelect_ULFRCO);
	}
	else
	{
		/* Enable necessary clocks */
		CMU_ClockSelectSet(cmuClock_LFA, cmuSelect_LFXO);
		//CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFXO);
	}

	  CMU_ClockEnable(cmuClock_LETIMER0, true);
	  CMU_ClockEnable(cmuClock_GPIO, true);
}
