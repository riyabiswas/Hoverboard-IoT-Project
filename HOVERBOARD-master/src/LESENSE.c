/*
 * LESENSE.c
 *
 *  Created on: Nov 4, 2016
 *      Author: Praveen
 *      		Riya
 */

#include "LESENSE.h"
#include "lightsenseconfig.h"
static const bool channelsInUse[LESENSE_CHANNELS] = LESENSE_CAPSENSE_CH_IN_USE;
int cap_pressed=0;
/* LESENSE alternate excitation channel configuration constant table. */
static const LESENSE_ConfAltEx_TypeDef initAltEx = LESENSE_LIGHTSENSE_ALTEX_CONF;
///* LESENSE channel configuration constant table in sense mode. */
static const LESENSE_ChAll_TypeDef initChsSense = LESENSE_CAPSENSE_SCAN_CONF_SENSE;
static const LESENSE_ChAll_TypeDef initChsSleep = LESENSE_CAPSENSE_SCAN_CONF_SLEEP;
const char *message = "CAPSENSE TOUCH DEMO  ";
int system_on=1;
/**************************************************************************//**
 * Local variables
 *****************************************************************************/
/** Callback function for LESENSE interrupts. */
static void (*lesenseScanCb)(void);
/** Callback function for LESENSE interrupts. */
static void (*lesenseChCb)(void);
/* Type definition for global state. */
typedef enum
{
  MODE0 = 0,
  MODE1 = 1
} LIGHTSENSE_GlobalMode_TypeDef;
/* Type definition for global state. */
typedef enum
{
  ERROR_STATE = -1,
  INIT_STATE = 0,
  TIMER_RESET_STATE = 1,
  AWAKE_STATE = 2,
  SENSE_PREPARE_STATE = 3,
  SENSE_STATE = 4,
  BUTTON0_PRESS_STATE = 5
} LIGHTSENSE_GlobalState_TypeDef;

/**************************************************************************//**
 * Global variables
 *****************************************************************************/
static volatile LIGHTSENSE_GlobalMode_TypeDef appModeGlobal = MODE0;
static volatile LIGHTSENSE_GlobalState_TypeDef appStateGlobal = INIT_STATE;
static volatile bool secTimerFired = false;
static volatile uint8_t eventCounterlight = 0U;
static volatile uint8_t eventCounterfsr1 = 0U;
static volatile uint8_t eventCounterfsr2 = 0U;
/** The current channel we are sensing */
static volatile uint8_t currentChannel;


int inc=0;

/* Enable or disable vboost */
bool vboost = false;
typedef enum
{
  DEMO_ERROR = -1,
  DEMO_SLEEP = 0,
  DEMO_SENSE = 1,
  DEMO_SLEEP_PREPARE = 2,
  DEMO_SENSE_PREPARE = 3
} DEMO_States_TypeDef;

#define DEMOMODES           3

#define DEMOMODE_SCROLLTEXT 0
#define DEMOMODE_BARS       1
#define DEMOMODE_VALUES     2
void capSenseTimerFired( RTCDRV_TimerID_t id, void *user);
/** Timer used for bringing the system into sleep. */
RTCDRV_TimerID_t xTimerForSleep;
static volatile DEMO_States_TypeDef demoState = DEMO_SLEEP_PREPARE;
volatile uint32_t demoMode;
volatile uint32_t subMode;
volatile uint32_t subModes;

/**************************************************************************//**
 * @brief  Callback for sensor scan complete.
 *****************************************************************************/
void capSenseScanComplete(void)
{
	demoState = DEMO_SENSE_PREPARE;
}

/**************************************************************************//**
 * @brief  Callback for sensor channel triggered.
 *****************************************************************************/
void capSenseChTrigger(void)
{
  demoState = DEMO_SENSE_PREPARE;
}
/**************************************************************************//**
 * @brief  Callback for timer overflow.
 *****************************************************************************/
void capSenseTimerFired( RTCDRV_TimerID_t id, void *user)
{
  ( void)id;
  ( void)user;
  demoState = DEMO_SLEEP_PREPARE;
}
static volatile uint32_t channelValues[LESENSE_CHANNELS] =
{
/*  Ch0,   Ch1,   Ch2,   Ch3,   Ch4,   Ch5,   Ch6,   Ch7    */
  0, 0, 0, 0, 0, 0, 0, 0,
/*  Ch8,   Ch9,   Ch10,  Ch11,  Ch12,  Ch13,  Ch14,  Ch15   */
  0, 0, 0, 0, 0, 0, 0, 0
};


/**************************************************************************//**
 * @brief  This stores the maximum values seen by a channel
 * @param LESENSE_CHANNELS Vector of channels.
 *****************************************************************************/
static volatile uint32_t channelMaxValues[LESENSE_CHANNELS] =
{
/*  Ch0,   Ch1,   Ch2,   Ch3,   Ch4,   Ch5,   Ch6,   Ch7    */
  1, 1, 1, 1, 1, 1, 1, 1,
/*  Ch8,   Ch9,   Ch11,  Ch11,  Ch12,  Ch13,  Ch14,  Ch15   */
  1, 1, 1, 1, 1, 1, 1, 1
};
/**************************************************************************//**
 * @brief Update rings according to slider position
 * @par sliderPos The current Slider position
 *****************************************************************************/
