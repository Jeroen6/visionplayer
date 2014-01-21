#include "VisionSet.h"
#include "musicplayer.h"
#include "stm32f4_discovery_audio_codec.h"
#include "led.h"
#include <stdlib.h>
#include <stdio.h>

// This vision set is executed when the system powers on
void VisionSet1(image_t *pSrc, image_t *pDst)
{
  benchmark_t bench;
	uint8_t blobCount = 0;
	blobinfo_t* blobinfo;
	BeepTone tone;
	point_t roi_top1;
	point_t roi_top2;
	point_t roi_bottom1;
	point_t roi_bottom2;
	point_t roi_left1;
	point_t roi_left2;
	point_t roi_right1;
	point_t roi_right2;
	
  // 1. Original source image to PC
  pc_send_string("1. Source image");
  pSrc->lut = LUT_CLIP;
  pc_send_image(pSrc);

	// vContrastStretchFast
  benchmark_start(&bench, "vContrastStretchFast");
  vContrastStretchFast(pSrc, pDst);
  benchmark_stop(&bench);
  pc_send_benchmark(&bench);
  pDst->lut = LUT_CLIP;
  pc_send_image(pDst);
  pc_send_string("2. vContrastStretchFast");

  // vThresholdIsoData
  benchmark_start(&bench, "vThresholdIsoData");
  vThresholdIsoData(pSrc, pDst, DARK);
  benchmark_stop(&bench);
  pc_send_benchmark(&bench);
  pDst->lut = LUT_BINARY;
  pc_send_image(pDst);
  pc_send_string("3. vThresholdIsoData");

  // vRemoveBorderBlobs
  benchmark_start(&bench, "vRemoveBorderBlobs");
  vRemoveBorderBlobs(pDst, pDst, EIGHT);
  benchmark_stop(&bench);
  pc_send_benchmark(&bench);
  pDst->lut = LUT_BINARY;
  pc_send_image(pDst);
  pc_send_string("4. vRemoveBorderBlobs");
	
	// vLabelBlobs
	benchmark_start(&bench, "iLabelBlobs");
  blobCount = iLabelBlobs(pDst, pDst, EIGHT);
  benchmark_stop(&bench);
  pc_send_benchmark(&bench);
  pDst->lut = LUT_LABELED;
  pc_send_image(pDst);
  pc_send_string("5. iLabelBlobs");

	// vBlobAnalyse
	blobinfo = (blobinfo_t*) malloc(blobCount * sizeof(blobinfo_t));	// Reserveer blobinfo geheugen
	benchmark_start(&bench, "vBlobAnalyse");
	vBlobAnalyse(pDst, blobCount, blobinfo );
	benchmark_stop(&bench);
  pc_send_benchmark(&bench);
  pDst->lut = LUT_LABELED;
  pc_send_image(pDst);
  pc_send_string("6. vBlobAnalyse");
	
	// Herken toon
	benchmark_start(&bench, "vRecognizeTone");
	tone = vRecognizeTone(pDst, blobinfo, blobCount);
	free(blobinfo);																										// Geef blobinfo geheugen vrij, belangrijk
	benchmark_stop(&bench);
  pc_send_benchmark(&bench);
  pDst->lut = LUT_LABELED;
  pc_send_image(pDst);
  pc_send_string("7. vRecognizeTone");
	
	// Speel toon
	benchmark_start(&bench, "vPlayTone");
	vPlayTone(tone);
	free(blobinfo);
	benchmark_stop(&bench);
  pc_send_benchmark(&bench);
  pDst->lut = LUT_LABELED;
  pc_send_image(pDst);
  pc_send_string("8. vPlayTone");
	
	led_toggle(LED_RED);
	
	// Horizontale ROI
	roi_top1.y = ROI_START_Y;  roi_top1.x = 0;
	roi_top2.y = ROI_START_Y;  roi_top2.x = pDst->width;
	roi_bottom1.y = ROI_END_Y; roi_bottom1.x = 0;
	roi_bottom2.y = ROI_END_Y; roi_bottom2.x = pDst->width;
	
	
	// Verticale ROI
	roi_left1.y  = 0;            roi_left1.x  = SEVENSEG_START_X;
	roi_left2.y  = pDst->height; roi_left2.x  = SEVENSEG_START_X;
	roi_right1.y = 0;            roi_right1.x = SEVENSEG_END_X;
	roi_right2.y = pDst->height; roi_right2.x = SEVENSEG_END_X;
	
	vCopy (pSrc, pDst);
	benchmark_start(&bench, "vDrawLines");
	vDrawLine(pDst, roi_top1,    roi_top2,    1, 1);
	vDrawLine(pDst, roi_bottom1, roi_bottom2, 1, 1);
	vDrawLine(pDst, roi_left1,   roi_left2,   1, 1);
	vDrawLine(pDst, roi_right1,  roi_right2,  1, 1);
	benchmark_stop(&bench);
  pc_send_benchmark(&bench);
  pDst->lut = LUT_CLIP;
  pc_send_image(pDst);
  pc_send_string("9. vDrawLine");
}
