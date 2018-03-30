/*
 * includes.h
 *
 *  Created on: Oct 28, 2016
 *      Author: Praveen
 */

#ifndef INCLUDES_H_
#define INCLUDES_H_

#include "vddcheck.h"
#include "rtcdriver.h"

//# include library header files
#include <stdint.h>
#include <stdbool.h>
#include "em_device.h"
#include "em_chip.h"
#include "em_int.h"
#include "em_cmu.h"
#include "em_emu.h"
#include "em_letimer.h"
#include "em_gpio.h"
#include "em_acmp.h"
#include "em_timer.h"
#include "em_rtc.h"
#include "em_lesense.h"
#include "caplesenseconfig.h"
//#include "DMASetup.h"
//#include "ADCSetup.h"
#include "em_adc.h"
#include "em_dma.h"
//#include "ACMP.h"
#include "uart.h"
#include "definitions.h"
//#include "TIMER.h"
//#include "sleep.h"
#include "em_i2c.h"
#include "em_leuart.h"

// GLOBAL VARIABLES

#define compmax 500
int dirn;
int duty_cycle;
int32_t slider;

#endif /* INCLUDES_H_ */