void capSenseAringUpdate(int sliderPos)
{
  int i;
  int stop;

  if (sliderPos == -1)
  {
    /* No ring if touch slider is not touched */
    stop = -1;
  }
  else
  {
    /* Map 8 segments to 48 slider positions */
    stop = (sliderPos * 8) / 48;
  }

  /* Draw ring */
  for (i=0; i < 8; i++)
  {
    if (i <= stop )
    {
      SegmentLCD_ARing(i, 1);
    }
    else
    {
      SegmentLCD_ARing(i, 0);
    }
  }
}

/**************************************************************************//**
 * @brief Get the position of the slider
 * @return The position of the slider if it can be determined,
 *         -1 otherwise.
 *****************************************************************************/
int32_t CAPLESENSE_getSliderPosition(void)
{
  int      i;
  int      minPos = -1;
  uint32_t minVal = 236; /* adjust it */
  /* Values used for interpolation. There is two more which represents the edges.
   * This makes the interpolation code a bit cleaner as we do not have to make special
   * cases for handling them */
  uint32_t interpol[6]      = { 255, 255, 255, 255, 255, 255 };
  uint32_t channelPattern[] = { 0,                        SLIDER_PART0_CHANNEL + 1,
                                SLIDER_PART1_CHANNEL + 1,
                                SLIDER_PART2_CHANNEL + 1,
                                SLIDER_PART3_CHANNEL + 1 };

  /* The calculated slider position. */
  int position;

  /* Iterate through the 4 slider bars and calculate the current value divided by
   * the maximum value multiplied by 256.
   * Note that there is an offset of 1 between channelValues and interpol.
   * This is done to make interpolation easier.
   */
  for (i = 1; i < CAPLESENSE_NUMOF_SLIDERS + 1; i++)
  {
    /* interpol[i] will be in the range 0-256 depending on channelMax */
    interpol[i]  = channelValues[channelPattern[i] - 1] << 8;
    interpol[i] /= channelMaxValues[channelPattern[i] - 1];
    /* Find the minimum value and position */
    if (interpol[i] < minVal)
    {
      minVal = interpol[i];
      minPos = i;
    }
  }
  /* Check if the slider has not been touched */
  if (minPos == -1)
    return -1;

  /* Start position. Shift by 4 to get additional resolution. */
  /* Because of the interpol trick earlier we have to substract one to offset that effect */
  position = (minPos - 1) << 4;

  /* Interpolate with pad to the left */
  position -= ((256 - interpol[minPos - 1]) << 3)
              / (256 - interpol[minPos]);

  /* Interpolate with pad to the right */
  position += ((256 - interpol[minPos + 1]) << 3)
              / (256 - interpol[minPos]);

  return position;
}

