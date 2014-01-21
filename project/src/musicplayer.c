//-------------------------------------------------------------------
// Headers
//-------------------------------------------------------------------
#include "stm32f4xx.h"
#include "musicplayer.h"
#include "stm32f4_discovery_audio_codec.h"
#include "led.h"
#include "motor.h"


//-------------------------------------------------------------------
// Globale variabelen
//-------------------------------------------------------------------
const uint16_t AUDIO_SAMPLE[48] = { 0 };	// Stilte

volatile uint8_t UserButtonPressed = 0;
/**
  * @brief  This function handles EXTI0_IRQ Handler.
  * @param  None
  * @retval None
  */
void EXTI2_IRQHandler(void)
{
  UserButtonPressed = 0x01;
  
  /* Clear the EXTI line pending bit */
  EXTI_ClearITPendingBit(EXTI_Line0);
}

//-------------------------------------------------------------------
// Musicplayer_Init
//-------------------------------------------------------------------
void vInitMusicplayer (void)
{
	// Initialize Audio interface
  EVAL_AUDIO_SetAudioInterface( AUDIO_INTERFACE_I2S );
	EVAL_AUDIO_Init( OUTPUT_DEVICE_BOTH, 60, I2S_AudioFreq_48k );		// Volume 80%, 44Khz samplefrequentie
	Audio_MAL_Play((uint32_t)AUDIO_SAMPLE, sizeof(AUDIO_SAMPLE) );	// Speel stilte, waarover de toongenerator de toon speelt
	EVAL_AUDIO_PauseResume(AUDIO_RESUME);														// Start met spelen
	
	// Motor init
	motor_init();
	
	// User button init
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the BUTTON Clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

  /* Configure Button pin as input */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_Init(GPIOA, &GPIO_InitStructure);

#if 0
    /* Connect Button EXTI Line to Button GPIO Pin */
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);

    /* Configure Button EXTI line */
    EXTI_InitStructure.EXTI_Line = EXTI_Line2;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;  
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Enable and set Button EXTI Interrupt to the lowest priority */
    NVIC_InitStructure.NVIC_IRQChannel = EXTI2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure); 
#endif
	
	
	// Motor test
	// Dit staat hier omdat een van de andere init's niet werkt zonder evdk
	//motor_init();
	//setLedBlink();
	//{int i; for(i=0;i<1000000;i++);}	// Small delay for the motor control board to accept the previous command
	//// Motor één rondje
	//SMC_step(1600,1,1000,1);

	
}


