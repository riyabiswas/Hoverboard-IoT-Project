/*
 * definitions.h
 *
 *  Created on: Oct 28, 2016
 *      Authors: Praveen, Riya	
 */

#ifndef DEFINITIONS_H_
#define DEFINITIONS_H_

/** Status value used for showing the Energy Mode the device is currently in. */

#define EM0 0
#define EM1 1
#define EM2 2
#define EM3 3
#define COMPMAX 500

#define block_sleep_mode 2


//LESENSE

#define LIGHTSENSE_EXCITE_PORT   gpioPortD
#define LIGHTSENSE_EXCITE_PIN    6U
#define LIGHTSENSE_SENSOR_PORT   gpioPortC
#define LIGHTSENSE_SENSOR_PIN    6U
#define LEDPORT					 gpioPortE
#define LEDPIN0					 2U
#define LEDPIN1					 3U
#define LIGHTSENSE_NUMOF_EVENTS  5U

#define INIT_STATE_TIME_SEC      3U

//ULTRASONIC SENSOR

int ultrasonic_count;

//FSR FLAGS
int fsr_flag1,fsr_flag2;

//MOTOR PIN MAPS

#define motor_input_port gpioPortD
#define motor_input_mode gpioModePushPull

#define motor_input_1_pin 0U

#define motor_input_2_pin 1U

#define motor_input_3_pin 2U

#define motor_input_4_pin 3U


//DEFINTIONS FOR I2C PINS AND PORTS

#define i2c_sda_port gpioPortC
#define i2c_sda_pin 4U
#define i2c_sda_port_mode gpioModeWiredAnd

#define i2c_scl_port gpioPortC
#define i2c_scl_pin 5U
#define i2c_scl_port_mode gpioModeWiredAnd

#define i2c_int_mg_port gpioPortB
#define i2c_int_mg_pin 12U
#define i2c_int_mg_port_mode gpioModeInput
#define i2c_int_mg_rising_edge true
#define i2c_int_mg_falling_edge true
#define i2c_int_mg_enable true
#define i2c_int_mg_disable false

#define i2c_int_xl_port gpioPortD
#define i2c_int_xl_pin 5U
#define i2c_int_xl_port_mode gpioModeInput
#define i2c_int_xl_rising_edge true
#define i2c_int_xl_falling_edge true
#define i2c_int_xl_enable true
#define i2c_int_xl_disable false

#define ACCEL_I2C_ADDRESS 0X1DU
#define MAGNETOMETER_I2C_ADDRESS 0X1EU
#define wr_bit 0
#define rd_bit 1




#define i2c_int_port_mode gpioModeInput

#define i2c_int_rising_edge false
#define i2c_int_falling_edge true
#define i2c_int_enable true
#define i2c_int_disable false
#define i2c_location _I2C_ROUTE_LOCATION_LOC0
#define i2c_scl_enable I2C_ROUTE_SCLPEN
#define i2c_sda_enable I2C_ROUTE_SDAPEN


//DEFININTIONS FOR I2C INIT REGISTER
#define i2c_enable_init true
#define i2c_master_init true
#define i2c_refFreq_init 0
#define i2c_freq_init I2C_FREQ_STANDARD_MAX
#define i2c_clhr_init _I2C_CTRL_CLHR_STANDARD
#define I2C_EM EM1			//I2C ENERGY MODE REGISTER

//ACCEL REGISTER ADDRESSES

#define ACT_THS_A 0x1E
#define ACT_THS_A_VAL 0x00

#define CTRL_REG1_A 0X20
#define CTRL_REG1_A_XEN (0x01<<0)
#define CTRL_REG1_A_YEN (0x01<<1)
#define CTRL_REG1_A_ZEN (0x01<<2)
#define CTRL_REG1_A_BDU (0x01<<3)
#define CTRL_REG1_A_ODR0 (0x01<<4)
#define CTRL_REG1_A_ODR1 (0x01<<5)
#define CTRL_REG1_A_ODR2 (0x01<<6)
#define CTRL_REG1_A_HR (0x01<<7)
#define CTRL_REG1_A_CONFG CTRL_REG1_A_XEN|CTRL_REG1_A_YEN|CTRL_REG1_A_ZEN|CTRL_REG1_A_ODR1|CTRL_REG1_A_ODR2