void CAPSEN_SETUP(bool sleep)
{
	  uint8_t     i,j;
	  static bool init = true;

	  /* Array for storing the calibration values. */
	  static uint16_t capsenseCalibrateVals[4];

	  /* LESENSE channel configuration constant table in sense mode. */
	  static const LESENSE_ChAll_TypeDef initChsSense = LESENSE_CAPSENSE_SCAN_CONF_SENSE;
	  /* LESENSE channel configuration constant table in sleep mode. */



	  /* Different configuration for "sleep" and "sense" modes. */
	  if (sleep)
	  {
	    /* Stop LESENSE before configuration. */
	    LESENSE_ScanStop();

	    /* Wait until the currently active scan is finished. */
	    while (LESENSE_STATUS_SCANACTIVE & LESENSE_StatusGet()) ;

	    /* Clear result buffer. */
	    LESENSE_ResultBufferClear();

	    /* Set scan frequency (in Hz). */
	    (void) LESENSE_ScanFreqSet(0U, 20U);

	    /* Set clock divisor for LF clock. */
	    LESENSE_ClkDivSet(lesenseClkLF, lesenseClkDiv_1);

	    /* Configure scan channels. */
//	    LESENSE_ChannelAllConfig(&initChsSleep);
	    static const LESENSE_ChAll_TypeDef initChsSleep = LESENSE_CAPSENSE_SCAN_CONF_SLEEP;
	   	LESENSE_ChAll_TypeDef const *confChAll=&initChsSleep;
	    LESENSE_ChannelConfig(&confChAll->Ch[8], 8);
	    LESENSE_ChannelConfig(&confChAll->Ch[9], 9);
	    LESENSE_ChannelConfig(&confChAll->Ch[10], 10);
	    LESENSE_ChannelConfig(&confChAll->Ch[11], 11);

	    /* Disable scan complete interrupt. */
	    LESENSE_IntDisable(LESENSE_IEN_SCANCOMPLETE);
	  }
	  else
	  {
	    /* Stop LESENSE before configuration. */
	    LESENSE_ScanStop();

	    /* Wait until the currently active scan is finished. */
	    while (LESENSE_STATUS_SCANACTIVE & LESENSE_StatusGet()) ;

	    /* Clean scan complete interrupt flag. */
//	    LESENSE_IntClear(LESENSE_IEN_SCANCOMPLETE);

	    /* Clear result buffer. */
	    LESENSE_ResultBufferClear();

	    //light fsr
//	    LESENSE_AltExConfig(&initAltEx);
	    //

	    /* Set scan frequency (in Hz). */
	    (void) LESENSE_ScanFreqSet(0U, 20U);

	    /* Set clock divisor for LF clock. */
	    LESENSE_ClkDivSet(lesenseClkLF, lesenseClkDiv_8);

	  //  static const LESENSE_ChAll_TypeDef initChsSense = LESENSE_CAPSENSE_SCAN_CONF_SENSE;
	   	LESENSE_ChAll_TypeDef const *confChAll=&initChsSense;
	    LESENSE_ChannelConfig(&confChAll->Ch[8], 8);
	    LESENSE_ChannelConfig(&confChAll->Ch[9], 9);
	    LESENSE_ChannelConfig(&confChAll->Ch[10], 10);
	    LESENSE_ChannelConfig(&confChAll->Ch[11], 11);


	    /* Enable scan complete interrupt. */
	    LESENSE_IntEnable(LESENSE_IEN_SCANCOMPLETE);
	  }

	  /* Enable LESENSE interrupt in NVIC. */
	  NVIC_EnableIRQ(LESENSE_IRQn);

	  /* Start scanning LESENSE channels. */
	  LESENSE_ScanStart();

	  /* Run it only once. */
	  if (init)
	  {
	    /* Assuming that the pads are not touched at first, we can use the result as
	     * the threshold value to calibrate the capacitive sensing in LESENSE. */
	    init = false;

	    /* Waiting for buffer to be full. */
	    while (!(LESENSE->STATUS & LESENSE_STATUS_BUFHALFFULL)) ;

	    /* Read out steady state values from LESENSE for calibration. */
	    for (i = 0U, j = 0U; j < LESENSE_CHANNELS; j++)
	    {
	      if (channelsInUse[j])
	      {
	        if (i < CAPLESENSE_NUMOF_SLIDERS)
	        {
	          capsenseCalibrateVals[i] = LESENSE_ScanResultDataBufferGet(j)
	                                     - CAPLESENSE_SENSITIVITY_OFFS;
	        }
	        i++;
	      }
	    }

	  }
}
/**************************************************************************//**
 * @brief  LESENSE callback setup
 * @param  scanCb Scan callback
 * @param  chCb Channel callback
 *****************************************************************************/
void CAPLESENSE_setupCallbacks(void (*scanCb)(void), void (*chCb)(void))
{
	lesenseChCb   = chCb;
	lesenseScanCb = scanCb;
//	for(int ab=0;ab<10000;ab++)
}

