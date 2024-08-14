#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <cuda_runtime.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// CUDA kernel to normalize image intensity
__global__ void normalize_image_kernel(unsigned char* img, int num_pixels, unsigned char min_val, unsigned char max_val) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < num_pixels) {
        float scale = (max_val != min_val) ? 255.0f / (max_val - min_val) : 1.0f;
        img[idx] = (unsigned char)((img[idx] - min_val) * scale);
    }
}

// CUDA kernel to apply contrast stretching
__global__ void apply_contrast_stretching_kernel(unsigned char* img, int num_pixels, unsigned char low_bound, unsigned char high_bound) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    if (idx < num_pixels) {
        if (img[idx] < low_bound) img[idx] = 0;
        else if (img[idx] > high_bound) img[idx] = 255;
        else {
            img[idx] = (unsigned char)(((float)(img[idx] - low_bound) / (high_bound - low_bound)) * 255.0f);
        }
    }
}

// Function to find min and max values on host
void find_min_max(unsigned char* img, int num_pixels, unsigned char* min_val, unsigned char* max_val) {
    *min_val = 255;
    *max_val = 0;
    for (int i = 0; i < num_pixels; i++) {
        if (img[i] < *min_val) *min_val = img[i];
        if (img[i] > *max_val) *max_val = img[i];
    }
}

int main() {
    char input_filename[256], output_filename[256];
    int width, height, channels;

    printf("Enter the input image filename (with .jpg, .png, etc.): ");
    scanf("%255s", input_filename);

    clock_t start_time = clock();

    // Load image
    unsigned char* img = stbi_load(input_filename, &width, &height, &channels, 0);
    if (img == NULL) {
        fprintf(stderr, "Error loading image %s: %s\n", input_filename, stbi_failure_reason());
        return 1;
    }

    int num_pixels = width * height * channels;
    unsigned char *d_img, min_val, max_val;

    // Find min and max values on host
    find_min_max(img, num_pixels, &min_val, &max_val);

    // Allocate memory on GPU
    cudaError_t err = cudaMalloc(&d_img, num_pixels * sizeof(unsigned char));
    if (err != cudaSuccess) {
        fprintf(stderr, "CUDA Error: %s\n", cudaGetErrorString(err));
        stbi_image_free(img);
        return 1;
    }

    err = cudaMemcpy(d_img, img, num_pixels * sizeof(unsigned char), cudaMemcpyHostToDevice);
    if (err != cudaSuccess) {
        fprintf(stderr, "CUDA Error: %s\n", cudaGetErrorString(err));
        cudaFree(d_img);
        stbi_image_free(img);
        return 1;
    }

    // Launch normalization kernel
    int blockSize = 256;
    int numBlocks = (num_pixels + blockSize - 1) / blockSize;
    normalize_image_kernel<<<numBlocks, blockSize>>>(d_img, num_pixels, min_val, max_val);
    err = cudaGetLastError();
    if (err != cudaSuccess) {
        fprintf(stderr, "CUDA Error: %s\n", cudaGetErrorString(err));
        cudaFree(d_img);
        stbi_image_free(img);
        return 1;
    }

    cudaDeviceSynchronize();

    // Apply contrast stretching
    unsigned char low_bound = (unsigned char)(min_val + 0.1f * (max_val - min_val));
    unsigned char high_bound = (unsigned char)(max_val - 0.1f * (max_val - min_val));
    apply_contrast_stretching_kernel<<<numBlocks, blockSize>>>(d_img, num_pixels, low_bound, high_bound);
    err = cudaGetLastError();
    if (err != cudaSuccess) {
        fprintf(stderr, "CUDA Error: %s\n", cudaGetErrorString(err));
        cudaFree(d_img);
        stbi_image_free(img);
        return 1;
    }

    cudaDeviceSynchronize();

    // Copy the processed image back to host
    err = cudaMemcpy(img, d_img, num_pixels * sizeof(unsigned char), cudaMemcpyDeviceToHost);
    if (err != cudaSuccess) {
        fprintf(stderr, "CUDA Error: %s\n", cudaGetErrorString(err));
        cudaFree(d_img);
        stbi_image_free(img);
        return 1;
    }

    // Save the processed image
    printf("Enter the output image filename (with .jpg, .png, etc.): ");
    scanf("%255s", output_filename);
    if (!stbi_write_jpg(output_filename, width, height, channels, img, 90)) {
        fprintf(stderr, "Error saving image %s\n", output_filename);
        cudaFree(d_img);
        stbi_image_free(img);
        return 1;
    }

    // Clean up
    cudaFree(d_img);
    stbi_image_free(img);

    clock_t end_time = clock();
    double duration = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    printf("Image normalized, contrast enhanced, and saved to %s\n", output_filename);
    printf("Time taken for processing: %.2f seconds\n", duration);

    return 0;
}