#define CTRL_REG2_A 0X21
#define CTRL_REG2_A_HPIS1 (0x01<<0)
#define CTRL_REG2_A_HPIS2 (0x01<<1)
#define CTRL_REG2_A_FDS (0x01<<2)
#define CTRL_REG2_A_HPM0 (0x01<<3)
#define CTRL_REG2_A_HPM1 (0x01<<4)
#define CTRL_REG2_A_DFC0 (0x01<<5)
#define CTRL_REG2_A_DFC1 (0x01<<6)
#define CTRL_REG2_A_CONFG CTRL_REG2_A_HPM1|CTRL_REG2_A_DFC1|CTRL_REG2_A_DFC0

#define CTRL_REG3_A 0X22
#define CTRL_REG3_A_INT_XL_DRDY (0x01<<0)
#define CTRL_REG3_A_INT_XL_FTH (0x01<<1)
#define CTRL_REG3_A_INT_XL_OVR (0x01<<2)
#define CTRL_REG3_A_INT_XL_IG1 (0x01<<3)
#define CTRL_REG3_A_INT_XL_IG2 (0x01<<4)
#define CTRL_REG3_A_INT_XL_INACT (0x01<<5)
#define CTRL_REG3_A_STOP_FTH (0x01<<6)
#define CTRL_REG3_A_FIFO_EN (0x01<<7)
#define CTRL_REG3_A_CONFG	CTRL_REG3_A_INT_XL_IG1


#define CTRL_REG4_A 0X23
#define CTRL_REG4_A_SIM (0X01<<0)
#define CTRL_REG4_A_I2C_DISABLE (0X01<<1)
#define CTRL_REG4_A_IF_ADD_INC (0X01<<2)
#define CTRL_REG4_A_BW_SCALE_ODR (0X01<<3)
#define CTRL_REG4_A_FS0 (0X01<<4)
#define CTRL_REG4_A_FS1 (0X01<<5)
#define CTRL_REG4_A_BW1 (0X01<<6)
#define CTRL_REG4_A_BW2 (0X01<<7)
#define CTRL_REG4_A_CONFG	0X00

#define STATUS_REG_A 0x27

#define IG_CFG1_A 0x30
#define IG_CFG1_A_XLIE (0X01<<0)
#define IG_CFG1_A_XHIE (0X01<<1)
#define IG_CFG1_A_YLIE (0X01<<2)
#define IG_CFG1_A_YHIE (0X01<<3)
#define IG_CFG1_A_ZLIE (0X01<<4)
#define IG_CFG1_A_ZHIE (0X01<<5)
#define IG_CFG1_A_6D (0X01<<6)
#define IG_CFG1_A_AOI (0X01<<7)		// SET TO ENABLE AND INTERRUPTS
#define IG_CFG1_A_CONFG	IG_CFG1_A_XHIE|IG_CFG1_A_YLIE|IG_CFG1_A_ZHIE|IG_CFG1_A_AOI
//IG_CFG1_A_XHIE|IG_CFG1_A_YLIE|IG_CFG1_A_ZHIE|IG_CFG1_A_AOI|IG_CFG1_A_XLIE|IG_CFG1_A_ZHIE

#define IG_SRC1_A 0x31


#define CTRL_REG6_A 0x25
#define CTRL_REG6_A_BOOT (0x01<<7)

#define IG_THS_X1_A 0x32
#define IG_THS_X1_A_VAL 0x07		//3000/255

#define IG_THS_Y1_A 0x33
#define IG_THS_Y1_A_VAL 0x07		//300/255

#define IG_THS_Z1_A 0x34
#define IG_THS_Z1_A_VAL 0x03		//1500/255

#define IG_DUR1_A 0x35
#define IG_DUR1_A_VAL 0x00

#define ACCEL_X_LSB 0x28
#define ACCEL_X_MSB 0x29
#define ACCEL_Y_LSB 0x2A
#define ACCEL_Y_MSB 0x2B
#define ACCEL_Z_LSB 0x2C
#define ACCEL_Z_MSB 0x2D
#define WHO_AM_I 0x0F


//MAGNETOMETER REGISTER ADDRESSES
#define CTRL_REG1_M 0X20
#define CTRL_REG1_M_ST (0X01<<0)
#define CTRL_REG1_M_DO0 (0X01<<2)
#define CTRL_REG1_M_DO1 (0X01<<3)
#define CTRL_REG1_M_DO2 (0X01<<4)
#define CTRL_REG1_M_OM0 (0X01<<5)
#define CTRL_REG1_M_OM1 (0X01<<6)
#define CTRL_REG1_M_TEMP_EN (0X01<<7)

