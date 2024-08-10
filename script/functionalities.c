#include <stdio.h>
#include "functionalities.h"

// ANSI escape codes for text formatting
#define BOLD "\033[1m"
#define RESET "\033[0m"

void display_functionalities() {
    printf("********************************************************\n");
    printf(BOLD "Image Processing Functionalities:" RESET "\n");
    printf("********************************************************\n");

    // Grayscale Conversion
    printf("1. " BOLD "Grayscale Conversion:" RESET "\n");
    printf("   - " BOLD "Algorithm:" RESET " Converts a color image to grayscale by averaging the red, green, and blue values of each pixel.\n");
    printf("   - " BOLD "Formula:" RESET " Gray = 0.299 * Red + 0.587 * Green + 0.114 * Blue\n");
    printf("   - " BOLD "C Implementation:" RESET " Reads each pixel's RGB values and calculates the grayscale value.\n");
    printf("   - " BOLD "CUDA Implementation:" RESET " Parallelizes the conversion process across multiple GPU threads.\n");
    printf("   - " BOLD "OpenMP Implementation:" RESET " Uses multiple threads to process different portions of the image.\n");
    printf("********************************************************\n");

    // Intensity Normalization
    printf("2. " BOLD "Intensity Normalization:" RESET "\n");
    printf("   - " BOLD "Algorithm:" RESET " Adjusts pixel intensity values to enhance contrast and uniformity.\n");
    printf("   - " BOLD "Formula:" RESET " Normalized Value = (Value - Min) / (Max - Min)\n");
    printf("   - " BOLD "C Implementation:" RESET " Iterates through each pixel to calculate and set the normalized intensity.\n");
    printf("   - " BOLD "CUDA Implementation:" RESET " Utilizes GPU threads to handle pixel intensity adjustments concurrently.\n");
    printf("   - " BOLD "OpenMP Implementation:" RESET " Distributes the intensity normalization tasks across multiple threads.\n");
    printf("********************************************************\n");

    // Edge Detection
    printf("3. " BOLD "Edge Detection:" RESET "\n");
    printf("   - " BOLD "Algorithm:" RESET " Detects edges in an image using methods like the Canny edge detector.\n");
    printf("   - " BOLD "Formula:" RESET " Involves convolution with a Sobel filter followed by non-maximum suppression and hysteresis thresholding.\n");
    printf("   - " BOLD "C Implementation:" RESET " Applies convolution and edge detection algorithms sequentially.\n");
    printf("   - " BOLD "CUDA Implementation:" RESET " Accelerates the edge detection process by applying the filter in parallel on the GPU.\n");
    printf("   - " BOLD "OpenMP Implementation:" RESET " Uses threading to parallelize the edge detection across different regions of the image.\n");
    printf("********************************************************\n");

    // Histogram Equalization
    printf("4. " BOLD "Histogram Equalization:" RESET "\n");
    printf("   - " BOLD "Algorithm:" RESET " Enhances the contrast of an image by redistributing the intensity levels.\n");
    printf("   - " BOLD "Formula:" RESET " Cumulative Distribution Function (CDF) is used to map pixel values to a new range.\n");
    printf("   - " BOLD "C Implementation:" RESET " Computes histograms and equalizes them sequentially.\n");
    printf("   - " BOLD "CUDA Implementation:" RESET " Uses GPU threads to compute histograms and apply equalization concurrently.\n");
    printf("   - " BOLD "OpenMP Implementation:" RESET " Distributes histogram computation and equalization tasks across multiple threads.\n");
    printf("********************************************************\n");

    // Gaussian Blur
    printf("5. " BOLD "Gaussian Blur:" RESET "\n");
    printf("   - " BOLD "Algorithm:" RESET " Applies a Gaussian filter to blur an image and reduce noise.\n");
    printf("   - " BOLD "Formula:" RESET " Convolution with a Gaussian kernel where G(x, y) = (1 / (2 * PI * sigma^2)) * e^(-(x^2 + y^2) / (2 * sigma^2))\n");
    printf("   - " BOLD "C Implementation:" RESET " Performs convolution with a Gaussian kernel in a sequential manner.\n");
    printf("   - " BOLD "CUDA Implementation:" RESET " Applies the Gaussian filter in parallel using GPU threads.\n");
    printf("   - " BOLD "OpenMP Implementation:" RESET " Utilizes multiple threads to perform convolution operations concurrently.\n");
    printf("********************************************************\n");

    // Denoising
    printf("6. " BOLD "Denoising:" RESET "\n");
    printf("   - " BOLD "Algorithm:" RESET " Removes noise from an image using techniques like Wiener filtering.\n");
    printf("   - " BOLD "Formula:" RESET " Wiener Filter = (Noise Variance / (Noise Variance + Signal Variance)) * Original Image\n");
    printf("   - " BOLD "C Implementation:" RESET " Implements filtering sequentially to remove noise from the image.\n");
    printf("   - " BOLD "CUDA Implementation:" RESET " Uses GPU to apply the denoising filter in parallel.\n");
    printf("   - " BOLD "OpenMP Implementation:" RESET " Distributes the denoising task across multiple threads.\n");
    printf("********************************************************\n");
}
