/******************************************************************************
 * Project    : Embedded Vision Design
 * Copyright  : 2012 HAN Embedded Systems Engineering
 * Author     : Hugo
 *
 * Description: Implementation of image operators
 *              Implement these functions only using C (not C++)!!
 *
 ******************************************************************************
  Change History:

    Version 2.0 - October 2012
    > Implemented new image structure

    Version 1.0 - September 2011
    > Initial revision

******************************************************************************/
#include "operators.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>


// ----------------------------------------------------------------------------
// vContrastStretch
// ----------------------------------------------------------------------------
void vContrastStretch(image_t *src, // must be a greyscale image
                      image_t *dst, // result is a greyscale image
                      uint16_t bottom,
                      uint16_t top)
{
    // Declaraties variabelen
    uint32_t hpx  = 0;
    uint32_t lpx  = 255;
    float sFactor = 0.0f;
    const float sFactorAdd = 0.5f + bottom;

    uint8_t* pSrcData    = &src->data[0][0];
    uint8_t* pDstData    = &dst->data[0][0];
    uint8_t* pSrcDataEnd = &src->data[IMG_HEIGHT-1][IMG_WIDTH-1];

    // Debug
    QDEBUG("> vContrastStretch: Begin");

    // Vind highest- & lowest pixel
    for ( ; pSrcData <= pSrcDataEnd; pSrcData++ )
    {
    if ( *pSrcData < lpx ) { lpx = *pSrcData; }
    if ( *pSrcData > hpx ) { hpx = *pSrcData; }
    }

    // Bereken stretch multiplier & divide-by-zero checking
    sFactor  = ((lpx >= hpx) || (bottom >= top)) ? (255.0f) : ((1.0f * top - bottom) / (1.0f * hpx - lpx));
    pSrcData = &src->data[0][0];		// Reset data pointer naar begin van plaatje

    // Pas contrast stretching toe
    for ( ; pSrcData <= pSrcDataEnd; pSrcData++, pDstData++ ) {
    *pDstData = (uint8_t)( ((*pSrcData - lpx) * sFactor) + sFactorAdd);
    }

    // Wat debug informatie
    QDEBUG("> vContrastStretch: Done, low = " << lpx << ", high = " << hpx << ", sfactor = " << sFactor);
}


// ----------------------------------------------------------------------------
// vContrastStretchFast
// ----------------------------------------------------------------------------
void vContrastStretchFast(image_t *src, // must be a greyscale image
                          image_t *dst) // result is a greyscale image
{
    // Debug
    QDEBUG("> vThreshold: Begin");

    vContrastStretch(src, dst, 0, 255);   // Klaar :)

    // Debug info
    QDEBUG("> vThreshold: Done");
}


// ----------------------------------------------------------------------------
// vThreshold
// ----------------------------------------------------------------------------
void vThreshold(image_t *src,
                image_t *dst, // result is a binary image
                uint8_t low,
                uint8_t high)
{
    // Declaraties variabelen
    uint8_t* pSrcData    = &src->data[0][0];
    uint8_t* pDstData    = &dst->data[0][0];
    uint8_t* pSrcDataEnd = &src->data[IMG_HEIGHT-1][IMG_WIDTH-1];

    // Debug
    QDEBUG("> vThreshold: Begin, applying threshold Low = " << low << ", high = " << high);

    // Pas thresholding toe
    for ( ; pSrcData <= pSrcDataEnd; pSrcData++, pDstData++ ) {
    *pDstData = (*pSrcData >= low) && (*pSrcData <= high) ? (1) : (0);
    }

    // Debug info
    QDEBUG("> vThreshold: Done");
}


// ----------------------------------------------------------------------------
// vMarkBorders
// ----------------------------------------------------------------------------
void vMarkBorders(image_t *dst,
                  uint8_t value)
{
    // Declaraties variabelen
    uint32_t x,y = 0;

    // Debug
    QDEBUG("> vMarkBorders: Begin, marking all borders with value of " << value << " to " << MARK );

    // Zet border pixels, rand boven en onder
    for ( x = 0; x != dst->width; x++ )
    {
        if (dst->data[0][x]             == value) { dst->data[0][x]             = MARK; }   // Boven
        if (dst->data[dst->height-1][x] == value) { dst->data[dst->height-1][x] = MARK; }   // Onder
    }

    // Zet border pixels, rand links en rechts
    for ( y = 0; y != dst->height; y++ )
    {
        if (dst->data[y][0]            == value) { dst->data[y][0]            = MARK; }     // Links
        if (dst->data[y][dst->width-1] == value) { dst->data[y][dst->width-1] = MARK; }     // Rechts
    }

    // Debug info
    QDEBUG("> vMarkBorders: Done");
}


// ----------------------------------------------------------------------------
// vSetBorders
// ----------------------------------------------------------------------------
void vSetBorders(image_t *src,
                 image_t *dst,
                 uint8_t value)
{
    // Declaraties variabelen
    uint32_t x,y = 0;

    // Debug
    QDEBUG("> vSetBorders: Begin, set all borders to value " << value);

    // Zet border pixels, X
    for ( x = 0; x != src->width; x++ )
    {
        dst->data[0][x]             = value;
        dst->data[src->height-1][x] = value;
    }

    // Zet border pixels, Y
    for ( y = 0; y != src->height; y++ )
    {
      dst->data[y][0]            = value;
      dst->data[y][src->width-1] = value;
    }

    // Debug info
    QDEBUG("> vSetBorders: Done");
}