void Demo(void)
{
	  int32_t slider;
	  bool oldBoost = vboost;

	  /* Initialize RTC timer. */
	  RTCDRV_Init();
	  RTCDRV_AllocateTimer( &xTimerForSleep);

	  /* Setup capSense callbacks. */
	  CAPLESENSE_setupCallbacks(&capSenseScanComplete, &capSenseChTrigger);
	  /* Mode0 (default on start-up). */
//	  appModeGlobal=1;
	 	    if (appModeGlobal == MODE0)
	 	    {
	 	      switch(appStateGlobal)
	 	      {
	 	        case INIT_STATE:
	 	        {
	 	          /* Enable clock for RTC. */
	 	          CMU_ClockEnable(cmuClock_RTC, true);
	 	          /* Enable RTC. */
	 	          RTC_Enable(true);
	 	          /* Go to TIMER_RESET_STATE to reset the global timer. */
	 	          appStateGlobal = TIMER_RESET_STATE;
	 	        }
	 	        break;

	 	        case TIMER_RESET_STATE:
	 	        {
	 	          /* Enable LESENSE interrupt in NVIC. */
	 	          NVIC_EnableIRQ(LESENSE_IRQn);
	 	          /* Reset RTC counter by disabling and enabling the RTC. */
	 	          RTC_Enable(false);
	 	          RTC_Enable(true);
	 	          /* Go to the AWAKE_STATE. */
	 	          appStateGlobal = AWAKE_STATE;
	 	        }
	 	        break;

	 	        case AWAKE_STATE:
	 	        {
	 	          /* Stay awake until the timer has fired. */
	 	          appStateGlobal = AWAKE_STATE;
	 	          /* Write the number of counts. */
	 //	          SegmentLCD_Number(eventCounter);

	 	          /* Check if timer has fired... */
	 	          if (secTimerFired)
	 	          {
	 	            /* ...if so, go to SENSE_PREPARE_STATE to prepare sensing. */
	 	            appStateGlobal = SENSE_PREPARE_STATE;
	 	            /* Reset sub-state. */
	 	            secTimerFired = false;
	 	            /* Disable RTC. */
	 	            RTC_Enable(false);
	 	            /* Disable clock for RTC. */
	 	            CMU_ClockEnable(cmuClock_RTC, false);
	 	          }
	 	          else
	 	          {
	 	        	 appModeGlobal=1;
	 	            EMU_EnterEM2(true);
	 	           appModeGlobal = MODE0;
	 	          }
	 	        }
	 	        break;

	 	        case SENSE_PREPARE_STATE:
	 	        {
	 	          /* Disable LCD to avoid excessive current consumption */
	 	          SegmentLCD_Disable();
	 	          /* Go to SENSE_STATE. */
	 	          appStateGlobal = SENSE_STATE;
	 	        }
	 	        break;

	 	        case SENSE_STATE:
	 	        {
	 	          /* Enter EM2. */
	 	          EMU_EnterEM2(true);
	 	        }
	 	        break;

	 	        case ERROR_STATE:
	 	        default:
	 	        {
	 	          /* Stay in ERROR_STATE. */
	 	          appStateGlobal = ERROR_STATE;
	 	        }
	 	        break;
	 	      }
	 	    }


else
{
	  /* Main loop */
	  while (1)
	  {
	    switch(demoState)
	    {
	      case DEMO_SLEEP_PREPARE:
	      {
	        /* Setup LESENSE in sleep mode. */
	    	  CAPSEN_SETUP(true);
	        /* Disable LCD to avoid excessive current consumption */
	        SegmentLCD_Disable();
	        /* Disable Vdd check. */
	        VDDCHECK_Disable();
	        /* Go to sleep state. */
	        demoState = DEMO_SLEEP;
	      }
	      break;

	      case DEMO_SLEEP:
	      {
	        /* Go to sleep and wait until the measurement completes. */
	    	  EMU_EnterEM2(true);
	      }
	      break;

	      case DEMO_SENSE_PREPARE:
	      {
	        /* Setup LESENSE in high-accuracy sense mode. */
	    	  CAPSEN_SETUP(false);
	        /* Enable vboost */
	        SegmentLCD_Init(vboost);
	        /* Go to sense state. */
	        demoState = DEMO_SENSE;
	      }
	      break;

	      case DEMO_SENSE:
	      {
	        /* Go to sleep and wait until the measurement completes. */
	    	  EMU_EnterEM2(true);

	        /* Get slider position. */
	        slider = CAPLESENSE_getSliderPosition();
	        if (-1 != slider)
	        {
	          RTCDRV_StartTimer( xTimerForSleep, rtcdrvTimerTypeOneshot, 1000, capSenseTimerFired, NULL);
	        }
	        capSenseAringUpdate(slider);

	        /* Check for change in input voltage. Enable vboost if necessary */
	        /* Initialize voltage comparator */
	        VDDCHECK_Init();

	        /* Check if voltage is below 3V, if so use voltage boost */
	        if (VDDCHECK_LowVoltage(2.9))
	        {
	          vboost = true;
	          if (oldBoost != vboost)
	          {
	            /* Enable vboost */
	            SegmentLCD_Init(vboost);
	            /* Use antenna symbol to signify enabling of vboost */
	           // SegmentLCD_Symbol(LCD_SYMBOL_ANT, vboost);
	          }
	          oldBoost = vboost;
	        }
	        else
	        {
	          vboost = false;
	        }

	        switch (demoMode)
	        {
	        case (DEMOMODE_SCROLLTEXT):
	          capSenseScrollText();
	          break;
	        case (DEMOMODE_BARS):
	          capSenseBars();
	          break;
	        case (DEMOMODE_VALUES):
	          capSenseValues();
	          break;
	        default:
	          break;
	        }
	      }
	      break;

	      default:
	      {
	        ;
	      }
	      break;
	    }
	  }
}
}


/**************************************************************************//**
 * @brief Cap sense values demo
 *****************************************************************************/
void capSenseBars(void)
{
  int barNum;
  int sliderPos;
  const char bars[3] = {'{', '!', '1' };
  char msg[8];

  sliderPos = CAPLESENSE_getSliderPosition();
  SegmentLCD_Number(sliderPos);

  if (sliderPos == -1)
  {
    SegmentLCD_Write("SLIDER");
  }
  else
  {
    /* Clear the msg string */
    snprintf(msg, 7, "         ");
    /* There are 21 possible "bars" on the display, while there are 48 slider
     * positions. This maps these 48 into 21 slider positions. */
    barNum = (sliderPos * 21) / 48;

    msg[barNum / 3] = bars[barNum %3];

    SegmentLCD_Write(msg);
  }
}
/**************************************************************************//**
 * @brief Get the current normalized channelValue for a channel
 * @param channel The channel.
 * @return The channel value in range (0-256).
 *****************************************************************************/
uint32_t CAPLESENSE_getNormalizedVal(uint8_t channel)
{
  uint32_t max = channelMaxValues[channel];
  return (channelValues[channel] << 8) / max;
}

/**************************************************************************//**
 * @brief Get the channelValue for a sensor segment
 * @param capSegment
 * @return channel
 *****************************************************************************/
uint8_t  CAPLESENSE_getSegmentChannel(uint8_t capSegment)
{
  uint8_t channel;

  switch (capSegment)
  {
  case(0):
    channel = SLIDER_PART0_CHANNEL;
    break;
  case(1):
    channel = SLIDER_PART1_CHANNEL;
    break;
  case(2):
    channel = SLIDER_PART2_CHANNEL;
    break;
  default:
    channel = SLIDER_PART3_CHANNEL;
    break;
  }
  return channel;

}
/**************************************************************************//**
 * @brief Cap sense values demo
 *****************************************************************************/
