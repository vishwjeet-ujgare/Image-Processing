#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <cuda_runtime.h>

// CUDA error checking macro
#define CUDA_CHECK(call) \
    do { \
        cudaError_t err = call; \
        if (err != cudaSuccess) { \
            fprintf(stderr, "CUDA error in file '%s' in line %i : %s.\n", __FILE__, __LINE__, cudaGetErrorString(err)); \
            exit(EXIT_FAILURE); \
        } \
    } while (0)

// CUDA kernel to compute histogram
__global__ void computeHistogramKernel(const unsigned char *data, int width, int height, int channels, int *histogram) {
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    if (idx < width * height * channels) {
        atomicAdd(&histogram[data[idx]], 1);
    }
}

// CUDA kernel to compute cumulative histogram
__global__ void computeCumulativeHistogramKernel(const int *histogram, int *cumulativeHistogram) {
    __shared__ int temp[256];
    int idx = threadIdx.x;
    
    // Load histogram values into shared memory
    if (idx < 256) {
        temp[idx] = histogram[idx];
    }
    __syncthreads();
    
    // Compute cumulative histogram
    if (idx < 256) {
        int sum = 0;
        for (int i = 0; i <= idx; i++) {
            sum += temp[i];
        }
        cumulativeHistogram[idx] = sum;
    }
}

// CUDA kernel to perform histogram equalization
__global__ void equalizeHistogramKernel(unsigned char *data, int width, int height, int channels, const int *cumulativeHistogram, int totalPixels) {
    int idx = threadIdx.x + blockIdx.x * blockDim.x;
    float scale = 255.0f / (totalPixels - 1);
    if (idx < width * height * channels) {
        data[idx] = (unsigned char)((cumulativeHistogram[data[idx]] * scale));
    }
}

// Function to save histogram as an image
void saveHistogramImage(const int *histogram, const char *filename, int width, int height) {
    unsigned char *histogramImage = (unsigned char *)malloc(width * height * 3);
    if (histogramImage == NULL) {
        fprintf(stderr, "Failed to allocate memory for histogram image.\n");
        return;
    }
    memset(histogramImage, 255, width * height * 3); // Initialize with white background

    int maxCount = 0;
    for (int i = 0; i < 256; ++i) {
        if (histogram[i] > maxCount) {
            maxCount = histogram[i];
        }
    }

    if (maxCount == 0) {
        printf("Histogram is empty. No data to display.\n");
        free(histogramImage);
        return;
    }

    for (int i = 0; i < 256; ++i) {
        int barHeight = (histogram[i] * height) / maxCount; // Compute the bar height
        for (int y = height - 1; y >= height - barHeight; --y) {
            int idx = (y * width + i) * 3;
            histogramImage[idx] = 0;     // Red
            histogramImage[idx + 1] = 0; // Green
            histogramImage[idx + 2] = 0; // Blue
        }
    }

    if (!stbi_write_jpg(filename, width, height, 3, histogramImage, 90)) {
        fprintf(stderr, "Failed to save histogram image.\n");
    }
    free(histogramImage);
}

// Function to calculate and print histogram statistics
void printHistogramStatistics(const int *histogram, int totalPixels) {
    int sum = 0;
    int min = 255, max = 0;
    int cumulative = 0;
    float mean, median, stddev;

    // Calculate mean
    for (int i = 0; i < 256; i++) {
        sum += histogram[i] * i;
        if (histogram[i] > 0) {
            if (i < min) min = i;
            if (i > max) max = i;
        }
    }
    mean = (float)sum / totalPixels;

    // Calculate median
    int half = totalPixels / 2;
    for (int i = 0; i < 256; i++) {
        cumulative += histogram[i];
        if (cumulative >= half) {
            median = i;
            break;
        }
    }

    // Calculate standard deviation
    float variance = 0.0f;
    for (int i = 0; i < 256; i++) {
        variance += histogram[i] * (i - mean) * (i - mean);
    }
    stddev = sqrt(variance / totalPixels);

    printf("Histogram Statistics:\n");
    printf("  Mean: %.2f\n", mean);
    printf("  Median: %.2f\n", median);
    printf("  Standard Deviation: %.2f\n", stddev);
    printf("  Min Value: %d\n", min);
    printf("  Max Value: %d\n", max);
}