// ----------------------------------------------------------------------------
// vRotate180
// ----------------------------------------------------------------------------
void vRotate180(image_t *img)
{
    // Declaraties variabelen
    uint8_t *pImgTail = &img->data[0][0];
    uint8_t *pImgHead = &img->data[IMG_HEIGHT-2][IMG_WIDTH-2];
    uint8_t  tmp      = 0;

    // Debug
    QDEBUG("> vRotate180: Begin");

    // Rotate
    for ( ; pImgHead != pImgTail; pImgHead--, pImgTail++ )
    {
    tmp       = *pImgHead;
    *pImgHead = *pImgTail;
    *pImgTail = tmp;
    }

    // Debug info
    QDEBUG("> vRotate180: Done");
}


// ----------------------------------------------------------------------------
// vErase
// ----------------------------------------------------------------------------
void vErase(image_t *img)
{
    // Declaraties variabelen
    uint8_t* pSrcData    = &img->data[0][0];
    uint8_t* pSrcDataEnd = &img->data[IMG_HEIGHT-1][IMG_WIDTH-1];

    // Debug
    QDEBUG("> vErase: Begin");

    // Zet alle waarden op NULL
    for ( ; pSrcData <= pSrcDataEnd; pSrcData++ ) {
        *pSrcData = 0;
    }

    // Debug info
    QDEBUG("> vErase: Done");
}


// ----------------------------------------------------------------------------
// vCopy
// ----------------------------------------------------------------------------
void vCopy(image_t *src,
           image_t *dst)
{
    // Declaraties variabelen
    uint8_t* pSrcData    = &src->data[0][0];
    uint8_t* pDstData    = &dst->data[0][0];
    uint8_t* pSrcDataEnd = &src->data[IMG_HEIGHT-1][IMG_WIDTH-1];

    // Debug
    QDEBUG("> vCopy: Begin");

    // Kopier
    for ( ; pSrcData <= pSrcDataEnd; pSrcData++, pDstData++ ) {
        *pDstData = *pSrcData;
    }

    // Debug info
    QDEBUG("> vCopy: Done");
}


// ----------------------------------------------------------------------------
// vAdd
// ----------------------------------------------------------------------------
void vAdd(image_t *src,
          image_t *dst)
{
    // Declaraties variabelen
    uint8_t* pSrcData    = &src->data[0][0];
    uint8_t* pDstData    = &dst->data[0][0];
    uint8_t* pSrcDataEnd = &src->data[IMG_HEIGHT-1][IMG_WIDTH-1];

    // Debug
    QDEBUG("> vAdd: Begin");

    // Zet alle waarden
    for ( ; pSrcData <= pSrcDataEnd; pSrcData++, pDstData++ ) {
        *pDstData = (*pDstData + *pSrcData) % 255;
    }

    // Debug info
    QDEBUG("> vAdd: Done");
}


// ----------------------------------------------------------------------------
// vMultiply
// ----------------------------------------------------------------------------
void vMultiply(image_t *src,
               image_t *dst)
{
    // Declaraties variabelen
    uint8_t* pSrcData    = &src->data[0][0];
    uint8_t* pDstData    = &dst->data[0][0];
    uint8_t* pSrcDataEnd = &src->data[IMG_HEIGHT-1][IMG_WIDTH-1];

    // Debug
    QDEBUG("> vMultiply: Begin");

    // Zet alle waarden
    for ( ; pSrcData <= pSrcDataEnd; pSrcData++, pDstData++ ) {
        *pDstData = (*pDstData * *pSrcData) % 255;
    }

    // Debug info
    QDEBUG("> vMultiply: Done");
}


// ----------------------------------------------------------------------------
// vSetSelectedToValue
// ----------------------------------------------------------------------------
void vSetSelectedToValue(image_t *src,
                         image_t *dst,
                         uint8_t selected,
                         uint8_t value)
{
    // Declaraties variabelen
    uint8_t* pSrcData    = &src->data[0][0];
    uint8_t* pDstData    = &dst->data[0][0];
    uint8_t* pSrcDataEnd = &src->data[IMG_HEIGHT-1][IMG_WIDTH-1];

    // Debug
    QDEBUG("> vSetSelectedToValue: Begin, set all values of " << selected << " to " << value);

    // Zet waarden
    for ( ; pSrcData <= pSrcDataEnd; pSrcData++, pDstData++ ) {
        *pDstData = (*pSrcData == selected) ? (value) : (*pSrcData);
    }

    // Debug info
    QDEBUG("> vSetSelectedToValue: Done");
}


// ----------------------------------------------------------------------------
// vBitSlicing
// ----------------------------------------------------------------------------
void vBitSlicing(image_t *src,
                 image_t *dst,
                 uint8_t mask)
{
    // Declaraties variabelen
    uint8_t* pSrcData    = &src->data[0][0];
    uint8_t* pDstData    = &dst->data[0][0];
    uint8_t* pSrcDataEnd = &src->data[IMG_HEIGHT-1][IMG_WIDTH-1];

    // Debug
    QDEBUG("> vBitSlicing: Begin, mask = " << mask );

    // Bitslicing
    for ( ; pSrcData <= pSrcDataEnd; pSrcData++, pDstData++ ) {
        *pDstData = *pSrcData & mask;
    }

    // Debug info
    QDEBUG("> vBitSlicing: Done");
}


