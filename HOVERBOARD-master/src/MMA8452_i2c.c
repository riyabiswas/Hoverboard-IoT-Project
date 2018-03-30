/*
 * MMA8452_i2c.c
 *
 *  Created on: Oct 28, 2016
 *      Authors: Praveen
 *      		Riya
 */


#include "MMA8452_i2c.h"
#include "DMASetup.h"
unsigned int datax_xl,data1,data2,val1,val2,val_0,val_1=0,i2c_int_flag,datay_xl,dataz_xl,datax,datay,dataz,datax_mg,datay_mg,dataz_mg,data;
int new_data,look_for_30,look_for_15,look_for_45,look_for_n15,look_for_n30,look_for_n45,look_for_0;


void load_poweron()
{
	CMU_ClockEnable(cmuClock_I2C1,true);

	GPIO_PinModeSet(i2c_int_xl_port, i2c_int_xl_pin, i2c_int_xl_port_mode, 1);
	GPIO_IntConfig(i2c_int_xl_port, i2c_int_xl_pin,  i2c_int_xl_rising_edge, i2c_int_xl_falling_edge, i2c_int_xl_enable);

//	GPIO_PinModeSet(i2c_int_mg_port, i2c_int_mg_pin, i2c_int_mg_port_mode, 1);
//	GPIO_IntConfig(i2c_int_mg_port, i2c_int_mg_pin,  i2c_int_mg_rising_edge, i2c_int_mg_falling_edge, i2c_int_mg_enable);


	GPIO_PinModeSet(i2c_scl_port, i2c_scl_pin, i2c_scl_port_mode, 1);
	GPIO_PinModeSet(i2c_sda_port, i2c_sda_pin, i2c_sda_port_mode, 1);

	I2C1->ROUTE = i2c_sda_enable | i2c_scl_enable | (i2c_location);

		 const I2C_Init_TypeDef i2cInit =
		 {
				 .enable= i2c_enable_init,
				 .master=i2c_master_init,
				 .refFreq=i2c_refFreq_init,
				 .freq=i2c_freq_init,
				 .clhr=i2c_clhr_init,
		 };
		 I2C_Init(I2C1, &i2cInit);
}

void load_poweroff()
{
//	GPIO_IntConfig(i2c_int_port, i2c_int_pin, i2c_int_rising_edge, i2c_int_falling_edge, i2c_int_disable);
	GPIO_PinModeSet(i2c_scl_port, i2c_scl_pin, i2c_scl_port_mode, 0);
	GPIO_PinModeSet(i2c_sda_port, i2c_sda_pin, i2c_sda_port_mode, 0);
	CMU_ClockEnable(cmuClock_I2C1,false);
}


void i2c_magnetometer_write()
{
	I2C1->TXDATA = (MAGNETOMETER_I2C_ADDRESS<<1)|wr_bit;
		   I2C1->CMD=I2C_CMD_START;
		   while((I2C1->IF & I2C_IF_ACK)==0);
}

void i2c_accel_write()
{
	I2C1->TXDATA = (ACCEL_I2C_ADDRESS<<1)|wr_bit;
		   I2C1->CMD=I2C_CMD_START;
		   while((I2C1->IF & I2C_IF_ACK)==0);
}

