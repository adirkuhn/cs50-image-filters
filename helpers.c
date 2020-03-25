#include "helpers.h"
#include <math.h>
#include <stdlib.h>

typedef struct {
    int sumBlue;
    int sumGreen;
    int sumRed;
    int items;
} SumItems;

void copyImage(int height, int width, RGBTRIPLE imageFrom[height][width], RGBTRIPLE imageTo[height][width])
{
    for (int h = 0; h < height; h++) {
        for (int w = 0; w < width; w++) {
            imageTo[h][w] = imageFrom[h][w];
        }
    }
}

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    BYTE avg;
    for (int h = 0; h < height; h++) {
        for (int w = 0; w < width; w++) {
            avg = (BYTE)((image[h][w].rgbtBlue + image[h][w].rgbtGreen + image[h][w].rgbtRed) / 3);

            image[h][w].rgbtBlue = avg;
            image[h][w].rgbtGreen = avg;
            image[h][w].rgbtRed = avg;
        }
    }
    return;
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    int delta = 1;
    int wMiddle = (int)width / 2;
    int replacementPixel = 0;
    RGBTRIPLE currentPixel;
    for (int h = 0; h < height; h++) {
        for (int w = 0; w < wMiddle; w++) {
            replacementPixel = width - (w + delta);
            currentPixel = image[h][w];

            image[h][w] = image[h][replacementPixel];
            image[h][replacementPixel] = currentPixel;
        }
    }
    return;
}

SumItems calculateRowAverage(int pixel, int width, RGBTRIPLE image[width])
{
    SumItems rowSumItems = {0, 0, 0, 0};

    int pixelLeft = pixel - 1;
    int pixelRight = pixel + 1;

    rowSumItems.sumBlue = image[pixel].rgbtBlue;
    rowSumItems.sumGreen = image[pixel].rgbtGreen;
    rowSumItems.sumRed =  image[pixel].rgbtRed;
    rowSumItems.items = 1;

    if (pixelLeft >= 0) {
        rowSumItems.sumBlue += image[pixelLeft].rgbtBlue;
        rowSumItems.sumGreen += image[pixelLeft].rgbtGreen;
        rowSumItems.sumRed += image[pixelLeft].rgbtRed;
        rowSumItems.items++;
    }

    if (pixelRight < width) {
        rowSumItems.sumBlue += image[pixelRight].rgbtBlue;
        rowSumItems.sumGreen += image[pixelRight].rgbtGreen;
        rowSumItems.sumRed += image[pixelRight].rgbtRed;
        rowSumItems.items++;
    }

    return rowSumItems;
}

/**
 * calculates the average from a 3x3 box from a middle point pixel
 *
 *  h-1 w-1 |  h-1 w  | h-1 w+1
 *  h   w-1 |  h   w  | h   w+1
 *  h+1 w-1 |  h+1 w  | h+1 w+1
 *
 */
SumItems calculateBlurAverage(int h, int w, int height, int width, RGBTRIPLE image[height][width])
{
    int rowAboveIndex = h - 1;
    int rowBelowIndex = h + 1;

    SumItems rgbAvg = {0, 0, 0, 0};
    SumItems rowAbove = {0, 0, 0, 0};
    SumItems rowBelow = {0, 0, 0, 0};
    SumItems currentRow = {0, 0, 0, 0};

    currentRow = calculateRowAverage(w, width, image[h]);

    if (rowAboveIndex >= 0) {
        rowAbove = calculateRowAverage(w, width, image[rowAboveIndex]);
    }

    if (rowBelowIndex < height) {
        rowBelow = calculateRowAverage(w, width, image[rowBelowIndex]);
    }

    int totalItems = (currentRow.items + rowAbove.items + rowBelow.items);
    rgbAvg.sumGreen = (currentRow.sumGreen + rowAbove.sumGreen + rowBelow.sumGreen) / totalItems;
    rgbAvg.sumBlue = (currentRow.sumBlue + rowAbove.sumBlue + rowBelow.sumBlue) / totalItems;
    rgbAvg.sumRed = (currentRow.sumRed + rowAbove.sumRed + rowBelow.sumRed) / totalItems;

    return rgbAvg;
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    // Allocate memory for new image
    RGBTRIPLE (*imageBlur)[width] = calloc(height, width * sizeof(RGBTRIPLE));

    SumItems rgbAvg;

    for (int h = 0; h < height; h++) {
        for (int w = 0; w < width; w++) {
            rgbAvg = calculateBlurAverage(h, w, height, width, image);

            imageBlur[h][w].rgbtRed = rgbAvg.sumRed;
            imageBlur[h][w].rgbtGreen = rgbAvg.sumGreen;
            imageBlur[h][w].rgbtBlue = rgbAvg.sumBlue;
        }
    }

    copyImage(height, width, imageBlur, image);

    free(imageBlur);

    return;
}

typedef struct {
    int gx;
    int gy;
} Sobel;

typedef struct {
    Sobel red;
    Sobel green;
    Sobel blue;
} RGBSobel;

RGBSobel initializeSobel()
{
    RGBSobel s = {
            .red = {0, 0},
            .green = {0, 0},
            .blue = {0 , 0},
    };

    return s;
}