// ----------------------------------------------------------------------------
// vHistogram
// ----------------------------------------------------------------------------
void vHistogram(image_t  *src,
                uint16_t *hist,
                uint32_t *sum )
{
    // Declaraties variabelen
    uint16_t i           = 0;
    uint8_t* pSrcData    = &src->data[0][0];
    uint8_t* pSrcDataEnd = &src->data[IMG_HEIGHT-1][IMG_WIDTH-1];

    // Debug
    QDEBUG("> vHistogram: Begin");

    // Reset enige oude waarden op 0
    for ( *sum = i = 0; i != 256; i++ ) {
    hist[i] = 0;
    }

    // Bereken histogram en som
    for ( ; pSrcData <= pSrcDataEnd; pSrcData++ )
    {
    hist[*pSrcData]++;
    *sum += *pSrcData;
    }

    // Debug info
    QDEBUG("> vHistogram: Done, sum = " << *sum);
}


// ----------------------------------------------------------------------------
// vThresholdIsoData
// ----------------------------------------------------------------------------
void vThresholdIsoData(image_t *src,
                       image_t *dst,
                       eBrightness brightness) // DARK | BRIGHT
{
    uint16_t hist[256]  = { 0 };
    uint32_t sum, i     = 0;
    uint8_t Threshold   = 0;
    uint8_t prvMean     = 0;
    uint8_t curMean     = 1;
    uint32_t meanLeft, meanRight, meanLeftDiv, meanRightDiv = 0;

    // Debug
    QDEBUG("> vThresholdIsoData: Begin");

    // Bereken histogram
    vHistogram (src, hist, &sum);

    // Vind ideale mean uit histogram
    Threshold = 100;    // Start threshold in midden van bereik 0 < T < 256

    // Zoek threshold
    while ( curMean != prvMean )    // Zolang berekende mean niet gelijk is aan de vorige mean
    {
        prvMean  = curMean;
        meanLeft = meanRight = meanLeftDiv = meanRightDiv = 0;

        // Bereken sommen mean links en rechts
        for (i = 0; i != 256; i++)
        {
            if      ( i < Threshold ) { meanLeft  += i * hist[i]; meanLeftDiv  += hist[i]; }    // 0 < i < T
            else if ( i > Threshold ) { meanRight += i * hist[i]; meanRightDiv += hist[i]; }    // T < i < 255
        }

        // Bereken means
        meanLeft  = (meanLeft  != 0) && (meanLeftDiv  != 0) ? (meanLeft  / meanLeftDiv)  : (0);
        meanRight = (meanRight != 0) && (meanRightDiv != 0) ? (meanRight / meanRightDiv) : (0);
        curMean   = ((meanLeft + meanRight) / 2) + 0.5;

        // Zet nieuwe threshold
        Threshold = curMean;
    }

    // Debug info
    QDEBUG("> vThresholdIsoData: Applying found threshold of " << Threshold );

    // Zet threshold, met de gevonden waarden,
    // afhankelijk of donkere of lichte objecten weergegeven moeten worden
    vThreshold ( src, dst, (brightness == DARK) ? (0)         : (Threshold),
                           (brightness == DARK) ? (Threshold) : (255)     );

    // Debug info
    QDEBUG("> vThresholdIsoData: Done");
}


// ----------------------------------------------------------------------------
// vThresholdOtsu
// ----------------------------------------------------------------------------
void vThresholdOtsu(image_t *src, image_t *dst, eBrightness brightness)
{
    // Zie ook: http://www.labbookpages.co.uk/software/imgProc/otsuThreshold.html

    uint8_t Threshold = 0;
    uint16_t hist[256] = { 0 };
    uint32_t sum, sumB, i, wB, wF  = 0;
    float mB, mF, max, between = 0.0;
    const uint32_t total = src->width * src->height;    // Totaal aantal pixels

    // Debug
    QDEBUG("> vThresholdOtsu: Begin");

    // Bereken histogram
    vHistogram (src, hist, &sum);

    // Zoek threshold
    for ( i = Threshold = wB = wF = mB = mF = max = sumB = between = 0; i != 256; i++)
    {
        wB += hist[i];                              // Weight, Background

        if      (wB == 0)     { continue; }
        else if (wB >= total) { break;    }

        wF      = total - wB;                       // Weight, Foreground
        sumB   += i * hist[i];                      // Sum, Background
        mB      = 1.0f * sumB / wB;                 // Mean, Background
        mF      = 1.0f * (sum - sumB) / wF;         // Mean, Foreground
        between = wB * wF * (mB - mF) * (mB - mF);  // Between Class Variance

        // Nieuwe maximum gevonden?
        if (between > max)
        {
            max = between;
            Threshold = i;
        }
    }

    // Debug info
    QDEBUG("> vThresholdOtsu: Applying found threshold of " << Threshold );

    // Zet threshold, met de gevonden waarden,
    // afhankelijk of donkere of lichte objecten weergegeven moeten worden
    vThreshold ( src, dst, (brightness == DARK) ? (0)         : (Threshold),
                           (brightness == DARK) ? (Threshold) : (255)     );

    // Debug info
    QDEBUG("> vThresholdOtsu: Done!");
}


// ----------------------------------------------------------------------------
// vFillHoles
// ----------------------------------------------------------------------------
void vFillHoles(image_t *src, // must be a binary image
                image_t *dst,
                eConnected connected) // FOUR | EIGHT
{
    // Declaraties variabelen
    uint8_t* pSrcData    = &src->data[0][0];
    uint8_t* pDstData    = &dst->data[0][0];
    uint8_t* pSrcDataEnd = &src->data[IMG_HEIGHT-1][IMG_WIDTH-1];

    // Debug
    QDEBUG("> vFillHoles: Begin");

    // Markeer border pixels die waarde 0 hebben
    vMarkBorders(dst, 0);

    // Markeer achtergrond
    vMarkConnectedToBorders (dst, 0, connected);

    // Vul gaten en verwijder gemarkeerde achtergrond pixels
    for ( ; pSrcData <= pSrcDataEnd; pSrcData++, pDstData++ )
    {
        if      (*pDstData == 0)    { *pDstData = 1; }  // Vul gaten
        else if (*pDstData == MARK) { *pDstData = 0; }  // Verwijder achtergrond
    }

    // Debug info
    QDEBUG("> vFillHoles: Done");
}