//-------------------------------------------------------------------
// Musicplayer_PlayTone
//-------------------------------------------------------------------
BeepTone vRecognizeTone (image_t* img, const blobinfo_t* blobinfo, const uint8_t blobCount)
{
	BeepTone tone   = Tone_XX;
	uint8_t i       = 0;
	uint8_t	xc, yc  = 0;
	uint8_t tonePos = 0;
	
	// Ga door alle gevonden blobs heen, herken tonen
	for (i = 0; i < blobCount; i++)
	{
		// Bereken centroid, zodat we de coordinaat van de blob en toon weten
		vCentroid(img, i+1, &xc, &yc);
		
		// Negeer huidige blob als deze ruis is
		if ( (blobinfo[i].nof_pixels < TONE_MIN_PX) )
		{
			continue;
		}

		// Negeer huidige blob als deze buiten region-of-interrest valt
		else if ( !(xc < SEVENSEG_START_X || xc > SEVENSEG_END_X) || 
							!(yc > ROI_START_Y      && yc < ROI_END_Y)      ||
							!(xc < img->width)                              )
		{
			continue;
		}
		
		// Vind toon; Linker segment, octaaf 5
		tonePos = xc - LEFT_SEGMENT_START_X;
		
		if ( (tonePos > (0 * TONE_HOR_PX)) && (tonePos <= (1 * TONE_HOR_PX)) ) { tone = Tone_C5; break; }
		if ( (tonePos > (1 * TONE_HOR_PX)) && (tonePos <= (2 * TONE_HOR_PX)) ) { tone = Tone_D5; break; }
		if ( (tonePos > (2 * TONE_HOR_PX)) && (tonePos <= (3 * TONE_HOR_PX)) ) { tone = Tone_E5; break; }
		if ( (tonePos > (3 * TONE_HOR_PX)) && (tonePos <= (4 * TONE_HOR_PX)) ) { tone = Tone_F5; break; }
		if ( (tonePos > (4 * TONE_HOR_PX)) && (tonePos <= (5 * TONE_HOR_PX)) ) { tone = Tone_G5; break; }
		if ( (tonePos > (5 * TONE_HOR_PX)) && (tonePos <= (6 * TONE_HOR_PX)) ) { tone = Tone_A5; break; }
		if ( (tonePos > (6 * TONE_HOR_PX)) && (tonePos <= (7 * TONE_HOR_PX)) ) { tone = Tone_B5; break; }
	
		// Vind toon; Rechter segment, octaaf 6
		tonePos = tonePos - RIGHT_SEGMENT_START_X;
		
		if ( (tonePos > (0 * TONE_HOR_PX)) && (tonePos <= (1 * TONE_HOR_PX)) ) { tone = Tone_C6; break; }
		if ( (tonePos > (1 * TONE_HOR_PX)) && (tonePos <= (2 * TONE_HOR_PX)) ) { tone = Tone_D6; break; }
		if ( (tonePos > (2 * TONE_HOR_PX)) && (tonePos <= (3 * TONE_HOR_PX)) ) { tone = Tone_E6; break; }
		if ( (tonePos > (3 * TONE_HOR_PX)) && (tonePos <= (4 * TONE_HOR_PX)) ) { tone = Tone_F6; break; }
		if ( (tonePos > (4 * TONE_HOR_PX)) && (tonePos <= (5 * TONE_HOR_PX)) ) { tone = Tone_G6; break; }
		if ( (tonePos > (5 * TONE_HOR_PX)) && (tonePos <= (6 * TONE_HOR_PX)) ) { tone = Tone_A6; break; }
		if ( (tonePos > (6 * TONE_HOR_PX)) && (tonePos <= (7 * TONE_HOR_PX)) ) { tone = Tone_B6; break; }
	}
	
	// Geef gevonden toon
	return tone;
}


//-------------------------------------------------------------------
// Musicplayer_PlayTone
//-------------------------------------------------------------------
void vPlayTone (const BeepTone tone)
{
	static uint16_t prevTone = Tone_XX;
	uint8_t toneReg = 0x01;

	// Stop als geen toonverandering nodig
	if ( tone == prevTone )
	{
		return;														
	}
	
	// Stop toon als nodig
	else if ( tone == Tone_XX )
	{
		prevTone = tone;
		Codec_WriteRegister(0x1E, 0x00);	
		prevTone = Tone_XX;
		return;
	}
	
	// Vorige toon = nieuwe toon
	else
	{
		prevTone = tone;
	}

	// Bereken register waarde, voor toon frequentie en aan-tijd
	toneReg |= ((tone > Tone_B6) ? (Tone_B6) : (tone)) << 4;
	
	// Zet benodigde registers voor nieuwe toon
	Codec_WriteRegister(0x1C, toneReg);					// Beep Frequency & aan-tijd
	Codec_WriteRegister(0x1D, 0x03);						// Beep Volume    & uit-Time
	Codec_WriteRegister(0x1E, Beep_Continuous);	// Beep generator aan, produceer enkele toon
}


//-------------------------------------------------------------------
// Audio driver callback funtie
//-------------------------------------------------------------------
uint32_t Codec_TIMEOUT_UserCallback(void)
{
	while (1);
}


//-------------------------------------------------------------------
// Audio driver callback funtie
//-------------------------------------------------------------------
void EVAL_AUDIO_TransferComplete_CallBack(uint32_t pBuffer, uint32_t Size)
{
	Audio_MAL_Play((uint32_t)AUDIO_SAMPLE, sizeof(AUDIO_SAMPLE) );
}


//-------------------------------------------------------------------
// Audio driver callback funtie
//-------------------------------------------------------------------
uint16_t EVAL_AUDIO_GetSampleCallBack(void)
{
	return 0;
}