void capSenseValues(void)
{
  char msg[8];
  uint8_t channel;

  switch (subMode)
  {
  case(0):
    snprintf(msg, 7, "Pad 0");
    channel = CAPLESENSE_getSegmentChannel(0);
    break;
  case(1):
    snprintf(msg, 7, "Pad 1");
    channel = CAPLESENSE_getSegmentChannel(1);
    break;
  case(2):
    snprintf(msg, 7, "Pad 2");
    channel = CAPLESENSE_getSegmentChannel(2);
    break;
  case(3):
    snprintf(msg, 7, "Pad 3");
    channel = CAPLESENSE_getSegmentChannel(3);
    break;
  default:
    snprintf(msg, 7, "Pad 0");
    channel = CAPLESENSE_getSegmentChannel(0);
    break;
  }
  SegmentLCD_Write(msg);
  SegmentLCD_Number(CAPLESENSE_getNormalizedVal(channel));
}

/**************************************************************************//**
 * @brief  ScrollText demo
 *****************************************************************************/
void capSenseScrollText(void)
{
  char msg[10];
  int sliderPos;
  static int oldSliderPos = -2;

  int offset;

  sliderPos = CAPLESENSE_getSliderPosition();
  if (oldSliderPos != sliderPos)
  {
    oldSliderPos = sliderPos;
    SegmentLCD_Number(sliderPos);

    if (sliderPos == -1)
      sliderPos = 0;

    offset = ((strlen(message) - 7) * sliderPos) / 48;

    snprintf(msg, 8, "%s", message + offset);
    SegmentLCD_Write(msg);
  }
}
/**************************************************************************//**
 * @brief  Callback for sensor scan complete.
 *****************************************************************************/
void capSensScanComplete(void)
{
  ;
}

/**************************************************************************//**
 * @brief  Callback for sensor channel triggered.
 *****************************************************************************/
void capSensChTrigger(void)
{
  demoState = DEMO_SENSE_PREPARE;
}

void LESENSE_setup(void)
{
	INT_Disable();
	CAPLESENSE_setupACMP();

	LIGHTFSRSENSE();
	CAPSEN_SETUP(true);	//PUT THIS AFTER LIGHTFSRSENSE
	setupRTC();
	/* Enable RTC interrupt in NVIC. */
	NVIC_EnableIRQ(RTC_IRQn);

	INT_Enable();
//  lightfsr_demo();
//	Demo();
}
void lightfsr_demo(void)
{
	/* Go to infinite loop. */
	  while(1)
	  {
	    /* Mode0 (default on start-up). */
	    if (appModeGlobal == MODE0)
	    {
	      switch(appStateGlobal)
	      {
	        case INIT_STATE:
	        {
	          /* Enable clock for RTC. */
	          CMU_ClockEnable(cmuClock_RTC, true);
	          /* Enable RTC. */
	          RTC_Enable(true);
	          /* Go to TIMER_RESET_STATE to reset the global timer. */
	          appStateGlobal = TIMER_RESET_STATE;
	        }
	        break;

	        case TIMER_RESET_STATE:
	        {
	          /* Enable LESENSE interrupt in NVIC. */
	          NVIC_EnableIRQ(LESENSE_IRQn);
	          /* Reset RTC counter by disabling and enabling the RTC. */
	          RTC_Enable(false);
	          RTC_Enable(true);
	          /* Go to the AWAKE_STATE. */
	          appStateGlobal = AWAKE_STATE;
	        }
	        break;

	        case AWAKE_STATE:
	        {
	          /* Stay awake until the timer has fired. */
	          appStateGlobal = AWAKE_STATE;
	          /* Write the number of counts. */
//	          SegmentLCD_Number(eventCounter);

	          /* Check if timer has fired... */
	          if (secTimerFired)
	          {
	            /* ...if so, go to SENSE_PREPARE_STATE to prepare sensing. */
	            appStateGlobal = SENSE_PREPARE_STATE;
	            /* Reset sub-state. */
	            secTimerFired = false;
	            /* Disable RTC. */
	            RTC_Enable(false);
	            /* Disable clock for RTC. */
	            CMU_ClockEnable(cmuClock_RTC, false);
	          }
	          else
	          {
	           sleep();
	          }
	        }
	        break;

	        case SENSE_PREPARE_STATE:
	        {
	          /* Disable LCD to avoid excessive current consumption */
	          SegmentLCD_Disable();
	          /* Go to SENSE_STATE. */
	          appStateGlobal = SENSE_STATE;
	        }
	        break;

	        case SENSE_STATE:
	        {
	          /* Enter EM2. */
	          sleep();
	        }
	        break;

	        case ERROR_STATE:
	        default:
	        {
	          /* Stay in ERROR_STATE. */
	          appStateGlobal = ERROR_STATE;
	        }
	        break;
	      }
	    }

	    else /* unknown mode */
	    {
	      /* Unknown error, go to app error state anyway. */
	      appStateGlobal = ERROR_STATE;
	    }
	  }
}
/**************************************************************************//**
 * @brief  Setup the RTC
 *****************************************************************************/