// ----------------------------------------------------------------------------
// vRemoveBorderBlobs
// ----------------------------------------------------------------------------
void vRemoveBorderBlobs(image_t *src,           // must be a binary image
                        image_t *dst,
                        eConnected connected)   // FOUR | EIGHT
{
    // Declaraties variabelen
    uint8_t* pSrcData    = &src->data[0][0];
    uint8_t* pDstData    = &dst->data[0][0];
    uint8_t* pSrcDataEnd = &src->data[IMG_HEIGHT-1][IMG_WIDTH-1];

    // Debug
    QDEBUG("> vRemoveBorderBlobs: Begin");

    // Markeer border pixels die waarde 1 hebben en Markeer borderblobs
    vMarkBorders(dst, 1);
    vMarkConnectedToBorders (dst, 1, connected);

    // Verwijder alle gemarkeerde pixels / blobs
    for ( ; pSrcData <= pSrcDataEnd; pSrcData++, pDstData++ )
    {
        if (*pDstData == MARK) { *pDstData  = 0; }      // Gemarkeerde pixel?
    }

    // Debug info
    QDEBUG("> vRemoveBorderBlobs: Done");
}


// ----------------------------------------------------------------------------
// vFindEdges
// ----------------------------------------------------------------------------
void vFindEdges(image_t *src, // must be a binary image
                image_t *dst,
                eConnected connected)
{
    // Declaraties variabele
    uint16_t x,y = 0;

    // Debug
    QDEBUG("> vFindEdges: Begin");

    // Markeer border pixels die waarde 0 hebben en Markeer achtergrond
    vSetBorders(src, dst, MARK);
    vMarkConnectedToBorders (dst, 0, connected);

    // Markeer randen
    for ( y = 0; y <= dst->height; y++ )
    for ( x = 0; x <= dst->width;  x++ )
    {
        if ( dst->data[y][x] == 1 && iNeighbourCount(dst, x, y, MARK, connected) ) {
             dst->data[y][x] = MARK+1;
        }
    }

    // Verwijder alles behalve de gemarkeerde randen
    for ( y = 0; y <= dst->height; y++ )
    for ( x = 0; x <= dst->width;  x++ )
    {
        dst->data[y][x] = (dst->data[y][x] == (MARK + 1) ) ? (1) : (0);
    }

    // Debug info
    QDEBUG("> vFindEdges: Done");
}


// ----------------------------------------------------------------------------
// iLabelBlobs
// ----------------------------------------------------------------------------
uint32_t iLabelBlobs(image_t *src, // must be a binary image
                     image_t *dst,
                     eConnected connected)
{
    // Declaraties variabelen
    uint8_t changed   = 1;
    uint8_t blobCount = 1;
    uint16_t x,y      = 0;
    uint8_t smallestNeighbour = 0xFF;

    // Debug
    QDEBUG("> iLabelBlobs: Begin");

    // Zet alle voorgrond pixels op 255
    vSetSelectedToValue(src, dst, 1, 0xFF);

    // Label blobs
    while (changed == 1)
    {
        changed = 0;

        for ( y = 0; y != dst->height; y++ )
        for ( x = 0; x != dst->width;  x++ )
        {
            // Huidige pixel geen achtergrond EN een buurpixel heeft die kleiner is
            if (dst->data[y][x] != 0)
            {
                smallestNeighbour = dst->data[y][x];

                // Kleinere buurpixel aanwezig, Four connected?
                if ( (dst->data[y][x+1] != 0) && (dst->data[y][x+1] < smallestNeighbour) ) { smallestNeighbour = dst->data[y][x+1]; }
                if ( (dst->data[y][x-1] != 0) && (dst->data[y][x-1] < smallestNeighbour) ) { smallestNeighbour = dst->data[y][x-1]; }
                if ( (dst->data[y+1][x] != 0) && (dst->data[y+1][x] < smallestNeighbour) ) { smallestNeighbour = dst->data[y+1][x]; }
                if ( (dst->data[y-1][x] != 0) && (dst->data[y-1][x] < smallestNeighbour) ) { smallestNeighbour = dst->data[y-1][x]; }

                // Kleinere buurpixel aanwezig, Eight connected?
                if (connected == EIGHT)
                {
                    if ( (dst->data[y+1][x+1] != 0) && (dst->data[y+1][x+1] < smallestNeighbour) ) { smallestNeighbour = dst->data[y+1][x+1]; }
                    if ( (dst->data[y+1][x-1] != 0) && (dst->data[y+1][x-1] < smallestNeighbour) ) { smallestNeighbour = dst->data[y+1][x-1]; }
                    if ( (dst->data[y+1][x+1] != 0) && (dst->data[y+1][x+1] < smallestNeighbour) ) { smallestNeighbour = dst->data[y+1][x+1]; }
                    if ( (dst->data[y-1][x+1] != 0) && (dst->data[y-1][x+1] < smallestNeighbour) ) { smallestNeighbour = dst->data[y-1][x+1]; }
                }

                // Heeft een buurpixel die kleiner is?
                if ( smallestNeighbour < dst->data[y][x] )
                {
                    dst->data[y][x] = smallestNeighbour;
                    changed = 1;
                }

                // Anders ALS huidige pixel nog gemarkeerd is met 255
                else if ( dst->data[y][x] == 0xFF )
                {
                    dst->data[y][x] = blobCount++;  //  Maak huidige pixel gelijk aan blob_cnt
                    changed = 1;
                }
            }
        }

        // Debug info
        QDEBUG("> iLabelBlobs: Loop, blobcount = " << blobCount);
    }

    // Zet de labels in de juiste volgorde
    QDEBUG("> iLabelBlobs: Arranging labels");
    blobCount = 0;

    for ( y = 0; y != dst->height; y++ )
    for ( x = 0; x != dst->width;  x++ )
    {
        // ALS huidige pixel is gelabeld EN groter dan blob_cnt
        if ( (dst->data[y][x] != 0) && (dst->data[y][x] > blobCount) )
        {
            blobCount++;
            vSetSelectedToValue(dst, dst, dst->data[y][x], blobCount);
        }
    }

    // Debug info
    QDEBUG("> iLabelBlobs: Done, final blobcount = " << blobCount);

    // Prevent 'no return statement' warning
    // Remove this code when implementing starts
    return(blobCount);
}


