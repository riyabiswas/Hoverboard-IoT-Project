/*
 * PWM_rtc.c
 *
 *  Created on: Nov 6, 2016
 *      Authors: Praveen
 *      		Riya
 */
#include "PWM_rtc.h"

/* Defines*/
//#define LFXO_FREQUENCY 32768



int count;


/**************************************************************************//**
 * @brief RTC Interrupt Handler, clears the flag.
 *****************************************************************************/
//void RTC_IRQHandler(void)
//{
//
//  count++;
//  INT_Disable();
//  NVIC_DisableIRQ(RTC_IRQn);
//
////  	/* Clear interrupt source */
////  	  RTC_IntClear(RTC_IFC_COMP0);
//
//
//  	 int IntFlags;
//  	   	IntFlags=RTC->IF;
//  	   	RTC->IFC=IntFlags;
//
//
//  if((IntFlags & RTC_IF_COMP1)!=0){
//  	  if(dirn==1 )
//  	  {
//  		  GPIO_PinOutSet(gpioPortD, 6);
//  		  GPIO_PinOutClear(gpioPortD, 7);
//  	  }
//
//  	  if(dirn==(-1))
//  	  {
//  		  GPIO_PinOutSet(gpioPortD, 7);
//  		  GPIO_PinOutClear(gpioPortD, 6);
//  	  }
//  	  //flagg=1;
//    }
//    //else{
//    if ((IntFlags & RTC_IF_COMP0)!=0)
//    {
//
//  		  GPIO_PinOutClear(gpioPortD, 6);
//  	  	  GPIO_PinOutClear(gpioPortD, 7);
//
//  	  //flagg=0;
//    }
//
//    NVIC_EnableIRQ(RTC_IRQn);
//    INT_Enable();
//}

/**************************************************************************//**
 * @brief  Start LFRCO for RTC
 * Starts the low frequency RC oscillator (LFRCO) and routes it to the RTC
 *****************************************************************************/
void startLfxoForRtc(void)
{
  /* Starting LFRCO and waiting until it is stable */
  CMU_OscillatorEnable(cmuOsc_LFXO, true, true);

  /* Routing the LFRCO clock to the RTC */
  CMU_ClockSelectSet(cmuClock_LFA,cmuSelect_LFXO);
  CMU_ClockEnable(cmuClock_RTC, true);

  /* Enabling clock to the interface of the low energy modules */
  CMU_ClockEnable(cmuClock_CORELE, true);
}


/**************************************************************************//**
 * @brief  Setup RTC
 * On compare match with COMP0, clear counter and set interrupt
 *****************************************************************************/
void setupRtc(void)
{

	CMU_ClockEnable(cmuClock_GPIO, true);

	  /* Configure PD6 and PD7 as push pull so the
	     LETIMER can override them */
	  GPIO_PinModeSet(gpioPortD, 6, gpioModePushPull, 0);
	  GPIO_PinModeSet(gpioPortD, 7, gpioModePushPull, 0);
	  GPIO_PinModeSet(gpioPortC,6, gpioModePushPull, 0);

	  GPIO_PinOutSet(gpioPortC, 6);

  /* Configuring clocks in the Clock Management Unit (CMU) */
  startLfxoForRtc();

  RTC_Init_TypeDef rtcInit = RTC_INIT_DEFAULT;

  rtcInit.enable   = true;      /* Enable RTC after init has run */
  rtcInit.comp0Top = true;      /* Clear counter on compare match */
  rtcInit.debugRun = false;     /* Counter shall keep running during debug halt. */

  /* Setting the compare value of the RTC */
  RTC_CompareSet(0, compmax);
  duty_cycle=50;
  dirn=1;
  RTC_CompareSet(1, (compmax*(duty_cycle/100)));


  /* Enabling Interrupt from RTC */
//  RTC_IntEnable(RTC_IFC_OF);
//  RTC_IntEnable(RTC_IFC_COMP1);
  RTC->IEN=RTC_IEN_COMP0|RTC_IEN_COMP1;



  NVIC_EnableIRQ(RTC_IRQn);

  /* Initialize the RTC */
  RTC_Init(&rtcInit);
}