void setupRTC(void)
{
  /* RTC configuration constant table. */
  static const RTC_Init_TypeDef initRTC = RTC_INIT_DEFAULT;


  /* Initialize RTC. */
  RTC_Init(&initRTC);

  /* Set COMP0 to overflow at the configured value (in seconds). */
  RTC_CompareSet(0, (uint32_t)CMU_ClockFreqGet(cmuClock_RTC) *
                    (uint32_t)INIT_STATE_TIME_SEC);

  /* Make sure that all pending interrupt is cleared. */
  RTC_IntClear(0xFFFFFFFFUL);

  /* Enable interrupt for COMP0. */
  RTC_IntEnable(RTC_IEN_COMP0);

  /* Finally enable RTC. */
  RTC_Enable(true);
}


void LIGHTFSRSENSE(void)
{
	  /* LESENSE channel configuration constant table. */
	  static const LESENSE_ChAll_TypeDef initChs = LESENSE_LIGHTSENSEON_SCAN_CONF;

	  /* LESENSE central configuration constant table. */
	  static const LESENSE_Init_TypeDef initLESENSE =
	  {
	    .coreCtrl =
	    {
	      .scanStart = lesenseScanStartPeriodic,
	      .prsSel = lesensePRSCh0,
	      .scanConfSel = lesenseScanConfDirMap,
	      .invACMP0 = false,
	      .invACMP1 = false,
	      .dualSample = false,
	      .storeScanRes = false,
	      .bufOverWr = true,
	      .bufTrigLevel = lesenseBufTrigHalf,
	      .wakeupOnDMA = lesenseDMAWakeUpDisable,
	      .biasMode = lesenseBiasModeDutyCycle,
	      .debugRun = false
	    },

	    .timeCtrl =
	    {
	      .startDelay = 0U
	    },

	    .perCtrl =
	    {
	      .dacCh0Data = lesenseDACIfData,
	      .dacCh0ConvMode = lesenseDACConvModeDisable,
	      .dacCh0OutMode = lesenseDACOutModeDisable,
	      .dacCh1Data = lesenseDACIfData,
	      .dacCh1ConvMode = lesenseDACConvModeDisable,
	      .dacCh1OutMode = lesenseDACOutModeDisable,
	      .dacPresc = 0U,
	      .dacRef = lesenseDACRefBandGap,
	      .acmp0Mode = lesenseACMPModeMuxThres,
	      .acmp1Mode = lesenseACMPModeMuxThres,
	      .warmupMode = lesenseWarmupModeNormal
	    },

	    .decCtrl =
	    {
	      .decInput = lesenseDecInputSensorSt,
	      .initState = 0U,
	      .chkState = false,
	      .intMap = true,
	      .hystPRS0 = false,
	      .hystPRS1 = false,
	      .hystPRS2 = false,
	      .hystIRQ = false,
	      .prsCount = true,
	      .prsChSel0 = lesensePRSCh0,
	      .prsChSel1 = lesensePRSCh1,
	      .prsChSel2 = lesensePRSCh2,
	      .prsChSel3 = lesensePRSCh3
	    }
	  };
	  /* Initialize LESENSE interface with RESET. */
	  LESENSE_Init(&initLESENSE, true);


		LESENSE_ChAll_TypeDef const *confChAll=&initChs;
		    LESENSE_ChannelConfig(&confChAll->Ch[6], 6);
		    LESENSE_ChannelConfig(&confChAll->Ch[7], 7);
		    LESENSE_ChannelConfig(&confChAll->Ch[1], 1);
		    LESENSE_ChannelConfig(&confChAll->Ch[2], 2);


	  /* Configure alternate excitation channels. */
	  LESENSE_AltExConfig(&initAltEx);

	  /* Set scan frequency (in Hz). */
	  (void)LESENSE_ScanFreqSet(0U, 30U);

	  /* Set clock divisor for LF clock. */
	  LESENSE_ClkDivSet(lesenseClkLF, lesenseClkDiv_1);

	  /* Start scanning LESENSE channels. */
	  LESENSE_ScanStart();
}
/**************************************************************************//**
 * @brief  Setup the ACMP
 *****************************************************************************/
