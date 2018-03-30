/*
 * gpio_setup.c
 *
 *  Created on: Nov 1, 2016
 *      Author: Praveen
 *      		Riya
 */

#include "gpio_setup.h"

  /* Set CC0 location 3 pin (PD1) as output */
void gpio_setup()
{
	  /* Configure the drive strength of the ports for the light sensor. */
	  GPIO_DriveModeSet(LIGHTSENSE_EXCITE_PORT, gpioDriveModeStandard);
	  GPIO_DriveModeSet(LIGHTSENSE_SENSOR_PORT, gpioDriveModeStandard);
	  GPIO_PinModeSet(LEDPORT, LEDPIN1, gpioModePushPull, 0);
//	  GPIO_PinModeSet(LEDPORT, LEDPIN0, gpioModePushPull, 0);
	//  GPIO_DriveModeSet(LIGHTSENSE_SENSOR_PORT, gpioDriveModeStandard);

	  /* Initialize the 2 GPIO pins of the light sensor setup. */
	  GPIO_PinModeSet(LIGHTSENSE_EXCITE_PORT, LIGHTSENSE_EXCITE_PIN, gpioModePushPull, 0);
	  GPIO_PinModeSet(LIGHTSENSE_SENSOR_PORT, LIGHTSENSE_SENSOR_PIN, gpioModeDisabled, 0);

	  GPIO_PinModeSet(LIGHTSENSE_SENSOR_PORT, 7U, gpioModeDisabled,0);
	  GPIO_PinModeSet(LIGHTSENSE_SENSOR_PORT, 1U, gpioModeDisabled,0);
	  GPIO_PinModeSet(LIGHTSENSE_SENSOR_PORT, 2U, gpioModeDisabled,0);

	  //MOTOR PINS
	  GPIO_PinModeSet(motor_input_port, motor_input_1_pin, motor_input_mode, 0);
	  GPIO_PinModeSet(motor_input_port, motor_input_2_pin, motor_input_mode, 0);
	  GPIO_PinModeSet(motor_input_port, motor_input_3_pin, motor_input_mode, 0);
	  GPIO_PinModeSet(motor_input_port, motor_input_4_pin, motor_input_mode, 0);
	  /* Initialize the 4 GPIO pins of the touch slider for using them as LESENSE
	   * scan channels for capacitive sensing. */
	  GPIO_PinModeSet(CAPLESENSE_SLIDER_PORT0, CAPLESENSE_SLIDER0_PIN, gpioModeDisabled, 0);
	  GPIO_PinModeSet(CAPLESENSE_SLIDER_PORT0, CAPLESENSE_SLIDER1_PIN, gpioModeDisabled, 0);
	  GPIO_PinModeSet(CAPLESENSE_SLIDER_PORT0, CAPLESENSE_SLIDER2_PIN, gpioModeDisabled, 0);
	  GPIO_PinModeSet(CAPLESENSE_SLIDER_PORT0, CAPLESENSE_SLIDER3_PIN, gpioModeDisabled, 0);

	  /* Enable GPIO_EVEN interrupt vector in NVIC. */
  //GPIO_PinModeSet(gpioPortD, 1, gpioModePushPull, 1);

  //  GPIO_PinOutSet(LEDPORT, LEDPIN0);// Added for uart BLE - because its GPIO is ON on initilization
  	  /* Enable GPIO_ODD interrupt vector in NVIC */
  	    NVIC_EnableIRQ(GPIO_ODD_IRQn);			//GPIO IRQ in i2c_tsl2651.c

}
