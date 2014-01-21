/******************************************************************************
 * Project    : Embedded Vision Design
 * Copyright  : 2012 HAN Embedded Systems Engineering
 * Author     : Hugo
 *
 * Description: Header file for the implementation of image operators
 *
 ******************************************************************************
  Change History:

    Version 2.0 - October 2012
    > Implemented new image sructure
    > Added platform dependent QDEBUG macro

    Version 1.0 - September 2011
    > Initial revision

******************************************************************************/
#ifndef _OPERATORS_H_
#define _OPERATORS_H_

#include <stdint.h>

// ----------------------------------------------------------------------------
// Debugging macro
// ----------------------------------------------------------------------------
#ifdef QDEBUG_ENABLE

  #include <QDebug>
  #define QDEBUG(x) qDebug()<<x

  // Example usage:
  // QDEBUG("Debug " << parameter << " value");

#else

  #define QDEBUG(x) //x

#endif

// ----------------------------------------------------------------------------
// Defines
// ----------------------------------------------------------------------------
// Image attributes
// Make sure (IMG_HEIGHT * IMG_WIDTH) is a power of 32 (DMA)
#define IMG_HEIGHT  (144)
#define IMG_WIDTH   (176)

// Image displaying LookUp Tables
// These values must match the index values of the Qt dropdownboxes
#define LUT_STRETCH (0)
#define LUT_CLIP    (1)
#define LUT_BINARY  (2)
#define LUT_LABELED (3)

// Value of a special marked pixel (in a binary image)
#define MARK 7


// ----------------------------------------------------------------------------
// Type definitions
// ----------------------------------------------------------------------------
typedef struct
{
  uint16_t width;
  uint16_t height;
  uint16_t lut;
  uint16_t dummy; // Make sure the size of this struct is a power of 32 (DMA)
  uint8_t  data[IMG_HEIGHT][IMG_WIDTH];
}image_t;

typedef enum
{
  BRIGHT = 0,
  DARK
}eBrightness;

typedef enum
{
  FOUR  = 4,
  EIGHT = 8
}eConnected;

typedef struct blobinfo_t
{

  uint16_t height;
  uint16_t width;
  uint16_t nof_pixels;
  double   perimeter;
}blobinfo_t;

typedef struct
{
    uint16_t x;
    uint16_t y;
} point_t;

typedef struct
{
    uint16_t x;
    uint16_t y;
    uint16_t w;
    uint16_t h;
}image_roi_t;

typedef enum
{
  UNKNOWN, CIRCLE, SQUARE, TRIANGLE
}form_t;


// ----------------------------------------------------------------------------
// Function prototypes
// ----------------------------------------------------------------------------

// Gets the shape of an object, based on its moment
form_t eGetShape (double *im1, double *im2);


// The contrast of the src image is stretch to 0-255 in the dst image
// This function is required by the Qt PC application. It is used to display
// the source- and destination image. If this function is not implemented, the
// user will see random data.
void vContrastStretch(image_t *src, // must be a greyscale image
                      image_t *dst, // result is a greyscale image
                      uint16_t bottom,
                      uint16_t top);

// All pixels in src image with value between 'low' and 'high' are
// set to 1 in dst image. Rest is set to 0
// This function is used in all VisionSets. Without it, initially nothing will
// seem to happen.
void vThreshold(image_t *src,
                image_t *dst, // result is a binary image
                uint8_t low,
                uint8_t high);

// Rotates an image 180 degrees
// This function is required by the microcontroller application. The camera is
// mounted upside down.
void vRotate180(image_t *img);

// This function does the same as vContrastStretch. However, it always
// stretches from 0 to 255 and implements a fast algorithm.
void vContrastStretchFast(image_t *src,  // must be a greyscale image
                          image_t *dst); // result is a greyscale image

// All pixel values are set to 0
void vErase(image_t *img);

// Src and dst image are the same
void vCopy(image_t *src,
           image_t *dst);

// Src and dst image are added pixel by pixel
// Result in dst image
void vAdd(image_t *src,
          image_t *dst);

