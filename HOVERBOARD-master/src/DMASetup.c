/*
 * DMASetup.c
 *
 *  Created on: Nov 24, 2016
 *      Author: Praveen
 *      		Riya	
 */
//#include "em_dma.h"
#include "dmactrl.c"
#include "dmactrl.h"
//#include "em_adc.h"
//#include "em_int.h"
//#include "em_gpio.h"
//#include "em_cmu.h"
#include "DMASetup.h"


volatile uint16_t BufferAdcData[ADC_SAMPLES];
#define DMA_CHANNEL_ADC      0
#define LEDPORT gpioPortE

DMA_CB_TypeDef ADC_cb;
float value;
void ADCTransferComplete(unsigned int channel, bool primary, void *user);
float convertToCelsius(int32_t adcSample);
//void DMASetup();
/**************************************************************************//**
 * @brief  Call-back called when transfer is complete
 *****************************************************************************/
void ADCTransferComplete(unsigned int channel, bool primary, void *user)
{
  INT_Disable();
  DMA_IntClear(DMA_CHANNEL_ADC);
  UnBlockSleep(ADC_EM);
  adc_complete=1;
  ADC_Start(ADC0,ADC_CMD_SINGLESTOP);
  CMU_ClockEnable(cmuClock_ADC0, false);

      uint16_t i=0;
      int32_t sum=0;

      while(i<ADC_SAMPLES)
      {
      sum=sum+BufferAdcData[i];
      i++;
      }
      sum = sum/ADC_SAMPLES;
      float temp;
      ADC_Start(ADC0, adcStartSingle);
//      while(ADC0->STATUS & ADC_STATUS_SINGLEACT);
      temp=convertToCelsius(ADC0->SINGLEDATA);

      if(temp<0)
      	  {
    	  	circular_buffer[write_pntr]='T';
			write_pntr=(write_pntr+1)%buffer_length;


    	  	circular_buffer[write_pntr]='-';
			write_pntr=(write_pntr+1)%buffer_length;

			temp=temp*(-1);		//convert temp to positive
			int first=temp;
    	  	circular_buffer[write_pntr]=first;
			write_pntr=(write_pntr+1)%buffer_length;

			int second =temp*100;
			first=first*100;
			second=second-first;
    	  	circular_buffer[write_pntr]=second;
			write_pntr=(write_pntr+1)%buffer_length;
			temp=temp*(-1);

      	  }
      	  else
      	  {
      	  	circular_buffer[write_pntr]='T';
  			write_pntr=(write_pntr+1)%buffer_length;


      	  	circular_buffer[write_pntr]='+';
  			write_pntr=(write_pntr+1)%buffer_length;

  			int first=temp;
      	  	circular_buffer[write_pntr]=first;
  			write_pntr=(write_pntr+1)%buffer_length;

  			int second =temp*100;
  			first=first*100;
  			second=second-first;
      	  	circular_buffer[write_pntr]=second;
  			write_pntr=(write_pntr+1)%buffer_length;

      	  }
  INT_Enable();
}


/* /******************************************************************************
* @section License
* <b>(C) Copyright 2014 Silicon Labs, http://www.silabs.com</b>
*******************************************************************************
*
* This Following routines are licensed under the Silicon Labs Software License Agreement. See
* "http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt"
* for details. Before using this software for any purpose, you must agree to the
* terms of that agreement.
*Names of the following routines are modified according to the developer
*float convertToCelsius(int32_t adcSample);
******************************************************************************/

float convertToCelsius(int32_t adcSample)
{
	float temp;
	float cal_temp_0=(float)((DEVINFO->CAL & _DEVINFO_CAL_TEMP_MASK) >> _DEVINFO_CAL_TEMP_SHIFT);
	float  cal_value_0=(float)((DEVINFO->ADC0CAL2 & _DEVINFO_ADC0CAL2_TEMP1V25_MASK) >> _DEVINFO_ADC0CAL2_TEMP1V25_SHIFT);

	float t_grad = -6.27;

	temp=(cal_temp_0-((cal_value_0-adcSample)/t_grad));
	return temp;
}
void DMASetup(void)
{
  DMA_Init_TypeDef       dmaInit;
  DMA_CfgDescr_TypeDef   descrCfg;
  DMA_CfgChannel_TypeDef chnlCfg;


  /* Configure general DMA issues */
  dmaInit.hprot        = 0;
  dmaInit.controlBlock = dmaControlBlock;
  DMA_Init(&dmaInit);

  /* Setting up call-back function */
  ADC_cb.cbFunc  = ADCTransferComplete;
//  ADC_cb.userPtr = &(ADCTransferComplete);
  ADC_cb.userPtr = NULL;
  ADC_cb.primary = true;
  /* Configure DMA channel used */
  chnlCfg.highPri   = true;
  chnlCfg.enableInt = true;
  chnlCfg.select    = DMAREQ_ADC0_SINGLE;
  chnlCfg.cb        = &(ADC_cb);
  DMA_CfgChannel(DMA_CHANNEL_ADC, &chnlCfg);

  /* Setting up channel descriptor */
    descrCfg.dstInc  = dmaDataInc2;
    descrCfg.srcInc  = dmaDataIncNone;
    descrCfg.size    = dmaDataSize2;
    descrCfg.arbRate = ADC0_DMA_Arbitration;
    descrCfg.hprot   = 0;
    DMA_CfgDescr(DMA_CHANNEL_ADC, TEMP_Sensor_DMA, &descrCfg);
    DMAActive();
}
void DMAActive(void)
{
	 /* Starting transfer. Using Basic since every transfer must be initiated
	     * by the ADC. */

	 DMA_ActivateBasic(DMA_CHANNEL_ADC,
	                      true,
	                      false,
	                      (void *)BufferAdcData,
	                      (void *)&(ADC0->SINGLEDATA),
	                      ADC_SAMPLES - 1);
}