void i2c_wr_to_accel_reg(uint16_t reg,uint16_t val)
{
	i2c_accel_write();
	     I2C1->IFC=I2C_IFC_ACK;

	     I2C1->TXDATA = (reg);
	     while((I2C1->IF & I2C_IF_ACK)==0);
		I2C1->IFC=I2C_IFC_ACK;

		I2C1->TXDATA = (val);
		while((I2C1->IF & I2C_IF_ACK)==0);
			I2C1->IFC=I2C_IFC_ACK;

			I2C1->CMD=I2C_CMD_STOP;
}
void i2c_wr_to_magnetometer_reg(uint16_t reg,uint16_t val)
{
	i2c_magnetometer_write();
	     I2C1->IFC=I2C_IFC_ACK;

	     I2C1->TXDATA = (reg);
	     while((I2C1->IF & I2C_IF_ACK)==0);
		I2C1->IFC=I2C_IFC_ACK;

		I2C1->TXDATA = (val);
		while((I2C1->IF & I2C_IF_ACK)==0);
			I2C1->IFC=I2C_IFC_ACK;

			I2C1->CMD=I2C_CMD_STOP;
}
uint32_t i2c_rd_from_ACCEL_reg(uint8_t regi)
{
	data=0;
	i2c_accel_write();
			     I2C1->IFC=I2C_IFC_ACK;

	   I2C1->TXDATA = (regi);
	   while((I2C1->IF & I2C_IF_ACK)==0);
	   I2C1->IFC=I2C_IFC_ACK;

	   I2C1->CMD=I2C_CMD_START;	//START EXECUTED ONCE FOR REPEATED START
	   I2C1->TXDATA = (ACCEL_I2C_ADDRESS<<1)|rd_bit;
	   while((I2C1->IF & I2C_IF_ACK)==0);
	   I2C1->IFC=I2C_IFC_ACK;

	   //I2C1->CMD=I2C_CMD_ACK;
	   while((I2C1->STATUS & I2C_STATUS_RXDATAV)==0);
	   data=I2C1->RXDATA;

	   I2C1->CMD=I2C_CMD_NACK;
	   I2C1->CMD=I2C_CMD_STOP;

	   return data;
}

uint32_t i2c_rd_from_MAGNETOMETER_reg(uint8_t regi)
{
	data=0;
	i2c_magnetometer_write();
		 I2C1->IFC=I2C_IFC_ACK;

	   I2C1->TXDATA = (regi);
	   while((I2C1->IF & I2C_IF_ACK)==0);
	   I2C1->IFC=I2C_IFC_ACK;

	   I2C1->CMD=I2C_CMD_START;	//START EXECUTED ONCE FOR REPEATED START
	   I2C1->TXDATA = (MAGNETOMETER_I2C_ADDRESS<<1)|rd_bit;
	   while((I2C1->IF & I2C_IF_ACK)==0);
	   I2C1->IFC=I2C_IFC_ACK;
	   while((I2C1->STATUS & I2C_STATUS_RXDATAV)==0);
	   data=I2C1->RXDATA;

	   I2C1->CMD=I2C_CMD_NACK;
	   I2C1->CMD=I2C_CMD_STOP;

	   return data;
}
void i2c_reset(void)
{
	if(I2C1->STATE & I2C_STATE_BUSY){
		 I2C1->CMD = I2C_CMD_ABORT;
	 }

	 /* In some situations (after a reset during an I2C transfer), the slave */
	   /* device may be left in an unknown state. Send 9 clock pulses just in case. */
	   for (int j = 0; j < 9; j++)
	   {
	     /*
	      * TBD: Seems to be clocking at appr 80kHz-120kHz depending on compiler
	      * optimization when running at 14MHz. A bit high for standard mode devices,
	      * but DVK only has fast mode devices. Need however to add some time
	      * measurement in order to not be dependable on frequency and code executed.
	      */
	     GPIO_PinModeSet(i2c_scl_port, i2c_scl_pin, i2c_scl_port_mode, 0);
	     GPIO_PinModeSet(i2c_scl_port, i2c_scl_pin, i2c_scl_port_mode, 1);
	   }
	   for(int j=0;j<1500;j++){}//ADDED TO REMOVE STRAY CAP ON I2C PINS WHILE GPIO POWER UP
}