// ----------------------------------------------------------------------------
// vBlobAnalyse
// ----------------------------------------------------------------------------
void vBlobAnalyse(image_t *img,
                  const uint8_t blobcount,
                  blobinfo_t *pBlobInfo)
{
    // Declaratie variabelen
    uint16_t i  = 0;
    uint16_t x  = 0;
    uint16_t x0 = 0;
    uint16_t x1 = 0;
    uint16_t y  = 0;
    uint16_t y0 = 0;
    uint16_t y1 = 0;

    // Debug
    QDEBUG("> vBlobAnalyse: Begin");

    // Analyseer blobs
    for ( i = 0; i != blobcount; i++ )
    {
			pBlobInfo[i].nof_pixels = 0;
			pBlobInfo[i].perimeter  = 0;
			
      // Bereken
      for ( y = 0; y != img->height; y++ )
      for ( x = 0; x != img->width;  x++ )
      {
          // Als de huidige pixel de gewenste blob waarde heeft
          if (img->data[y][x] == (i+1))
          {
              // Tel aantal pixels
              pBlobInfo[i].nof_pixels++;

              // Vind hoogte en breedte
              if (x < x0) { x0 = x; }
              if (y < y0) { y0 = y; }
              if (x > x1) { x1 = x; }
              if (y > y1) { y1 = y; }

              // Bereken omtrek gebasseerd op randen pixel
              switch( iNeighbourCount(img, x, y, 0, FOUR) )
              {
                case 1: pBlobInfo[i].perimeter += 1.0000; break;	// sqrt(1)
                case 2: pBlobInfo[i].perimeter += 1.4142; break;	// sqrt(2)
                case 3: pBlobInfo[i].perimeter += 2.2360; break;	// sqrt(5)
                default: break;
              }
          }
      }

      // Bereken dimensies en formFactorpBlobInfo[i].nof_pixels
      pBlobInfo[i].height = x1 - x0;
      pBlobInfo[i].width  = y1 - y0;

      // Debug
      QDEBUG("> vBlobAnalyse: Blob #" << (i+1) << ", height = "     << pBlobInfo[i].height     << ", width = "     << pBlobInfo[i].width);
      QDEBUG("> vBlobAnalyse: Blob #" << (i+1) << ", pixels = "     << pBlobInfo[i].nof_pixels << ", perimeter = " << pBlobInfo[i].perimeter);
    }

    QDEBUG("> vBlobAnalyse: Done");
}


// ----------------------------------------------------------------------------
// vCentroid
// ----------------------------------------------------------------------------
void vCentroid(image_t *img, uint8_t blobnr, uint8_t *xc, uint8_t *yc)
{

    uint32_t x,y = 0;
    uint32_t m00 = 0;
    uint32_t m10 = 0;
    uint32_t m01 = 0;

    // Debug
    QDEBUG("> vCentroid: Begin, blobnr. = " << blobnr);

    // Bereken momenten
    for ( y = 0; y != img->height; y++ )
    for ( x = 0; x != img->width;  x++ )
    {
        if (img->data[y][x] == blobnr )
        {
            m00++;
            m10 += x;
            m01 += y;
        }
    }

    // Bereken centroid
    *xc = (m10 != 0 && m00 != 0) ? (m10 / m00) : (0);
    *yc = (m01 != 0 && m00 != 0) ? (m01 / m00) : (0);

    // Debug info
    QDEBUG("> vCentroid: Done, xc = " << *xc << " yc = " << *yc);
}


// ----------------------------------------------------------------------------
// dNormalizedCentralMoments
// ----------------------------------------------------------------------------
double dNormalizedCentralMoments(image_t *img, uint8_t blobnr, int p, int q)
{
    // http://en.wikipedia.org/wiki/Image_moment

    uint8_t xc,yc = 0;
    uint32_t x,y  = 0;
    uint32_t u00  = 0;

    double ncm = 0.0;
    double upq = 0.0;

    // Debug
    QDEBUG("> dNormalizedCentralMoments: Begin, blobnr. = " << blobnr);

    // Condition check
    if ( (p == 0 && q == 1) || (p == 1 && q == 0) ) {
         return 0.0;
    }
    else if (p == 0 && q == 0) {
        return 1.0;
    }
    else if (img->lut == LUT_BINARY) {
        blobnr = 1;
    }

    // Bereken centroid, Xc en Yc
    vCentroid(img, blobnr, &xc, &yc);

    // Bereken Upq
    for ( y = 0; y != img->height; y++ )
    for ( x = 0; x != img->width;  x++ )
    {
        if (img->data[y][x] == blobnr )
        {
            u00++;
            upq += pow((int32_t)(x - xc), p) * pow((int32_t)(y - yc), q);
        }
    }

    // Bereken Ncm
    ncm = upq / (u00 * u00);

    QDEBUG("> dNormalizedCentralMoments: Done, ncm = " << ncm);

    // Geef berekende genormaliseerde central moments
    return( ncm );
}


