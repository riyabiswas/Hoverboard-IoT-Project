/*
 * uart.c
 *
 *  Created on: Nov 24, 2016
 *      Authors: Praveen
 *      		 Riya
 */

#include "uart.h"
void uart0_init()
{

   CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFRCO);
 /* Reseting and initializing LEUART1 */
  LEUART_Reset(LEUART0);

  LEUART_Init_TypeDef leuart0Init =
  {
   // .enable   = leuartEnableTx,       /* Activate data reception on LEUn_TX pin. */
    .refFreq  = LEUART0_REF_FREQ,                    /* Inherit the clock frequenzy from the LEUART clock source */
    .baudrate = LEUART0_BAUD,                 /* Baudrate = 9600 bps */
    .databits = LEUART0_DATABITS,      /* Each LEUART frame containes 8 databits */
    .parity   = LEUART0_PARITY,       /* No parity bits in use */
    .stopbits = LEUART0_STOP_BITS,      /* Setting the number of stop bits in a frame to 2 bitperiods */
  };

  LEUART_Init(LEUART0, &leuart0Init);

  /* Route LEUART1 TX pin to DMA location 0 */
  LEUART0->ROUTE = LEUART_ROUTE_TXPEN |
                   _LEUART_ROUTE_LOCATION_LOC0;
  LEUART0->CMD|=LEUART_CMD_TXEN;

  /* Enable GPIO for LEUART1. TX is on C6 */
  GPIO_PinModeSet(LEUART0_PORT,                /* GPIO port *///PD4-TX PIN
		  	  	  LEUART0_PIN,                 /* GPIO port number */
		  	  	  LEUART0_MODE,         /* Pin mode is set to push pull */
                  LEUART0_STATE);                       /* High idle state */
//  LEUART0->IEN|=LEUART_IEN_TXC;
  NVIC_EnableIRQ(LEUART0_IRQn);
}

void LEUART0_IRQHandler()
{
	INT_Disable();
	LEUART0->IFC=LEUART_IFC_TXC;
	while(read_pntr!=write_pntr)
	{

		char a=circular_buffer[read_pntr];

		LEUART_Tx(LEUART0,a);
		circular_buffer[read_pntr]='\0';
		read_pntr=(read_pntr+1)%buffer_length;
	}
//	else
		LEUART0->IEN&=~LEUART_IEN_TXBL;

//	else
//	{
//		if(glbl_write>0)
//		{
//		char b=circular_buffer[glbl];
//		LEUART_Tx(LEUART0,circular_buffer[glbl]);
//		glbl=(glbl+1)%buffer_length;
//		glbl_write--;
//		}
//		if(glbl_write==0)
//		LEUART0->IEN&=~LEUART_IEN_TXBL;
//	}

	INT_Enable();
}