void i2c_setup(void)
{
	load_poweron();
	i2c_reset();

	//MAGNETOMETER CONFIG

	i2c_wr_to_magnetometer_reg(CTRL_REG1_M,CTRL_REG1_M_CONFG);
	i2c_wr_to_magnetometer_reg(CTRL_REG2_M,CTRL_REG2_M_CONFG);
	i2c_wr_to_magnetometer_reg(CTRL_REG5_M,CTRL_REG5_M_CONFG);

	i2c_wr_to_magnetometer_reg(CTRL_REG4_M,CTRL_REG4_M_CONFG);

	i2c_wr_to_magnetometer_reg(CTRL_REG3_M,CTRL_REG3_M_CONFG);


	//ACCELEROMETER CONFIGURATION
    i2c_wr_to_accel_reg(CTRL_REG6_A,CTRL_REG6_A_BOOT);

	i2c_wr_to_accel_reg(CTRL_REG1_A, CTRL_REG1_A_CONFG);
	i2c_wr_to_accel_reg(CTRL_REG2_A, CTRL_REG2_A_CONFG);
	i2c_wr_to_accel_reg(CTRL_REG4_A, CTRL_REG4_A_CONFG);
	//i2c_wr_to_accel_reg(ACT_THS_A,ACT_THS_A_VAL);

    i2c_wr_to_accel_reg(IG_THS_X1_A,IG_THS_X1_A_VAL);
	i2c_wr_to_accel_reg(IG_THS_Y1_A,IG_THS_Y1_A_VAL);
	i2c_wr_to_accel_reg(IG_THS_Z1_A,IG_THS_Z1_A_VAL);
//	i2c_wr_to_accel_reg(IG_DUR1_A,IG_DUR1_A_VAL);

	i2c_wr_to_accel_reg(IG_CFG1_A, IG_CFG1_A_CONFG);
    i2c_wr_to_accel_reg(CTRL_REG3_A, CTRL_REG3_A_CONFG);
    look_for_0=1;


   while(1)
   {
		temph=i2c_rd_from_MAGNETOMETER_reg(MAG_TEMPH);
		templ=i2c_rd_from_MAGNETOMETER_reg(MAG_TEMPL);
		temp=(temph<<8)+(templ);
		temp=temp/8;
   }
//	   data1=i2c_rd_from_ACCEL_reg(ACCEL_X_LSB);
//	   data2=i2c_rd_from_ACCEL_reg(ACCEL_X_MSB);
//	   datax_xl=(data2<<8)+data1;
//
//
//	   data1=i2c_rd_from_ACCEL_reg(ACCEL_Y_LSB);
//	   data2=i2c_rd_from_ACCEL_reg(ACCEL_Y_MSB);
//	   datay_xl=(data2<<8)+data1;
//
//
//	   data1=i2c_rd_from_ACCEL_reg(ACCEL_Z_LSB);
//	   data2=i2c_rd_from_ACCEL_reg(ACCEL_Z_MSB);
//	   dataz_xl=(data2<<8)+data1;

//	   datax=(datax_xl*datax_xl)+(datay_xl*datay_xl)+(dataz_xl*dataz_xl);

//	   new_data=i2c_rd_from_MAGNETOMETER_reg(0X27);
//	   while((new_data & 0x01) !=1)
//	   new_data=i2c_rd_from_MAGNETOMETER_reg(0x27);
//	   data1=i2c_rd_from_MAGNETOMETER_reg(MAGNETOMETER_Z_LSB);
//	   data2=i2c_rd_from_MAGNETOMETER_reg(MAGNETOMETER_Z_MSB);
//	   dataz_mg=(data2<<8)+data1;
//	   if((dataz_mg & (1 << 17)) != 0)
//		   {
//			  dataz_mg = dataz_mg | ~((1 << 18) - 1);
//		   }
//
//	   data1=i2c_rd_from_MAGNETOMETER_reg(MAGNETOMETER_X_LSB);
//	   data2=i2c_rd_from_MAGNETOMETER_reg(MAGNETOMETER_X_MSB);
//	   datax_mg=(data2<<8)+data1;
//	   if((datax_mg & (1 << 17)) != 0)
//	   	   {
//			  datax_mg = datax_mg | ~((1 << 18) - 1);
//	   	   }
//
////	   new_data=i2c_rd_from_MAGNETOMETER_reg(0x27);
////	   while((new_data & 0x02) !=1)
////	   new_data=i2c_rd_from_MAGNETOMETER_reg(0x27);
//
//	   data1=i2c_rd_from_MAGNETOMETER_reg(MAGNETOMETER_Y_LSB);
//	   data2=i2c_rd_from_MAGNETOMETER_reg(MAGNETOMETER_Y_MSB);
//	   datay_mg=(data2<<8)+data1;
//	   if((datay_mg & (1 << 17)) != 0)
//		   {
//			  datay_mg = datay_mg | ~((1 << 18) - 1);
//		   }

//	   new_data=i2c_rd_from_MAGNETOMETER_reg(0x27);
//	   while((new_data & 0x04) !=1)
//	   new_data=i2c_rd_from_MAGNETOMETER_reg(0x27);



//	   datax=datax_xl-datax_mg;
//	   datay=datay_xl-datay_mg;
//	   dataz=dataz_xl-dataz_mg;
//   }
}

