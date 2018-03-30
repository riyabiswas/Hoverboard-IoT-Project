/**************************************************************************//**
 * @file
 * @brief Empty Project
 * @author Energy Micro AS
 * @version 3.20.2
 ******************************************************************************
 * @section License
 * <b>(C) Copyright 2014 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This file is licensed under the Silicon Labs Software License Agreement. See 
 * "http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt"  
 * for details. Before using this software for any purpose, you must agree to the 
 * terms of that agreement.
 *
 ******************************************************************************/
#include "includes.h"

/**************************************************************************//**
 * @brief  Main function
 *****************************************************************************/
int main(void)
{
  /* Chip errata */
  CHIP_Init();
  CMU_setup();	//SET UP ALL THE CLOCK OF CONTROLLER AND PERIPHERALS
  BlockSleep(block_sleep_mode);
  gpio_setup();

//  ADCSetup();
//  DMASetup();
//  ADC_Start(ADC0, adcStartSingle);

  uart0_init();

  i2c_setup();
  timer_setup();

  LESENSE_setup();

/* Infinite loop */
  while (1)
  {
	  sleep();
  }
}
void cir_buf_array(char* pntr, int sze)
{
	for(int cnnt=0;cnnt<(sze);cnnt++,pntr++)
	{
		circular_buffer[cnnt+write_pntr]=*pntr;
	}
	glbl_write=(glbl_write+sze)%buffer_length;

		write_pntr=(write_pntr+sze)%buffer_length;
		LEUART0->IEN|=LEUART_IEN_TXBL;
}