// ----------------------------------------------------------------------------
// vMarkConnectedToBorders
// ----------------------------------------------------------------------------
void vMarkConnectedToBorders (image_t *img,
                              uint8_t value,    // Te markeren waarden
                              eConnected connected)
{
    // Declaraties variabelen
    uint8_t changed = 1;
    uint16_t x0,x1,y0,y1 = 0;

    // Debug
    QDEBUG("> vMarkBackground: Begin, mark = " << value);

    // Markeer achtergrond
    while( changed == 1 )
    {
        changed = 0;

        // Kolommen
        for( y0 = x0 = 1, x1 = img->width-1, y1 = img->height-1;  x0 <= x1;  y0++, y1-- )
        {
            // Link, boven naar onder
            if (img->data[y0][x0] == value && iNeighbourCount(img,x0,y0,MARK,connected) != 0) {
                img->data[y0][x0] = MARK; changed = 1;
            }

            // Rechts, boven naar onder
            if (img->data[y0][x1] == value && iNeighbourCount(img,x1,y0,MARK,connected) != 0) {
                img->data[y0][x1] = MARK; changed = 1;
            }

            // Link, onder naar boven
            if (img->data[y1][x0] == value && iNeighbourCount(img,x0,y1,MARK,connected) != 0) {
                img->data[y1][x0] = MARK; changed = 1;
            }

            // Rechts, onder naar boven
            if (img->data[y1][x1] == value && iNeighbourCount(img,x1,y1,MARK,connected) != 0) {
                img->data[y1][x1] = MARK; changed = 1;
            }
						
						// Volgende kolom
            if (y0 == img->height || y1 == 0)
            {
                y0 = 1;
                y1 = img->height-1;
                x0++;
                x1--;
            }
        }

        // Rijen
        for( x0 = y0 = 1, x1 = img->width-1, y1 = img->height-1;  y0 <= y1;  x0++, x1-- )
        {
            // Boven, links naar rechts
            if (img->data[y0][x0] == value && iNeighbourCount(img,x0,y0,MARK,connected) != 0) {
                img->data[y0][x0] = MARK; changed = 1;
            }

            // Onder, links naar rechts
            if (img->data[y1][x0] == value && iNeighbourCount(img,x0,y1,MARK,connected) != 0) {
                img->data[y1][x0] = MARK; changed = 1;
            }

            // Boven, rechts naar links
            if (img->data[y0][x1] == value && iNeighbourCount(img,x1,y0,MARK,connected) != 0) {
                img->data[y0][x1] = MARK; changed = 1;
            }

            // Onder, rechts naar links
            if (img->data[y1][x1] == value && iNeighbourCount(img,x1,y1,MARK,connected) != 0) {
                img->data[y1][x1] = MARK; changed = 1;
            }
						
						// Volgende rij
            if (x0 == img->width || x1 == 0)
            {
                x0 = 1;
                x1 = img->width-1;
                y0++;
                y1--;
            }
        }

        // Debug info
        QDEBUG("> vMarkBackground: Loop");
    }

    // Debug info
    QDEBUG("> vMarkBackground: Done");
}


// ----------------------------------------------------------------------------
// iNeighbourCount
// ----------------------------------------------------------------------------
uint8_t iNeighbourCount(image_t *img,
                        uint16_t x,
                        uint16_t y,
                        uint8_t value,
                        eConnected connected)
{
    uint32_t count = 0;

    // Four connected
    if ( (x != 0)          && (img->data[y][x-1] == value) ) { count++; }
    if ( (y != 0)          && (img->data[y-1][x] == value) ) { count++; }
    if ( (y < img->height) && (img->data[y+1][x] == value) ) { count++; }
    if ( (x < img->width)  && (img->data[y][x+1] == value) ) { count++; }

    // Eight connected
    if (connected == EIGHT)
    {
        if ( (y < img->height) && (x < img->width) && (img->data[y+1][x+1] == value) ) { count++; }
        if ( (y < img->height) && (x != 0)         && (img->data[y+1][x-1] == value) ) { count++; }
        if ( (y != 0)          && (x < img->width) && (img->data[y-1][x+1] == value) ) { count++; }
        if ( (y != 0)          && (x != 0)         && (img->data[y-1][x-1] == value) ) { count++; }
    }

    // Debug info
    //QDEBUG("iNeighbourCount: Done!");   // Spam

    // OK
    return count;
}