void GPIO_ODD_IRQHandler(void)
{
	INT_Disable();
	GPIO_IntClear(0x02);
	GPIO_IntConfig(i2c_int_xl_port, i2c_int_xl_pin,  i2c_int_xl_rising_edge, i2c_int_xl_falling_edge, i2c_int_xl_disable);
	GPIO_IntConfig(i2c_int_xl_port, i2c_int_xl_pin,  i2c_int_xl_rising_edge, i2c_int_xl_falling_edge, i2c_int_xl_enable);
	data1=i2c_rd_from_ACCEL_reg(ACCEL_X_LSB);
	data2=i2c_rd_from_ACCEL_reg(ACCEL_X_MSB);
	datax_xl=(data2<<8)+data1;

	data1=i2c_rd_from_ACCEL_reg(ACCEL_Y_LSB);
	data2=i2c_rd_from_ACCEL_reg(ACCEL_Y_MSB);
	datay_xl=(data2<<8)+data1;

	data1=i2c_rd_from_ACCEL_reg(ACCEL_Z_LSB);
	data2=i2c_rd_from_ACCEL_reg(ACCEL_Z_MSB);
	dataz_xl=(data2<<8)+data1;

//	data1=i2c_rd_from_MAGNETOMETER_reg(MAGNETOMETER_Z_LSB);
//	data2=i2c_rd_from_MAGNETOMETER_reg(MAGNETOMETER_Z_MSB);
//	dataz_mg=(data2<<8)+data1;
//
//	data1=i2c_rd_from_MAGNETOMETER_reg(MAGNETOMETER_X_LSB);
//	data2=i2c_rd_from_MAGNETOMETER_reg(MAGNETOMETER_X_MSB);
//	datax_mg=(data2<<8)+data1;
//
//	data1=i2c_rd_from_MAGNETOMETER_reg(MAGNETOMETER_Y_LSB);
//	data2=i2c_rd_from_MAGNETOMETER_reg(MAGNETOMETER_Y_MSB);
//	datay_mg=(data2<<8)+data1;


	//0 condition
	if((((0<datax_xl)&&(datax_xl<63000))||((datax_xl>0)&&(datax_xl<3000))) && ((0<datay_xl)&&(datay_xl<65535)) && ((7000<dataz_xl)&&(dataz_xl<16500))  && look_for_0==1)
	{
		LETIMER_CompareSet(LETIMER0, 1, 0);
		GPIO_PinModeSet(motor_input_port, motor_input_1_pin, motor_input_port, 1);
	   GPIO_PinModeSet(motor_input_port, motor_input_4_pin, motor_input_port, 1);
		GPIO_PinModeSet(motor_input_port, motor_input_2_pin, motor_input_port, 1);
	   GPIO_PinModeSet(motor_input_port, motor_input_3_pin, motor_input_port, 1);
		new_data=0;
		look_for_0=0;
		look_for_15=1;
		look_for_n15=1;
		circular_buffer[write_pntr]='P';
		write_pntr=(write_pntr+1)%buffer_length;
		circular_buffer[write_pntr]='+';
		write_pntr=(write_pntr+1)%buffer_length;
		circular_buffer[write_pntr]=0;
		write_pntr=(write_pntr+1)%buffer_length;
		circular_buffer[write_pntr]=0;
		write_pntr=(write_pntr+1)%buffer_length;

		CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFRCO);
		LEUART0->IEN|=LEUART_IEN_TXBL;
	}
	//+15 condition

	if(((3000<datax_xl)&&(datax_xl<6000)) && ((0<datay_xl)&&(datay_xl<65535)) && ((14300<dataz_xl)&&(dataz_xl<17000)) && look_for_15==1)
	{
		LETIMER_CompareSet(LETIMER0, 1, COMPMAX/4);
		if(fsr_flag1==1){
		GPIO_PinModeSet(motor_input_port, motor_input_1_pin, motor_input_port, 0);
		 GPIO_PinModeSet(motor_input_port, motor_input_4_pin, motor_input_mode, 1);
	}
		 else
		 {
			 GPIO_PinModeSet(motor_input_port, motor_input_1_pin, motor_input_mode,1);
			 		 GPIO_PinModeSet(motor_input_port, motor_input_4_pin, motor_input_mode, 1);
		 }
if(fsr_flag2==1){
				GPIO_PinModeSet(motor_input_port, motor_input_2_pin, motor_input_mode, 0);
				 GPIO_PinModeSet(motor_input_port, motor_input_3_pin, motor_input_mode, 1);
				 	}
		 else
		 {
				GPIO_PinModeSet(motor_input_port, motor_input_2_pin, motor_input_mode, 1);
				 GPIO_PinModeSet(motor_input_port, motor_input_3_pin, motor_input_mode, 1);

		 }
		new_data=15;
		look_for_30=1;
		look_for_15=0;
		look_for_0=1;
		circular_buffer[write_pntr]='P';
		write_pntr=(write_pntr+1)%buffer_length;
		circular_buffer[write_pntr]='+';
		write_pntr=(write_pntr+1)%buffer_length;
		circular_buffer[write_pntr]=1;
		write_pntr=(write_pntr+1)%buffer_length;
		circular_buffer[write_pntr]=5;
		write_pntr=(write_pntr+1)%buffer_length;
		CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFRCO);
		LEUART0->IEN|=LEUART_IEN_TXBL;

	}
	//+30 condition
	if(((6000<datax_xl)&&(datax_xl<10000)) && ((0<datay_xl)&&(datay_xl<65535)) && ((13000<dataz_xl)&&(dataz_xl<16000)) && (look_for_30==1))
	{

		LETIMER_CompareSet(LETIMER0, 1, COMPMAX/2);
		if(fsr_flag1==1){
		GPIO_PinModeSet(motor_input_port, motor_input_1_pin, motor_input_mode, 0);
		 GPIO_PinModeSet(motor_input_port, motor_input_4_pin, motor_input_mode, 1);
	}
		 else
		 {
			 GPIO_PinModeSet(motor_input_port, motor_input_1_pin, motor_input_mode, 1);
			 		 GPIO_PinModeSet(motor_input_port, motor_input_4_pin, motor_input_mode, 1);
		 }
if(fsr_flag2==1){
				GPIO_PinModeSet(motor_input_port, motor_input_2_pin, motor_input_mode, 0);
				 GPIO_PinModeSet(motor_input_port, motor_input_3_pin, motor_input_mode, 1);
				 	}
		 else
		 {
				GPIO_PinModeSet(motor_input_port, motor_input_2_pin, motor_input_mode, 1);
				 GPIO_PinModeSet(motor_input_port, motor_input_3_pin, motor_input_mode, 1);

		 }
		new_data=30;
		look_for_30=0;
		look_for_45=1;
		look_for_15=1;
		circular_buffer[write_pntr]='P';
		write_pntr=(write_pntr+1)%buffer_length;
		circular_buffer[write_pntr]='+';
		write_pntr=(write_pntr+1)%buffer_length;
		circular_buffer[write_pntr]=3;
		write_pntr=(write_pntr+1)%buffer_length;
		circular_buffer[write_pntr]=0;
		write_pntr=(write_pntr+1)%buffer_length;
		CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFRCO);
		LEUART0->IEN|=LEUART_IEN_TXBL;

	}
	//+45 condition
	if(((7900<datax_xl)&&(datax_xl<14000)) && ((0<datay_xl)&&(datay_xl<65535)) && ((10000<dataz_xl)&&(dataz_xl<13000)) && (look_for_45==1))
	{
		LETIMER_CompareSet(LETIMER0, 1, COMPMAX);
			if(fsr_flag1==1){
			GPIO_PinModeSet(motor_input_port, motor_input_1_pin, motor_input_mode, 0);
			 GPIO_PinModeSet(motor_input_port, motor_input_4_pin, motor_input_mode, 1);
		}
			 else
			 {
				 GPIO_PinModeSet(motor_input_port, motor_input_1_pin, motor_input_mode, 1);
				 		 GPIO_PinModeSet(motor_input_port, motor_input_4_pin, motor_input_mode, 1);
			 }
	if(fsr_flag2==1){
					GPIO_PinModeSet(motor_input_port, motor_input_2_pin, motor_input_mode, 0);
					 GPIO_PinModeSet(motor_input_port, motor_input_3_pin, motor_input_mode, 1);
					 	}
			 else
			 {
					GPIO_PinModeSet(motor_input_port, motor_input_2_pin, motor_input_mode, 1);
					 GPIO_PinModeSet(motor_input_port, motor_input_3_pin, motor_input_mode, 1);

			 }

		new_data=45;
		look_for_45=0;
		look_for_30=1;
		circular_buffer[write_pntr]='P';
		write_pntr=(write_pntr+1)%buffer_length;
		circular_buffer[write_pntr]='+';
		write_pntr=(write_pntr+1)%buffer_length;
		circular_buffer[write_pntr]=4;
		write_pntr=(write_pntr+1)%buffer_length;
		circular_buffer[write_pntr]=5;
		write_pntr=(write_pntr+1)%buffer_length;
		CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFRCO);
		LEUART0->IEN|=LEUART_IEN_TXBL;

	}
	//-15 condition
	if(((59000<datax_xl)&&(datax_xl<63000)) && ((0<datay_xl)&&(datay_xl<65535)) && ((14500<dataz_xl)&&(dataz_xl<15900))  && look_for_n15==1)
	{
		//change direction
		LETIMER_CompareSet(LETIMER0, 1, COMPMAX/4);
		if(fsr_flag1==1){
		GPIO_PinModeSet(motor_input_port, motor_input_1_pin, motor_input_mode, 1);
		 GPIO_PinModeSet(motor_input_port, motor_input_4_pin, motor_input_mode, 0);
	}
		 else
		 {
			 GPIO_PinModeSet(motor_input_port, motor_input_1_pin, motor_input_mode, 1);
			 		 GPIO_PinModeSet(motor_input_port, motor_input_4_pin, motor_input_mode, 1);
		 }
if(fsr_flag2==1){
				GPIO_PinModeSet(motor_input_port, motor_input_2_pin, motor_input_mode, 1);
				 GPIO_PinModeSet(motor_input_port, motor_input_3_pin, motor_input_mode, 0);
				 	}
		 else
		 {
				GPIO_PinModeSet(motor_input_port, motor_input_2_pin, motor_input_mode, 1);
				 GPIO_PinModeSet(motor_input_port, motor_input_3_pin, motor_input_mode, 1);

		 }
		new_data=-15;
		look_for_n15=0;
		look_for_n45=0;
		look_for_15=0;
		look_for_n30=1;
		look_for_0=1;
		circular_buffer[write_pntr]='P';
		write_pntr=(write_pntr+1)%buffer_length;
		circular_buffer[write_pntr]='-';
		write_pntr=(write_pntr+1)%buffer_length;
		circular_buffer[write_pntr]=1;
		write_pntr=(write_pntr+1)%buffer_length;
		circular_buffer[write_pntr]=5;
		write_pntr=(write_pntr+1)%buffer_length;
		CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFRCO);
		LEUART0->IEN|=LEUART_IEN_TXBL;
	}
	//-30 condition
	if(((53000<datax_xl)&&(datax_xl<65000)) && ((0<datay_xl)&&(datay_xl<65535)) && ((10000<dataz_xl)&&(dataz_xl<15900))  && look_for_n30==1)
	{
		//change direction
		LETIMER_CompareSet(LETIMER0, 1, COMPMAX/2);
		if(fsr_flag1==1){
		GPIO_PinModeSet(motor_input_port, motor_input_1_pin, motor_input_mode, 1);
		 GPIO_PinModeSet(motor_input_port, motor_input_4_pin, motor_input_mode, 0);
	}
		 else
		 {
			 GPIO_PinModeSet(motor_input_port, motor_input_1_pin, motor_input_mode, 1);
			 		 GPIO_PinModeSet(motor_input_port, motor_input_4_pin, motor_input_mode, 1);
		 }
if(fsr_flag2==1){
				GPIO_PinModeSet(motor_input_port, motor_input_2_pin, motor_input_mode, 1);
				 GPIO_PinModeSet(motor_input_port, motor_input_3_pin, motor_input_mode, 0);
				 	}
		 else
		 {
				GPIO_PinModeSet(motor_input_port, motor_input_2_pin, motor_input_mode, 1);
				 GPIO_PinModeSet(motor_input_port, motor_input_3_pin, motor_input_mode, 1);

		 }
		new_data=-30;
		look_for_n30=0;
		look_for_0=0;
		look_for_n15=1;
		look_for_n45=1;
		circular_buffer[write_pntr]='P';
		write_pntr=(write_pntr+1)%buffer_length;
		circular_buffer[write_pntr]='-';
		write_pntr=(write_pntr+1)%buffer_length;
		circular_buffer[write_pntr]=3;
		write_pntr=(write_pntr+1)%buffer_length;
		circular_buffer[write_pntr]=0;
		write_pntr=(write_pntr+1)%buffer_length;
		CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFRCO);
		LEUART0->IEN|=LEUART_IEN_TXBL;
	}
	//-45 condition
	if(((52000<datax_xl)&&(datax_xl<55000)) && ((0<datay_xl)&&(datay_xl<65535)) && ((8000<dataz_xl)&&(dataz_xl<13000)) && look_for_n45==1)
	{
		//change direction
		LETIMER_CompareSet(LETIMER0, 1, COMPMAX);
		if(fsr_flag1==1){
		GPIO_PinModeSet(motor_input_port, motor_input_1_pin, motor_input_mode, 1);
		 GPIO_PinModeSet(motor_input_port, motor_input_4_pin, motor_input_mode, 0);
	}
		 else
		 {
			 GPIO_PinModeSet(motor_input_port, motor_input_1_pin, motor_input_mode, 1);
			 		 GPIO_PinModeSet(motor_input_port, motor_input_4_pin, motor_input_mode, 1);
		 }
if(fsr_flag2==1){
				GPIO_PinModeSet(motor_input_port, motor_input_2_pin, motor_input_mode, 1);
				GPIO_PinModeSet(motor_input_port, motor_input_3_pin, motor_input_mode, 0);
		}
		 else
		 {
				GPIO_PinModeSet(motor_input_port, motor_input_2_pin, motor_input_mode, 1);
				GPIO_PinModeSet(motor_input_port, motor_input_3_pin, motor_input_mode, 1);

		 }
		new_data=-45;
		look_for_n45=0;
		look_for_n30=1;
		look_for_0=0;
		circular_buffer[write_pntr]='P';
		write_pntr=(write_pntr+1)%buffer_length;
		circular_buffer[write_pntr]='-';
		write_pntr=(write_pntr+1)%buffer_length;
		circular_buffer[write_pntr]=4;
		write_pntr=(write_pntr+1)%buffer_length;
		circular_buffer[write_pntr]=5;
		write_pntr=(write_pntr+1)%buffer_length;
		CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFRCO);
		LEUART0->IEN|=LEUART_IEN_TXBL;

	}
//		templ=i2c_rd_from_MAGNETOMETER_reg(MAG_TEMPL);
//		temph=i2c_rd_from_MAGNETOMETER_reg(MAG_TEMPH);
//		temp=(temph<<8)+templ;
		temp=35.5;
//		temp=templ;
		circular_buffer[write_pntr]='T';
		write_pntr=(write_pntr+1)%buffer_length;

		if(temp>=0)
		circular_buffer[write_pntr]='+';
		else{
		circular_buffer[write_pntr]='-';
		temp=temp*(-1);		//convert temp to positive
		}
		write_pntr=(write_pntr+1)%buffer_length;

		int first=temp;
		first=first*100;
		float second=temp;
		second=second*100;
		second=second-first;
		first=temp;

		circular_buffer[write_pntr]=first;
		write_pntr=(write_pntr+1)%buffer_length;
		circular_buffer[write_pntr]=second;
		write_pntr=(write_pntr+1)%buffer_length;

		LEUART0->IEN|=LEUART_IEN_TXBL;



  INT_Enable();
}

