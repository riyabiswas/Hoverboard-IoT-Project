/*
 * DMASetup.h
 *
 *  Created on: Nov 24, 2016
 *      Authors: Praveen, Riya
 */

#ifndef SRC_DMASETUP_H_
#define SRC_DMASETUP_H_

#include "definitions.h"
#include "includes.h"

volatile uint16_t BufferAdcData[ADC_SAMPLES];
#define DMA_CHANNEL_ADC      0
#define LEDPORT gpioPortE

DMA_CB_TypeDef ADC_cb;
float value;
void ADCTransferComplete(unsigned int channel, bool primary, void *user);
float convertToCelsius(int32_t adcSample);

#endif /* SRC_DMASETUP_H_ */