// ----------------------------------------------------------------------------
// iNeighboursEqualOrHigher
// ----------------------------------------------------------------------------
uint8_t iNeighboursEqualOrHigher(image_t *img,
                                 uint16_t x,
                                 uint16_t y,
                                 uint8_t value,
                                 eConnected connected)
{
    uint32_t count = 0;

    // Four connected
    if ( (x != 0)          && (img->data[y][x-1] >= value) ) { count++; }
    if ( (y != 0)          && (img->data[y-1][x] >= value) ) { count++; }
    if ( (y < img->height) && (img->data[y+1][x] >= value) ) { count++; }
    if ( (x < img->width)  && (img->data[y][x+1] >= value) ) { count++; }

    // Eight connected
    if (connected == EIGHT)
    {
        if ( (y < img->height) && (x < img->width) && (img->data[y+1][x+1] >= value) ) { count++; }
        if ( (y < img->height) && (x != 0)         && (img->data[y+1][x-1] >= value) ) { count++; }
        if ( (y != 0)          && (x < img->width) && (img->data[y-1][x+1] >= value) ) { count++; }
        if ( (y != 0)          && (x != 0)         && (img->data[y-1][x-1] >= value) ) { count++; }
    }

    // Debug info
    //QDEBUG("iNeighbourCount: Done!");   // Spam

    // OK
    return count;
}


// ----------------------------------------------------------------------------
// vDrawLine
// ----------------------------------------------------------------------------
void vDrawLine (image_t *img, const point_t start, const point_t end, const uint8_t size, const uint8_t value)
{
    // http://en.wikipedia.org/wiki/Bresenham's_line_algorithm
    // Teken lijn van start x,y -> end x,y, met dikte size en kleurwaarde value

    const int32_t dX    = abs(end.x - start.x);             // Delta X
    const int32_t dY    = abs(end.y - start.y);             // Delta Y
    const int32_t sX    = (start.x < end.x) ? (1) : (-1);   // Richting X
    const int32_t sY    = (start.y < end.y) ? (1) : (-1);   // Richting Y
    const uint8_t width = (size == 0) ? (0) : ((size - 1) / 2);

    uint32_t x  = start.x;
    uint32_t y  = start.y;
    int32_t err = (dX > dY ? dX : -dY) / 2;
    int32_t e2  = 0;
    uint8_t i   = 0;

    // Debug
    QDEBUG("> vDrawLine: Begin, size = " << size << " with value = " << value << " from " << start.x << "." << start.y << " to " << end.x << "." << end.y );

    // Teken lijn
    while( !(x == end.x && y == end.y) )
    {
        e2 = err;

        // Teken pixel
        img->data[y][x] = value;

        // Teken rondom
        for ( i = -width; i <= width; i++ )
        {
            // Four
            img->data[y][x+i]   = value;
            img->data[y][x-i]   = value;
            img->data[y+i][x]   = value;
            img->data[y-i][x]   = value;

            // Eight
            img->data[y+i][x+i] = value;
            img->data[y+i][x-i] = value;
            img->data[y-i][x+i] = value;
            img->data[y-i][x-i] = value;
        }

        if (e2 > -dX) { err -= dY; x += sX; } // Volgende X
        if (e2 <  dY) { err += dX; y += sY; } // Volgende Y
    }

    // Debug info
    QDEBUG("> Done!" );
}


// ----------------------------------------------------------------------------
// eGetShape
// ----------------------------------------------------------------------------
form_t eGetShape (double* im1, double* im2)
{
    if      ( *im1 < 0.162 && *im1 > 0.155 && *im2 < 1.0e-05) { return CIRCLE;   }
    else if ( *im1 < 0.170 && *im1 > 0.162 && *im2 < 1.0e-06) { return SQUARE;   }
    else if ( *im1 < 0.250 && *im1 > 0.162 && *im2 > 1.0e-03) { return TRIANGLE; }
    else                                                      { return UNKNOWN;  }
}


// ----------------------------------------------------------------------------
// vCropVertical
// ----------------------------------------------------------------------------
void vCropVertical (image_t *src, // must be a binary image
                    image_t *dst,
                    uint8_t val,
                    uint8_t y0,
                    uint8_t y1 )
{
    // Declaraties variabelen
    uint8_t x,y = 0;

    // Behoud alleen region-of-interrest
    for (y = 0; y != src->height; y++)
    for (x = 0; x != src->width;  x++)
    {
        if ( y >= y0 && y <= y1 )
            dst->data[y][x] = src->data[y][x];
        else
            dst->data[y][x] = val;
    }
}

/**
 *  @brief Verifies that the roi is 32 bit aligned
 *  @param[in] img      image_roi_t
 *  @return same or corrected image_roi_t
 */
image_roi_t sRoiValidate(image_roi_t roi){
    #warning EVD1 To be implemented: sRoiValidate
    return roi;
}

/**
 *  @brief converts image of 7 segment to BCD
 *  @param[in] img      input image_t (max 32x32 pixels)
 *  @return bcd
 */
