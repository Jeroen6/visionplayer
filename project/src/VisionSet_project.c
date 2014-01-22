#include "VisionSet.h"
#include "musicplayer.h"
#include "stm32f4_discovery_audio_codec.h"
#include "led.h"
#include <stdlib.h>
#include <stdio.h>
#include "motor.h"

volatile uint8_t ledsOn = 0;

// This vision set is executed when the system powers on
void VisionSet1(image_t *pSrc, image_t *pDst)
{
  uint8_t allowMotorControl = 1;
  benchmark_t bench;
  uint8_t blobCount = 0;
  blobinfo_t* blobinfo;
  BeepTone tone               = Tone_XX;
  point_t roi_top1            = {0,0};
  point_t roi_top2            = {0,0};
  point_t roi_bottom1         = {0,0};
  point_t roi_bottom2         = {0,0};
  point_t roi_left1           = {0,0};
  point_t roi_left2           = {0,0};
  point_t roi_right1          = {0,0};
  point_t roi_right2          = {0,0};
  point_t roi_segment_right   = {0,0};
  point_t roi_segment_left    = {0,0};
  point_t roi_segment_ctop    = {0,0};
  point_t roi_segment_cbottom = {0,0};
  

  // 1. Original source image to PC
  pc_send_string("1. Source image");
  pSrc->lut = LUT_CLIP;
  pc_send_image(pSrc);

  // vContrastStretchFast
  #if 0
  benchmark_start(&bench, "vContrastStretchFast");
  vContrastStretchFast(pSrc, pDst);
  benchmark_stop(&bench);
  pc_send_benchmark(&bench);
  pDst->lut = LUT_CLIP;
  pc_send_image(pDst);
  pc_send_string("2. vContrastStretchFast");
  #endif

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
  blobinfo = (blobinfo_t*) malloc(blobCount * sizeof(blobinfo_t));  // Reserveer blobinfo geheugen
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
  free(blobinfo);                                                    // Geef blobinfo geheugen vrij, belangrijk
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
  
  // Zend gevonden toon naar pc applicatie
  switch ( tone )
  {
    // Octaaf 5
    case Tone_C5: pc_send_string("    - Tone = C5"); break;
    case Tone_D5: pc_send_string("    - Tone = D5"); break;
    case Tone_E5: pc_send_string("    - Tone = E5"); break;
    case Tone_F5: pc_send_string("    - Tone = F5"); break;
    case Tone_G5: pc_send_string("    - Tone = G5"); break;
    case Tone_A5: pc_send_string("    - Tone = A5"); break;
    case Tone_B5: pc_send_string("    - Tone = B5"); break;
    
    // Octaaf 6
    case Tone_C6: pc_send_string("    - Tone = C6"); break;
    case Tone_D6: pc_send_string("    - Tone = D6"); break;
    case Tone_E6: pc_send_string("    - Tone = E6"); break;
    case Tone_F6: pc_send_string("    - Tone = F6"); break;
    case Tone_G6: pc_send_string("    - Tone = G6"); break;
    case Tone_A6: pc_send_string("    - Tone = A6"); break;
    case Tone_B6: pc_send_string("    - Tone = B6"); break;
    
    // Unknown
    default:      pc_send_string("    - Tone = None"); break;
  }
  
  
  // Zet led als een toon gedetecteerd is
  if  (tone != Tone_XX) { led_set  (LED_RED); }  // Toon gevonden
  else                  { led_reset(LED_RED); }  // Geen toon
  
  // Horizontale ROI
  roi_top1.y    = ROI_START_Y; roi_top1.x = 0;
  roi_top2.y    = ROI_START_Y; roi_top2.x = pDst->width;
  roi_bottom1.y = ROI_END_Y;   roi_bottom1.x = 0;
  roi_bottom2.y = ROI_END_Y;   roi_bottom2.x = pDst->width;
  
  // Verticale ROI
  roi_left1.y  = 0;            roi_left1.x  = SEVENSEG_START_X;
  roi_left2.y  = pDst->height; roi_left2.x  = SEVENSEG_START_X;
  roi_right1.y = 0;            roi_right1.x = SEVENSEG_END_X;
  roi_right2.y = pDst->height; roi_right2.x = SEVENSEG_END_X;
  
  // Find digits
  image_roi_t digit1_roi,digit2_roi;
  int digit1=-1,digit2=-1;
  const uint16_t sevensegWidth = (SEVENSEG_END_X - SEVENSEG_START_X)/ 2;
  
  digit1_roi.h = 32; 
  digit1_roi.w = sevensegWidth;
  digit1_roi.x = SEVENSEG_START_X;
  digit1_roi.y = ROI_START_Y;
  
  digit2_roi.h = 32;
  digit2_roi.w = sevensegWidth;
  digit2_roi.x = SEVENSEG_START_X + sevensegWidth;
  digit2_roi.y = ROI_START_Y;
  
  roi_segment_left.x     = 0;
  roi_segment_right.x   = pDst->width;
  roi_segment_left.y     = digit2_roi.y + digit2_roi.h;
  roi_segment_right.y   = digit2_roi.y + digit2_roi.h;
  roi_segment_ctop.x     = digit1_roi.x + digit2_roi.w;
  roi_segment_cbottom.x = digit1_roi.x + digit2_roi.w;
  roi_segment_ctop.y     = digit1_roi.y;
  roi_segment_cbottom.y = digit1_roi.y + digit1_roi.h;
  
  digit1 = imageToBCD(pDst, digit1_roi);
  digit2 = imageToBCD(pDst, digit2_roi);
  
  
  vCopy (pSrc, pDst);
  benchmark_start(&bench, "vDrawLines");
  vDrawLine(pDst, roi_top1,    roi_top2,    1, 255);
  vDrawLine(pDst, roi_bottom1, roi_bottom2, 1, 255);
  vDrawLine(pDst, roi_left1,   roi_left2,   1, 255);
  vDrawLine(pDst, roi_right1,  roi_right2,  1, 255);
  vDrawLine(pDst, roi_segment_left,  roi_segment_right,  1, 1);  // 7 segment bottom
  vDrawLine(pDst, roi_segment_ctop,  roi_segment_cbottom,  1, 1);  // 7 segment center
  benchmark_stop(&bench);
  pc_send_benchmark(&bench);
  pDst->lut = LUT_CLIP;
  pc_send_image(pDst);
  pc_send_string("9. vDrawLine");
  
    
  // user interaction
  if(allowMotorControl && ledsOn < 5){
    // Enable target leds
    setAux(1);
    allowMotorControl=0;
    ledsOn++;
  }
  // Allow enable rotate if valid 7 segments
  if(digit1 >= 0 && digit2 >= 0){
    led_reset(LED_RED);  
    if(GPIOA->IDR & (1<<0)){

      if(allowMotorControl){
        // Calculate digits to time
        uint32_t time_ms = ((digit1 * 10)+(digit2))*1000;
        SMC_step(1600,1,time_ms,1);
        allowMotorControl = 0;
      }
    }else{
      
    }
  
  }else{
    led_set(LED_RED);
  }
}