void CAPLESENSE_setupACMP(void)
{
	  /* ACMP configuration constant table. */
	  static const ACMP_Init_TypeDef initACMP =
	  {
	    .fullBias = false,                 /* fullBias */
	    .halfBias = true,                  /* halfBias */
	    .biasProg =  0x0,                  /* biasProg */
	    .interruptOnFallingEdge =  false,  /* interrupt on rising edge */
	    .interruptOnRisingEdge =  false,   /* interrupt on falling edge */
	    .warmTime = acmpWarmTime512,       /* 512 cycle warmup to be safe */
	    .hysteresisLevel = acmpHysteresisLevel5, /* hysteresis level 5 */
	    .inactiveValue = false,            /* inactive value */
	    .lowPowerReferenceEnabled = false, /* low power reference */
	    .vddLevel = 0x00,                  /* VDD level */
	    .enable = false                    /* Don't request enabling. */
	  };

	  static const ACMP_CapsenseInit_TypeDef initCACMP =
	   {
	     .fullBias                 = true,
	     .halfBias                 = false,
	     .biasProg                 =                  0xFF,
	     .warmTime                 = acmpWarmTime512,
	     .hysteresisLevel          = acmpHysteresisLevel7,
	     .resistor                 = acmpResistor0,
	     .lowPowerReferenceEnabled = false,
	     .vddLevel                 =                 0x5D,
	     .enable                   = true
	   };

	  /* Configure ACMP. */
	  ACMP_Init(ACMP0, &initACMP);
	  /* Disable ACMP0 out to a pin. */
	  ACMP_GPIOSetup(ACMP0, 0, false, false);
	  ACMP_GPIOSetup(ACMP1, 0, false, true);
	  /* Initialize ACMPs in capacitive sense mode. */
	  ACMP_CapsenseInit(ACMP1, &initCACMP);


	  /* Set up ACMP negSel to VDD, posSel is controlled by LESENSE. */
	  ACMP_ChannelSet(ACMP0, acmpChannelVDD, acmpChannel0);
	  /* LESENSE controls ACMP thus ACMP_Enable(ACMP0) should NOT be called in order
	   * to ensure lower current consumption. */
}

/**************************************************************************//**
 * @brief  LESENSE interrupt handler
 *****************************************************************************/
void LESENSE_IRQHandler(void)
{
	  uint32_t count;
	  INT_Disable();
	  /* LESENSE scan complete interrupt. */
	  if (LESENSE_IF_SCANCOMPLETE & LESENSE_IntGetEnabled())
	  {
	    LESENSE_IntClear(LESENSE_IF_SCANCOMPLETE);

//	    /* Iterate trough all channels */
	    for (currentChannel = 0; currentChannel < LESENSE_CHANNELS; currentChannel++)
	    {
	      /* If this channel is not in use, skip to the next one */
	      if (!channelsInUse[currentChannel])
	      {
	        continue;
	      }

	      /* Read out value from LESENSE buffer */
	      count = LESENSE_ScanResultDataGet();

	      /* Store value in channelValues */
	      channelValues[currentChannel] = count;

	      /* Update channelMaxValues */
	      if (count > channelMaxValues[currentChannel])
	      {
	        channelMaxValues[currentChannel] = count;
	      }
	    }
//
//	    /* Call callback function. */
	    if (lesenseScanCb != 0x00000000)
	    {
	      lesenseScanCb();
	    }

	  }

	  /* LESENSE channel interrupt. */
	  if (CAPLESENSE_CHANNEL_INT & LESENSE_IntGetEnabled())
	  {

	    /* Clear flags. */
	    LESENSE_IntClear(CAPLESENSE_CHANNEL_INT);

	    if(system_on==1)
	    {
	    	GPIO_PinModeSet(gpioPortB, 11, gpioModeDisabled, 0);
	    	system_on=0;
	    }
	    else
	    {
	    	GPIO_PinModeSet(gpioPortB, 11, gpioModePushPull, 0);
	    	system_on=1;
	    }
	    for (currentChannel = 0; currentChannel < LESENSE_CHANNELS; currentChannel++)
	   	    {
	   	      /* If this channel is not in use, skip to the next one */
	   	      if (!channelsInUse[currentChannel])
	   	      {
	   	        continue;
	   	      }

	   	      /* Read out value from LESENSE buffer */
	   	      count = LESENSE_ScanResultDataGet();

	   	      /* Store value in channelValues */
	   	      channelValues[currentChannel] = count;

	   	      /* Update channelMaxValues */
	   	      if (count > channelMaxValues[currentChannel])
	   	      {
	   	        channelMaxValues[currentChannel] = count;
	   	      }
	   	    }

	  }
  if (LESENSE_IF_CH7 & LESENSE_IntGetEnabled())//REALTED TO FSR1
	 {
	   LESENSE_IntClear(LESENSE_IF_CH7);
	  if(fsr_flag1==0)//ONE STEP
		  {
			static const LESENSE_ChAll_TypeDef initChs = LESENSE_LIGHTSENSEOFF_SCAN_CONF;
			LESENSE_ChAll_TypeDef const *confChAll=&initChs;
			LESENSE_ChannelConfig(&confChAll->Ch[7], 7);
			fsr_flag1=1;;

		  }

		  else//FOOT OFF
		  {
			static const LESENSE_ChAll_TypeDef initChs = LESENSE_LIGHTSENSEON_SCAN_CONF;
			LESENSE_ChAll_TypeDef const *confChAll=&initChs;
			LESENSE_ChannelConfig(&confChAll->Ch[7], 7);
			circular_buffer[write_pntr]='T';
			write_pntr=(write_pntr+1)%buffer_length;
			fsr_flag1=0;

				// INPUT GPIOS OF MOTOR1 TO BE SAME SO THEY DONT TURN

 	 	 	 	 GPIO_PinModeSet(motor_input_port, motor_input_1_pin, motor_input_mode, 1);
				 GPIO_PinModeSet(motor_input_port, motor_input_4_pin, motor_input_mode, 1);

			circular_buffer[write_pntr]='+';
			write_pntr=(write_pntr+1)%buffer_length;

//			temp=temp*(-1);		//convert temp to positive
			int first=49;
			circular_buffer[write_pntr]=first;
			write_pntr=(write_pntr+1)%buffer_length;
			int second =0;
			circular_buffer[write_pntr]=second;
			write_pntr=(write_pntr+1)%buffer_length;
			LEUART0->IEN|=LEUART_IEN_TXBL;
		  }
	 }
  if (LESENSE_IF_CH1 & LESENSE_IntGetEnabled())//REALTED TO ULTRSONIC SENSOR
	 {
	   LESENSE_IntClear(LESENSE_IF_CH1);
	   ultrasonic_count++;
	 }
  if (LESENSE_IF_CH2 & LESENSE_IntGetEnabled())//REALTED TO FSR2
  	 {
  	   LESENSE_IntClear(LESENSE_IF_CH2);
  	  if(fsr_flag2==0)//ONE STEP
  		  {
  			static const LESENSE_ChAll_TypeDef initChs = LESENSE_LIGHTSENSEOFF_SCAN_CONF;
  			LESENSE_ChAll_TypeDef const *confChAll=&initChs;
  			LESENSE_ChannelConfig(&confChAll->Ch[2], 2);
  			fsr_flag2=1;
  		  }

  		  else//FOOT OFF
  		  {
  			static const LESENSE_ChAll_TypeDef initChs = LESENSE_LIGHTSENSEON_SCAN_CONF;
  			LESENSE_ChAll_TypeDef const *confChAll=&initChs;
  			LESENSE_ChannelConfig(&confChAll->Ch[2], 2);
  			fsr_flag2=0;
			//INPUT GPIOS OF MOTOR 2 TO BE SAME SO THEY DONT TURN
  			GPIO_PinModeSet(motor_input_port, motor_input_2_pin, motor_input_mode, 1);
			GPIO_PinModeSet(motor_input_port, motor_input_3_pin, motor_input_mode,1);
  		  }
  	 }
  /* Negative edge interrupt on LESENSE CH6. */
 if (LESENSE_IF_CH6 & LESENSE_IntGetEnabled())// REALTED TO LIGHTSENSE		//PRAVEEN ADDED ELSE IF TO REMOVE FLICKER LED
  {
    LESENSE_IntClear(LESENSE_IF_CH6);
    eventCounterlight=(eventCounterlight+1)%2;
    if(eventCounterlight==1)// DARKNESS
        {
    	static const LESENSE_ChAll_TypeDef initChs = LESENSE_LIGHTSENSEOFF_SCAN_CONF;
   		LESENSE_ChAll_TypeDef const *confChAll=&initChs;
    	LESENSE_ChannelConfig(&confChAll->Ch[6], 6);
    	GPIO_PinOutSet(LEDPORT, LEDPIN1);
        }
        else//LET THERE BE LIGHT
        {
		static const LESENSE_ChAll_TypeDef initChs = LESENSE_LIGHTSENSEON_SCAN_CONF;
		LESENSE_ChAll_TypeDef const *confChAll=&initChs;
		LESENSE_ChannelConfig(&confChAll->Ch[6], 6);
		GPIO_PinOutClear(LEDPORT, LEDPIN1);
        }
  }



  /* Check the current mode of the application. */
  if (appModeGlobal == MODE0)
  {
    /* Increase the event counter... */

    /* ...and go to INIT_STATE. */
    appStateGlobal = INIT_STATE;

  }
  else if (appModeGlobal == MODE1)
  {
    /* LESENSE interrupts only enabled in EM0 in order to keep the MCU
     * awake on every sensor event.
     * Go to RESET_STATE to reset the timeout timer. */
    appStateGlobal = TIMER_RESET_STATE;
  }
  else
  {
    appStateGlobal = ERROR_STATE;
  }
	    INT_Enable();
}