int imageToBCD(image_t *img, image_roi_t roi){
    const uint8_t digit_list[10][7] = {
        /* A ,  B ,  C ,  D ,  E ,  F ,  G */
        {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00},   // 0 - ABCDEF
        {0x00,0xFF,0xFF,0x00,0x00,0x00,0x00},   // 1 - BC
        {0xFF,0xFF,0x00,0xFF,0xFF,0x00,0xFF},   // 2 - ABDEG
        {0xFF,0xFF,0xFF,0xFF,0x00,0x00,0xFF},   // 3 - ABCDF
        {0x00,0xFF,0xFF,0x00,0x00,0xFF,0xFF},   // 4 - BCFG
        {0xFF,0x00,0xFF,0xFF,0x00,0xFF,0xFF},   // 5 - ACDFG
        {0xFF,0x00,0xFF,0xFF,0xFF,0xFF,0xFF},   // 6 - ACDEFG
        {0xFF,0xFF,0xFF,0x00,0x00,0xFF,0x00},   // 7 - ABCF
        {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF},   // 8 - ABCDEFG
        {0xFF,0xFF,0xFF,0xFF,0x00,0xFF,0xFF}    // 9 - ABCDFG
    };
    uint8_t segments[7] = {0,0,0,0,0,0,0};
		float digit_width;
    float digit_heigth;
    float aspect_ratio;
		uint16_t start_x, end_x;
    uint16_t start_y, end_y;
    uint16_t w;
    uint16_t h;
		uint16_t img_half_x;
    uint16_t img_half_y_top;
    uint16_t img_half_y_bottom;
    uint16_t img_start_x;
    uint16_t img_end_x;
    uint16_t img_start_y;
    uint16_t img_end_y;
		uint8_t compare;
		char segmentchars[7];
		int result;
		
    #define SEGMENT_A   0
    #define SEGMENT_B   1
    #define SEGMENT_C   2
    #define SEGMENT_D   3
    #define SEGMENT_E   4
    #define SEGMENT_F   5
    #define SEGMENT_G   6
    static uint32_t x;
    static uint32_t y;


    /* Step 1: Find beginning and end of digit */
    start_x = 0xFFFF; end_x = 0;
    start_y = 0xFFFF; end_y = 0;
    w = roi.w + roi.x;
    h = roi.h + roi.y;
    for(y = roi.y; y<h; y++){
        for(x = roi.x; x<w; x++){
            if(img->data[y][x]){
                // Relative x and y
                uint16_t rx = x - roi.x;
                uint16_t ry = y - roi.y;
                if(rx < start_x)
                    start_x = rx;
                if(rx > end_x)
                    end_x = rx;
                if(ry < start_y)
                    start_y = ry;
                if(ry > end_y)
                    end_y = ry;
            }
        }
    }
    // Offset fix
    end_x++; end_y++;


    // Step 2:
    // We know the ratios of 7 segment displays
    // Check aspect ratio of 0.5 for everything except the 1
    digit_width  = end_x - start_x;
    digit_heigth = end_y - start_y;
    aspect_ratio = digit_width / digit_heigth;
    if( aspect_ratio < 0.5 ){
        // This might be a 1
        return 1;
    }
    if( aspect_ratio > 0.6 ){
        return -1; // Invalid character
    }

    // Scan for vertical segments at 1/3 and 2/3 of digit heigth
    img_half_x         = roi.x + (start_x + (digit_width / 2));
    img_half_y_top     = roi.y + (start_y + (digit_heigth / 3));
    img_half_y_bottom  = roi.y + (end_y - (digit_heigth / 3));
    img_start_x        = roi.x + start_x;
    img_end_x          = roi.x + start_x + (end_x - start_x);
    img_start_y        = roi.y + start_y;
    img_end_y          = roi.y + start_y + (end_y - start_y);
    // Segments F and E (left)
    for(x = img_start_x; x < img_half_x; x++){
        if(img->data[img_half_y_top][x]){
            segments[SEGMENT_F]++;
        }
        if(img->data[img_half_y_bottom][x]){
            segments[SEGMENT_E]++;
        }
        //img->data[img_half_y_top][x] = 1;
        //img->data[img_half_y_bottom][x] = 2;
    }
    // Segments B and C (right)
    for(x = img_half_x; x < img_end_x; x++){
        if(img->data[img_half_y_top][x]){
            segments[SEGMENT_B]++;
        }
        if(img->data[img_half_y_bottom][x]){
            segments[SEGMENT_C]++;
        }
        //img->data[img_half_y_top][x] = 3;
        //img->data[img_half_y_bottom][x] = 4;
    }

    // Scan for horizontal segments at 1/2 of start_x and end_x
    // Segment A
    for(y = img_start_y; y<img_half_y_top; y++){
        if(img->data[y][img_half_x]){
            segments[SEGMENT_A]++;
        }
        //img->data[y][img_half_x] = 5;
    }
    // Segments G
    for(y = img_half_y_top; y<img_half_y_bottom; y++){
        if(img->data[y][img_half_x]){
            segments[SEGMENT_G]++;
        }
        //img->data[y][img_half_x] = 6;
    }
    // Segments D
    for(y = img_half_y_bottom; y<img_end_y; y++){
        if(img->data[y][img_half_x]){
            segments[SEGMENT_D]++;
        }
        //img->data[y][img_half_x] = 7;
    }


    segmentchars[0] = 'A';
		segmentchars[1] = 'B';
		segmentchars[2] = 'C';
		segmentchars[3] = 'D';
		segmentchars[4] = 'E';
		segmentchars[5] = 'F';
		segmentchars[6] = 'G';

    // Step 3: Lookup set segments to a number
    
    result = -1;
    for(y=0; y<10; y++){    // known digit_list
        compare = 0;
        for(x=0; x<7; x++){ // found set elements
            if(digit_list[y][x]){
                if(segments[x]){
                    compare++;
                }else{
                    compare = 0; break;
                }
            }else{
                if(segments[x]){
                    compare = 0; break;
                }else{
                    compare++;
                }
            }
        }
        if(compare){
            result = y;
            break;
        }
    }
    if(y >= 10) // No char in databse
        result = -2;

    //QDEBUG("SubRoi from x: " << start_x << " to " << end_x);
    //QDEBUG("SubRoi from y: " << start_y << " to " << end_y);
    //QDEBUG("SubRoi aspect ratio: " << aspect_ratio );


    for(x=0; x<7; x++)
        QDEBUG("Set segments: " << segmentchars[x] << " = " << segments[x]);

    QDEBUG("Found " << result);
    return result;
}

// ----------------------------------------------------------------------------
// EOF
// ----------------------------------------------------------------------------