// Src and dst image are multiplied pixel by pixel
// Result in dst image
void vMultiply(image_t *src,
               image_t *dst);

// Src image is copied to dst image
// All pixels in dst image with value 'selected' are set to 'value'
void vSetSelectedToValue(image_t *src,
                         image_t *dst,
                         uint8_t selected,
                         uint8_t value);

// Src image is copied to dst image
// All border pixels of dst image are set to 'value'
void vSetBorders(image_t *src,
                 image_t *dst,
                 uint8_t value);

// Uses a bitplane (mask) to slice the source image
void vBitSlicing(image_t *src,
                 image_t *dst,
                 uint8_t mask);

// Make a histogram of the source image and calculate the sum of all pixel
// values
void vHistogram(image_t  *src,
                uint16_t *hist,
                uint32_t *sum);

// Threshold values are automatically generated based on the histogram of
// src image
void vThresholdIsoData(image_t *src,
                       image_t *dst,
                       eBrightness brightness); // DARK | BRIGHT

// Threshold values are automatically generated based on the histogram of
// src image
void vThresholdOtsu(image_t *src,
                    image_t *dst,
                    eBrightness brightness); // DARK | BRIGHT

// Fill holes
void vFillHoles(image_t *src, // must be a binary image
                image_t *dst,
                eConnected connected); // FOUR | EIGHT

// Remove the border blobs
void vRemoveBorderBlobs(image_t *src, // must be a binary image
                        image_t *dst,
                        eConnected connected); // FOUR | EIGHT

// Find the edges
void vFindEdges(image_t *src, // must be a binary image
                image_t *dst,
                eConnected connected);

// Label all blobs, returns the number of labeled blobs
uint32_t iLabelBlobs(image_t *src, // must be a binary image
                     image_t *dst,
                     eConnected connected);

// Analyse blobs
// pBlobInfo points to a blobinfo_t struct declared by the calling program
void vBlobAnalyse(image_t *img,
                  const uint8_t blobnr,
                  blobinfo_t *pBlobInfo);

// Calculates the centroid of a blob
void vCentroid(image_t *img, uint8_t blobnr, uint8_t *xc, uint8_t *yc);

// Calculates the normalized central moments of a blob specified by p and q
// Ref.: Gonzalez, R. (). 11.3.4 Moment Invariants.
//       In Digital Image Processing. pp. 839-842.
//       New Jersey: Pearson Prentice Hall.
//
//       Myler, R. (2010). Moments. In The Pocket Handbook of Image Processing
//       Algoritms in C. pp. 157-159.  New Jersey: Prentice Hall.
double dNormalizedCentralMoments(image_t *img, // must be a binary or labeled image
                                 uint8_t blobnr, // must be '1' if img is binary
                                 int p,
                                 int q);

// vMarkBackground (binary)
// Marks pixels connected to the borsers
void vMarkConnectedToBorders (image_t *img,
                                                            uint8_t value,
                                                            eConnected connected);

// vMarkBackground (binary)
// Marks border pixels of value
void vMarkBorders(image_t *dst,
                  uint8_t value);

// This function checks the number of pixels around pixel (x,y) and returns the
// number of pixels that equal value.
uint8_t iNeighbourCount(image_t *img,
                        uint16_t x,
                        uint16_t y,
                        uint8_t value,
                        eConnected connected);

uint8_t iNeighboursEqualOrHigher(image_t *img,
                                 uint16_t x,
                                 uint16_t y,
                                 uint8_t value,
                                 eConnected connected);

// This function will draw a line from point 'begin' to 'end,
// with a linesize of 'size' pixels and colour 'value'
void vDrawLine (image_t *img,
                point_t start,
                point_t end,
                uint8_t size,
                uint8_t value);


// This function wil cut an image vertically based on a Region-of-interrest
void vCropVertical (image_t *src, // must be a binary image
                    image_t *dst,
                    uint8_t val,
                    uint8_t y0,
                    uint8_t y1 );


image_roi_t sRoiValidate(image_roi_t);
int imageToBCD(image_t *img, image_roi_t roi);

#endif // _OPERATORS_H_
// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------