/**************************************************************************//**
 * @brief  RTC common interrupt handler
 *****************************************************************************/
void RTC_IRQHandler(void)
{
	INT_Disable();
  uint32_t tmp;
//  BlockSleep(1);
//  ADC_Start(ADC0, adcStartSingle);

  /* Store enabled interrupts in temp variable. */
  tmp = RTC->IEN;
  //ultrasonic_count=74;
  if(ultrasonic_count>75)
  {
	  GPIO_PinOutSet(LEDPORT, LEDPIN1);

  }//send alert
  	circular_buffer[write_pntr]='A';
	write_pntr=(write_pntr+1)%buffer_length;

	circular_buffer[write_pntr]=34;
	write_pntr=(write_pntr+1)%buffer_length;
	circular_buffer[write_pntr]=0;
	write_pntr=(write_pntr+1)%buffer_length;
	circular_buffer[write_pntr]=0;
	write_pntr=(write_pntr+1)%buffer_length;
	LEUART0->IEN|=LEUART_IEN_TXBL;
 	ultrasonic_count=0;

  /* Check if COMP0 interrupt is enabled and set. */
  if (RTC_IF_COMP0 & (tmp & RTC_IntGet()))
  {
    /* Timer has fired, clear interrupt flag... */
    RTC_IntClear(RTC_IFC_COMP0);

    /* ...and set the global flag. */
    secTimerFired = true;
  }
  INT_Enable();
}
