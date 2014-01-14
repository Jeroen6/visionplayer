#include "VisionSet.h"
#include "audio_driver.h"
#include "led.h"
#include <stdlib.h>


// This vision set is executed when the system powers on
void VisionSet2(image_t *pSrc, image_t *pDst)
{
  benchmark_t bench;
	uint8_t i, blobCount, xc, yc = 0;
  double n20,n02,n11 = 0.0;
  double im1, im2    = 0.0;
	
  // 1. Original source image to PC
  pc_send_string("1. Source image");
  pSrc->lut = LUT_CLIP;
  pc_send_image(pSrc);
	
	// vCopy
  benchmark_start(&bench, "vCopy");
  vCopy(pSrc, pDst);
  benchmark_stop(&bench);
  pc_send_benchmark(&bench);
  pDst->lut = LUT_CLIP;
  pc_send_image(pDst);
  pc_send_string("2. vCopy");
	
  // Notice that from now on the destination image also is the source image!!

	// vContrastStretchFast
  benchmark_start(&bench, "vContrastStretchFast");
  vContrastStretchFast(pDst, pDst);
  benchmark_stop(&bench);
  pc_send_benchmark(&bench);
  pDst->lut = LUT_CLIP;
  pc_send_image(pDst);
  pc_send_string("3. vContrastStretchFast");

  // vThresholdIsoData
  benchmark_start(&bench, "vThresholdIsoData");
  vThresholdIsoData(pDst, pDst, DARK);
  benchmark_stop(&bench);
  pc_send_benchmark(&bench);
  pDst->lut = LUT_BINARY;
  pc_send_image(pDst);
  pc_send_string("4. vThresholdIsoData DARK");

  // vRemoveBorderBlobs
  benchmark_start(&bench, "vRemoveBorderBlobs");
  vRemoveBorderBlobs(pDst, pDst, EIGHT);
  benchmark_stop(&bench);
  pc_send_benchmark(&bench);
  pDst->lut = LUT_BINARY;
  pc_send_image(pDst);
  pc_send_string("5. vRemoveBorderBlobs EIGHT");
	
	// vFillHoles
  benchmark_start(&bench, "vFillHoles");
  vFillHoles(pDst, pDst, EIGHT);
  benchmark_stop(&bench);
  pc_send_benchmark(&bench);
  pDst->lut = LUT_BINARY;
  pc_send_image(pDst);
  pc_send_string("6. vFillHoles EIGHT");
	
	// vLabelBlobs
	benchmark_start(&bench, "iLabelBlobs");
  blobCount = iLabelBlobs(pDst, pDst, EIGHT);
  benchmark_stop(&bench);
  pc_send_benchmark(&bench);
  pDst->lut = LUT_LABELED;
  pc_send_image(pDst);
  pc_send_string("7. iLabelBlobs EIGHT");

	// Reset LEDs
	led_reset(LED1);
	led_reset(LED2);
	led_reset(LED3);
	led_reset(LED4);
	
	for (i = 1; i <= blobCount; i++)
	{
		// Bereken centroid
    vCentroid(pDst, i, &xc, &yc);
		
		// Bereken centrale momenten
		n20 = dNormalizedCentralMoments (pDst, i, 2, 0);
		n02 = dNormalizedCentralMoments (pDst, i, 0, 2);
		n11 = dNormalizedCentralMoments (pDst, i, 1, 1);

		// Bereken Hu invariante momenten
		im1  =  n20 + n02;
		im2  = (n20 - n02) * (n20 - n02);
		im2 += 4 * (n11* n11);

		// Blob vorm gebasseerd op de 2 berekende momenten
		switch ( eGetShape(&im1, &im2) )
		{
				case CIRCLE:
					led_set(LED1);
					break;

				case SQUARE:
					led_set(LED1);
					led_set(LED2);
					led_set(LED3);
					led_set(LED4);
					break;

				case TRIANGLE:
					led_set(LED1);
					led_set(LED2);
					led_set(LED3);
					break;

				default:
					break;
		}

	}
}
