/*
 * ADCSetup.c
 *
 *  Created on: Nov 24, 2016
 *      Authors: Praveen, Riya
 */
#include "ADCSetup.h"
uint16_t i;
int32_t sum=0;
uint32_t adc_array[ADC_SAMPLES];

void ADC0_IRQHandler()
{
	INT_Disable();
	 ADC_IntDisable(ADC0,ADC_IEN_SINGLE);
	ADC_IntClear(ADC0,ADC_IEN_SINGLE);
					  int32_t sum=0;
					  uint32_t adc_array[ADC_SAMPLES];
						  adc_array[i]=ADC0->SINGLEDATA;
						  i++;

//				  if(i==(ADC_SAMPLES-1))
//				  {
//				  i=0;
//				  UnBlockSleep(ADC_EM);
//				  ADC_IntClear(ADC0,ADC_IEN_SINGLE);
//				  ADC_Start(ADC0,ADC_CMD_SINGLESTOP);
//				  while(i<ADC_SAMPLES)
//					  {
//					  sum=sum+adc_array[i];
//					  i++;
//					  }
//				  i=0;
//					  sum = sum/ADC_SAMPLES;
					  sum=adc_array[0];
					  float temp;
					  temp=convertToCelsius(sum);

//					  if((upper_temp>=temp)&&(temp>=lower_temp))
//						  GPIO_PinOutClear(LEDPORT, 3U);
//					  else
//						  GPIO_PinOutSet(LEDPORT, 3U);
//				  }

	  INT_Enable();
}
void ADCSetup(void)
{
  ADC_Init_TypeDef       init       = ADC_INIT_DEFAULT;
   ADC_InitSingle_TypeDef singleInit = ADC_INITSINGLE_DEFAULT;

  /* Init common settings for both single conversion and scan mode */
  init.timebase = ADC_TimebaseCalc(0);		//since hfperclk=0.071 micro seconds
  init.warmUpMode= ADC0_warmup;
  /* Might as well finish conversion as quickly as possibly since polling */
  /* for completion. */
  /* Set ADC clock to 7 MHz, use default HFPERCLK */
  init.prescale = ADC_PrescaleCalc(adc_clk, 0);
  /* Set oversampling rate */
  init.ovsRateSel = adcOvsRateSel2;

  /* WARMUPMODE must be set to Normal according to ref manual before */
  /* entering EM2. In this example, the warmup time is not a big problem */
  /* due to relatively infrequent polling. Leave at default NORMAL, */

  ADC_Init(adc_channel, &init);

  /* Init for single conversion use, measure VDD/3 with 1.25 reference. */
  singleInit.reference = int_temp_reference;
  singleInit.input     = temperature_pin;
  singleInit.prsEnable = false;
  singleInit.leftAdjust = false;
  /* Enable oversampling rate */
  singleInit.resolution = adc_resolution;

  /* The datasheet specifies a minimum aquisition time when sampling vdd/3 */
  /* 32 cycles should be safe for all ADC clock frequencies */
  singleInit.acqTime = adc_acquire_time;
  singleInit.rep = ADC0_rep;
  ADC_InitSingle(adc_channel, &singleInit);
//  NVIC_EnableIRQ(ADC0_IRQn);
}