int main() {
    char input_filename[256], output_filename[256];
    int width, height, channels;

    // Record start time in milliseconds
    clock_t start_time = clock();

    // Get input filename from the user
    printf("Enter the input image filename (with .jpg or .jpeg extension): ");
    scanf("%255s", input_filename);

    // Load image
    unsigned char* img = stbi_load(input_filename, &width, &height, &channels, 0);
    if (img == NULL) {
        printf("Error loading image %s\n", input_filename);
        return 1;
    }

    // Allocate memory on the device
    unsigned char *d_img;
    int *d_histogram, *d_cumulativeHistogram;
    int histogram[256] = {0};
    int cumulativeHistogram[256];
    int numPixels = width * height * channels;
    
    CUDA_CHECK(cudaMalloc((void**)&d_img, numPixels * sizeof(unsigned char)));
    CUDA_CHECK(cudaMalloc((void**)&d_histogram, 256 * sizeof(int)));
    CUDA_CHECK(cudaMalloc((void**)&d_cumulativeHistogram, 256 * sizeof(int)));

    // Copy image data to device
    CUDA_CHECK(cudaMemcpy(d_img, img, numPixels * sizeof(unsigned char), cudaMemcpyHostToDevice));
    CUDA_CHECK(cudaMemset(d_histogram, 0, 256 * sizeof(int)));

    // Compute histogram on device
    int threadsPerBlock = 256;
    int blocksPerGrid = (numPixels + threadsPerBlock - 1) / threadsPerBlock;
    computeHistogramKernel<<<blocksPerGrid, threadsPerBlock>>>(d_img, width, height, channels, d_histogram);
    CUDA_CHECK(cudaGetLastError());
    CUDA_CHECK(cudaDeviceSynchronize());

    // Compute cumulative histogram on device
    computeCumulativeHistogramKernel<<<1, 256>>>(d_histogram, d_cumulativeHistogram);
    CUDA_CHECK(cudaGetLastError());
    CUDA_CHECK(cudaDeviceSynchronize());

    // Copy histogram back to host
    CUDA_CHECK(cudaMemcpy(histogram, d_histogram, 256 * sizeof(int), cudaMemcpyDeviceToHost));
    CUDA_CHECK(cudaMemcpy(cumulativeHistogram, d_cumulativeHistogram, 256 * sizeof(int), cudaMemcpyDeviceToHost));

    // Print histogram statistics
    printHistogramStatistics(histogram, numPixels);

    // Perform histogram equalization on device
    equalizeHistogramKernel<<<blocksPerGrid, threadsPerBlock>>>(d_img, width, height, channels, d_cumulativeHistogram, numPixels);
    CUDA_CHECK(cudaGetLastError());
    CUDA_CHECK(cudaDeviceSynchronize());

    // Copy processed image data back to host
    CUDA_CHECK(cudaMemcpy(img, d_img, numPixels * sizeof(unsigned char), cudaMemcpyDeviceToHost));

    // Get output filename from the user
    printf("Enter the output image filename (with .jpg or .jpeg extension): ");
    scanf("%255s", output_filename);

    // Save the equalized image
    if (!stbi_write_jpg(output_filename, width, height, channels, img, 90)) {
        printf("Error saving image %s\n", output_filename);
        CUDA_CHECK(cudaFree(d_img));
        CUDA_CHECK(cudaFree(d_histogram));
        CUDA_CHECK(cudaFree(d_cumulativeHistogram));
        stbi_image_free(img);
        return 1;
    }

    // Save the original histogram as an image
    saveHistogramImage(histogram, "original_histogram.jpg", 256, 200);

    // Save the equalized histogram as an image
    computeHistogramKernel<<<blocksPerGrid, threadsPerBlock>>>(d_img, width, height, channels, d_histogram);
    CUDA_CHECK(cudaGetLastError());
    CUDA_CHECK(cudaDeviceSynchronize());
    CUDA_CHECK(cudaMemcpy(histogram, d_histogram, 256 * sizeof(int), cudaMemcpyDeviceToHost));
    saveHistogramImage(histogram, "equalized_histogram.jpg", 256, 200);

    // Free device memory
    CUDA_CHECK(cudaFree(d_img));
    CUDA_CHECK(cudaFree(d_histogram));
    CUDA_CHECK(cudaFree(d_cumulativeHistogram));

    // Free host memory
    stbi_image_free(img);

    // Record end time and calculate duration in milliseconds
    clock_t end_time = clock();
    double elapsed_time_ms = ((double)(end_time - start_time) / CLOCKS_PER_SEC) * 1000.0;

    printf("Image processed and saved to %s\n", output_filename);
    printf("Execution time: %.2f milliseconds\n", elapsed_time_ms);

    return 0;
}
