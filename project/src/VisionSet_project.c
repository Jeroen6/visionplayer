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
	//point_t left_top;
	//point_t left_bottom;
	//point_t right_top;
	//point_t right_bottom;
	//point_t top_left;
	//point_t top_right;
	//point_t bottom_left;
	//point_t bottom_right;
	
	
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

/*
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
	
	benchmark_start(&bench, "vDrawLines");
	vDrawLine(pDst, left_top,    left_bottom,  1, 1);
	vDrawLine(pDst, right_top,   right_bottom, 1, 1);
	vDrawLine(pDst, top_left,    top_right,    1, 1);
	vDrawLine(pDst, bottom_left, bottom_right, 1, 1);
	benchmark_stop(&bench);
  pc_send_benchmark(&bench);
  pDst->lut = LUT_LABELED;
  pc_send_image(pDst);
  pc_send_string("10. vDrawLine");
*/
}