#define CTRL_REG1_M_CONFG		CTRL_REG1_M_DO0|CTRL_REG1_M_DO1|CTRL_REG1_M_DO2|CTRL_REG1_M_OM1|CTRL_REG1_M_OM0|CTRL_REG1_M_TEMP_EN

#define CTRL_REG2_M 0X21
#define CTRL_REG2_M_SOFT_RST (0X01<<2)
#define CTRL_REG2_M_REBOOT   (0X01<<3)
#define CTRL_REG2_M_FS0		 (0X01<<5)
#define CTRL_REG2_M_FS1		 (0X01<<6)

#define CTRL_REG2_M_CONFG	CTRL_REG2_M_FS0|CTRL_REG2_M_FS1

#define CTRL_REG3_M 0X22
#define CTRL_REG3_M_MD0  (0X01<<0)
#define CTRL_REG3_M_MD1	 (0X01<<1)
#define CTRL_REG3_M_SIM  (0X01<<2)
#define CTRL_REG3_M_LP   (0X01<<5)
#define CTRL_REG3_M_I2C_DISABLE (0X01<<7)

#define CTRL_REG3_M_CONFG CTRL_REG3_M_LP

#define CTRL_REG4_M 0X23
#define CTRL_REG4_M_BLE (0X01<<1)
#define CTRL_REG4_M_OMZ0 (0X01<<2)
#define CTRL_REG4_M_OMZ1 (0X01<<3)

#define CTRL_REG4_M_CONFG CTRL_REG4_M_OMZ1

#define CTRL_REG5_M 0X24
#define CTRL_REG5_M_CONFG 0X00

#define MAGNETOMETER_X_LSB 0x28
#define MAGNETOMETER_X_MSB 0x29
#define MAGNETOMETER_Y_LSB 0x2A
#define MAGNETOMETER_Y_MSB 0x2B
#define MAGNETOMETER_Z_LSB 0x2C
#define MAGNETOMETER_Z_MSB 0x2D

#define INT_CFG_M 0X30
#define INT_CFG_M_IEN (0X01<<0)
#define INT_CFG_M_IEL (0X01<<1)
#define INT_CFG_M_IEA (0X01<<2)
#define INT_CFG_M_ZIEN (0X01<<5)
#define INT_CFG_M_YIEN (0X01<<6)
#define INT_CFG_M_XIEN (0X01<<7)

#define INT_CFG_M_CONFG INT_CFG_M_XIEN|INT_CFG_M_IEA|INT_CFG_M_IEN|INT_CFG_M_IEL|INT_CFG_M_YIEN|INT_CFG_M_ZIEN
//#define INT_CFG_M_CONFG 0XFF

#define INT_THS_L_M 0X32
#define INT_THS_L_M_CONFG 0X00

#define INT_THS_H_M 0X33
#define INT_THS_H_M_CONFG 0XFF

#define MAG_TEMPL 0x2E
#define MAG_TEMPH 0x2F
float temp;
unsigned int temph,templ,tempr;
//UART DEFITIONS
#define LEUART0_PORT gpioPortD
#define LEUART0_PIN 4
#define LEUART0_MODE gpioModePushPull
#define LEUART0_STATE 1
#define LEUART0_REF_FREQ 0
#define LEUART0_BAUD 9600
#define LEUART0_DATABITS leuartDatabits8
#define LEUART0_PARITY leuartNoParity
#define LEUART0_STOP_BITS leuartStopbits1

#define buffer_length 100
char circular_buffer[buffer_length];
int read_pntr;
int write_pntr;
int glbl;
int glbl_write;

//DEFINITIONS FOR ADC
#define ADC_SAMPLES 1
int adc_complete;
#define ADC_EM EM1				//Energy mode of ADC is EM1
#define CAL_TEMP_0 0x0FE081B2
#define ADC0_TEMP_0_READ_1V25 0x0FE081BE
#define adc_channel ADC0
#define temperature_pin adcSingleInputTemp
#define int_temp_reference adcRef1V25
#define adc_resolution adcRes12Bit
#define adc_acquire_time adcAcqTime4
#define ADC0_rep true
#define ADC0_warmup adcWarmupNormal
#define adc_clk 1300000
#define ADC0_DMA_Arbitration dmaArbitrate1
#define TEMP_Sensor_DMA true


#endif /* DEFINITIONS_H_ */
