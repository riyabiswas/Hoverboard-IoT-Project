/*
 * sleep.c
 *
 *  Created on: Nov 3, 2016
 *      Authors: Praveen, Riya
 */

/**************************************************************************//**
 * @file
 * @brief ULFRCO SELF CALIBRATING
 * @author Energy Micro AS
 * @version 3.20.2
 * /******************************************************************************
 * @section License
 * <b>(C) Copyright 2014 Silicon Labs, http://www.silabs.com</b>
 *******************************************************************************
 *
 * This Following routines are licensed under the Silicon Labs Software License Agreement. See
 * "http://developer.silabs.com/legal/version/v11/Silicon_Labs_Software_License_Agreement.txt"
 * for details. Before using this software for any purpose, you must agree to the
 * terms of that agreement.
 *Names of the following routines are modified according to the developer
 *void BlockSleep(unsigned int energymode);
 *void UnBlockSleep(unsigned int energymode);
 *void sleep(void);
 *float convertToCelsius(int32_t adcSample);
 ******************************************************************************/
#include "sleep.h"
unsigned int EnergyMode[4];// For energy modes EM0,1,2,3
//REFERNCED FROM CLASS PDFS ON D2L-Used professor Keith Graham's work for reference.
void BlockSleep(unsigned int energymode)
{
	INT_Disable();
	EnergyMode[energymode]++;
	INT_Enable();
}
//REFERNCED FROM CLASS PDFS ON D2L-Used professor Keith Graham's work for reference.
void UnBlockSleep(unsigned int energymode)
{
	INT_Disable();
	if(EnergyMode[energymode]>0)
		EnergyMode[energymode]--;
	else EnergyMode[energymode]=0;
	INT_Enable();
}
//REFERNCED FROM CLASS PDFS ON D2L-Used professor Keith Graham's work for reference.
void sleep(void){
	if(EnergyMode[EM0]>0){return;}
	else if(EnergyMode[EM1]>0)
		EMU_EnterEM1();
	else if(EnergyMode[EM2]>0)
			EMU_EnterEM2(true);
	else if(EnergyMode[EM3]>0)
			EMU_EnterEM3(true);
}

