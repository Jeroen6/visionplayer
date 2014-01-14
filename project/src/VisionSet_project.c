#include "VisionSet.h"
#include "audio_driver.h"
#include "stm32f4_discovery_audio_codec.h"
#include "led.h"
#include <stdlib.h>
#include <stdio.h>


AudioDriver_Tone freq = Tone_C4;

// This vision set is executed when the system powers on
void VisionSet1(image_t *pSrc, image_t *pDst)
{
  benchmark_t bench;
	uint8_t blobCount, i  = 0;
	uint8_t xc,yc = 0;
	uint16_t toneFreq = 0;
	point_t left_top;
	point_t left_bottom;
	point_t right_top;
	point_t right_bottom;
	point_t top_left;
	point_t top_right;
	point_t bottom_left;
	point_t bottom_right;
	blobinfo_t* blobinfo;
	
  // 1. Original source image to PC
  pc_send_string("1. Source image");
  pSrc->lut = LUT_CLIP;
  pc_send_image(pSrc);

	/*
	// vContrastStretchFast
  benchmark_start(&bench, "vContrastStretchFast");
  vContrastStretchFast(pSrc, pDst);
  benchmark_stop(&bench);
  pc_send_benchmark(&bench);
  pDst->lut = LUT_CLIP;
  pc_send_image(pDst);
  pc_send_string("2. vContrastStretchFast");
	*/

  // vThresholdIsoData
  benchmark_start(&bench, "vThresholdIsoData");
  vThresholdIsoData(pSrc, pDst, DARK);
  benchmark_stop(&bench);
  pc_send_benchmark(&bench);
  pDst->lut = LUT_BINARY;
  pc_send_image(pDst);
  pc_send_string("3. vThresholdIsoData");

  // vCropVertical
  benchmark_start(&bench, "vCropVertical");
  vCropVertical(pDst, pDst, 0, ROI_TOP, ROI_BOTTOM);
  benchmark_stop(&bench);
  pc_send_benchmark(&bench);
  pDst->lut = LUT_BINARY;
  pc_send_image(pDst);
  pc_send_string("4. vCropVertical");

	// vSetBorders
  benchmark_start(&bench, "vSetborders");
  vSetBorders(pDst, pDst, 1);
  benchmark_stop(&bench);
  pc_send_benchmark(&bench);
  pDst->lut = LUT_BINARY;
  pc_send_image(pDst);
  pc_send_string("5. vSetBorders");
	
  // vRemoveBorderBlobs
  benchmark_start(&bench, "vRemoveBorderBlobs");
  vRemoveBorderBlobs(pDst, pDst, EIGHT);
  benchmark_stop(&bench);
  pc_send_benchmark(&bench);
  pDst->lut = LUT_BINARY;
  pc_send_image(pDst);
  pc_send_string("6. vRemoveBorderBlobs");
	
	// vLabelBlobs
	benchmark_start(&bench, "iLabelBlobs");
  blobCount = iLabelBlobs(pDst, pDst, EIGHT);
  benchmark_stop(&bench);
  pc_send_benchmark(&bench);
  pDst->lut = LUT_LABELED;
  pc_send_image(pDst);
  pc_send_string("7. iLabelBlobs");

	// vBlobAnalyse
	blobinfo = (blobinfo_t*) malloc(blobCount * sizeof(blobinfo_t));
	benchmark_start(&bench, "vBlobAnalyse");
	vBlobAnalyse(pDst, blobCount, blobinfo );
	benchmark_stop(&bench);
  pc_send_benchmark(&bench);
  pDst->lut = LUT_LABELED;
  pc_send_image(pDst);
  pc_send_string("7. vBlobAnalyse");
	
	// Herken tonen
	benchmark_start(&bench, "vRecognizeTone");
	
	for (toneFreq = 0, i = 1; i <= blobCount; i++)
	{
		// Bereken centroid
		vCentroid(pDst, i, &xc, &yc);
		
		// Binnen horizontale region-of-interrest?
		if ( ((xc > ROI_LEFT) && (xc < ROI_RIGHT)) ) {	// Negeer 7-segmenten
			continue;
		}
		
		// Binnen verticale region-of-interrest?
		//else if ( (yc < (ROI_TOP + ROI_MARGIN)) || (yc > (ROI_BOTTOM - ROI_MARGIN)) ) {
		//	continue;
		//}
		
		// Verhoog toonfrequentie, gebasseerd op de positie van de huidige blobs
		
		if (xc > 60) {
			xc -= 120;
		}
		
		toneFreq = (uint8_t )((xc / 10) + 1);
		
/*
		// Linker 4
		if      ( (xc > (START_LEFT  + (0 * DOT_WIDTH))) && (xc <= (START_LEFT  + (1 * DOT_WIDTH))) ) { toneFreq = Tone_C5;  }		// Baan 1 -> C4	261.63
		else if ( (xc > (START_LEFT  + (1 * DOT_WIDTH))) && (xc <= (START_LEFT  + (2 * DOT_WIDTH))) ) { toneFreq = Tone_D5;  }		// Baan 2 -> D4	293.66
		else if ( (xc > (START_LEFT  + (2 * DOT_WIDTH))) && (xc <= (START_LEFT  + (3 * DOT_WIDTH))) ) { toneFreq = Tone_E5;  }		// Baan 3 -> E4	329.63
		else if ( (xc > (START_LEFT  + (3 * DOT_WIDTH))) && (xc <= (START_LEFT  + (4 * DOT_WIDTH))) ) { toneFreq = Tone_F5;  }		// Baan 4 -> F4	349.23

		// Rechter 4
		else if ( (xc > (START_RIGHT + (0 * DOT_WIDTH))) && (xc <= (START_RIGHT + (1 * DOT_WIDTH))) ) { toneFreq = Tone_F5;  }		// Baan 5 -> G4	392.00
		else if ( (xc > (START_RIGHT + (1 * DOT_WIDTH))) && (xc <= (START_RIGHT + (2 * DOT_WIDTH))) ) { toneFreq = Tone_A5;  }		// Baan 6 -> A4	440.00
		else if ( (xc > (START_RIGHT + (2 * DOT_WIDTH))) && (xc <= (START_RIGHT + (3 * DOT_WIDTH))) ) { toneFreq = Tone_B5;  }		// Baan 7 -> B4	493.88
		else if ( (xc > (START_RIGHT + (3 * DOT_WIDTH))) && (xc <= (START_RIGHT + (4 * DOT_WIDTH))) ) { toneFreq = Tone_C6;  }		// Baan 8 -> B4	493.88
	*/
		
	}

	free(blobinfo);
	benchmark_stop(&bench);
  pc_send_benchmark(&bench);
  pDst->lut = LUT_LABELED;
  pc_send_image(pDst);
  pc_send_string("8. Recognize tones");
	
	// Speel de gevonden toon
	benchmark_start(&bench, "vPlayTone");
	Project_PlayTone( (AudioDriver_Tone)(toneFreq) );		// Speel toon
	led_toggle(LED_RED);
	benchmark_stop(&bench);
  pc_send_benchmark(&bench);
  pDst->lut = LUT_LABELED;
  pc_send_image(pDst);
  pc_send_string("9. Play tone");

	// Teken region-of-interrest
	left_top.x     = ROI_LEFT;
	left_bottom.x  = ROI_LEFT;
	left_bottom.y  = pDst->height;
	left_top.y     = 0;
	
	right_top.x    = ROI_RIGHT;
	right_bottom.x = ROI_RIGHT;
	right_bottom.y = pDst->height;
	right_top.y    = 0;
	
	top_left.x  = 0;
	top_left.y  = ROI_TOP + ROI_MARGIN;
	top_right.y = ROI_TOP + ROI_MARGIN;
	top_right.x = pDst->width;
	
	bottom_left.x  = 0;
	bottom_left.y  = ROI_BOTTOM - ROI_MARGIN;
	bottom_right.y = ROI_BOTTOM - ROI_MARGIN;
	bottom_right.x = pDst->width;
	
	benchmark_start(&bench, "vDrawLine");
	vDrawLine(pDst, left_top,    left_bottom,  1, 1);
	vDrawLine(pDst, right_top,   right_bottom, 1, 1);
	vDrawLine(pDst, top_left,    top_right,    1, 1);
	vDrawLine(pDst, bottom_left, bottom_right, 1, 1);
	
	benchmark_stop(&bench);
  pc_send_benchmark(&bench);
  pDst->lut = LUT_LABELED;
  pc_send_image(pDst);
  pc_send_string("10. vDrawLine");
}