RGBSobel calculateGxGy(int pixel, int gx[3], int gy[3], int width, RGBTRIPLE image[width])
{
    RGBSobel rgbSobel = initializeSobel();

    int pixelLeft = pixel - 1;
    int pixelRight = pixel  + 1;

    // middle gx
    rgbSobel.red.gx += image[pixel].rgbtRed * (gx[1]);
    rgbSobel.green.gx += image[pixel].rgbtGreen * (gx[1]);
    rgbSobel.blue.gx += image[pixel].rgbtBlue * (gx[1]);

    // middle gy
    rgbSobel.red.gy += image[pixel].rgbtRed * (gy[1]);
    rgbSobel.green.gy += image[pixel].rgbtGreen * (gy[1]);
    rgbSobel.blue.gy += image[pixel].rgbtBlue * (gy[1]);


    if (pixelLeft > -1) {
        // left gx
        rgbSobel.red.gx += image[pixelLeft].rgbtRed * (gx[0]);
        rgbSobel.green.gx += image[pixelLeft].rgbtGreen * (gx[0]);
        rgbSobel.blue.gx += image[pixelLeft].rgbtBlue * (gx[0]);

        // left gy
        rgbSobel.red.gy += image[pixelLeft].rgbtRed * (gy[0]);
        rgbSobel.green.gy += image[pixelLeft].rgbtGreen * (gy[0]);
        rgbSobel.blue.gy += image[pixelLeft].rgbtBlue * (gy[0]);
    }

    if (pixelRight < width) {
        // right gx
        rgbSobel.red.gx += image[pixelRight].rgbtRed * (gx[2]);
        rgbSobel.green.gx += image[pixelRight].rgbtGreen * (gx[2]);
        rgbSobel.blue.gx += image[pixelRight].rgbtBlue * (gx[2]);

        //right gy
        rgbSobel.red.gy += image[pixelRight].rgbtRed * (gy[2]);
        rgbSobel.green.gy += image[pixelRight].rgbtGreen * (gy[2]);
        rgbSobel.blue.gy += image[pixelRight].rgbtBlue * (gy[2]);
    }

    return rgbSobel;
}

BYTE calculateSobel(Sobel current, Sobel above, Sobel bellow)
{
    int sumGx = 0;
    int sumGy = 0;
    int sobel = 0;

    sumGx = current.gx + above.gx + bellow.gx;
    sumGy = current.gy + above.gy + bellow.gy;

    sobel = (int)sqrt((sumGx * sumGx) + (sumGy * sumGy));

    if (sobel > 255) {
        sobel = 255;
    }

    return (BYTE)sobel;
}

/**
 * Calculate the sobel value to each color channel
 *
 * gx
 * -1  0  1
 * -2  0  2
 * -1  0  1
 *
 *
 * gy
 * -1  -2  -1
 *  0   0   0
 *  1   2   1
 */
RGBTRIPLE calculateEdges(int row, int pixel, int height, int width, RGBTRIPLE image[height][width])
{
    RGBTRIPLE edges;
    RGBSobel currentRowRGBSobel = initializeSobel();
    RGBSobel aboveRowRGBSobel = initializeSobel();
    RGBSobel belowRowRGBSobel = initializeSobel();

    int rowAboveIndex = row - 1;
    int rowBelowIndex = row + 1;

    int gx[3][3] = {
            {-1, 0, 1},
            {-2, 0, 2},
            {-1, 0, 1}
    };
    int gy[3][3] = {
            {-1, -2, -1},
            {0, 0, 0},
            {1, 2, 1}
    };

    /*
     * current row calculates
     * the middle gx [-2  0  2]
     * the middle gy [ 0  0  0]
     */
    currentRowRGBSobel = calculateGxGy(pixel, gx[1], gy[1], width, image[row]);

    /*
     * checks if there is a row above and calculates
     * the top gx [-1   0   1]
     * the top gy [-1  -2  -1]
     */
    if (rowAboveIndex > -1) {
        aboveRowRGBSobel = calculateGxGy(pixel, gx[0], gy[0], width, image[rowAboveIndex]);
    }

    /*
     * checks if there is a row below and calculates
     * the below gx [-1, 0, 1]
     * the below gy [ 1, 2, 1]
     */
    if (rowBelowIndex < height) {
        belowRowRGBSobel = calculateGxGy(pixel, gx[2], gy[2], width, image[rowBelowIndex]);
    }


    edges.rgbtRed = calculateSobel(currentRowRGBSobel.red, aboveRowRGBSobel.red, belowRowRGBSobel.red);
    edges.rgbtGreen = calculateSobel(currentRowRGBSobel.green, aboveRowRGBSobel.green, belowRowRGBSobel.green);
    edges.rgbtBlue = calculateSobel(currentRowRGBSobel.blue, aboveRowRGBSobel.blue, belowRowRGBSobel.blue);

    return edges;
}

// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    // Allocate memory for new image
    RGBTRIPLE (*imageWithEdges)[width] = calloc(height, width * sizeof(RGBTRIPLE));

    RGBTRIPLE rgbEdgeValues;
    for (int h = 0; h < height; h++) {
        for (int w = 0; w < width; w++) {
            rgbEdgeValues = calculateEdges(h, w, height, width, image);

            imageWithEdges[h][w].rgbtRed = rgbEdgeValues.rgbtRed;
            imageWithEdges[h][w].rgbtGreen = rgbEdgeValues.rgbtGreen;
            imageWithEdges[h][w].rgbtBlue = rgbEdgeValues.rgbtBlue;
        }
    }

    // copy the pixels
    // we could only update the pointer reference but dunno how to do it
    copyImage(height, width, imageWithEdges, image);

    free(imageWithEdges);
    return;
}
