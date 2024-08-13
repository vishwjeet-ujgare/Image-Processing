#include <cassert> // Include for assert
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STBI_MALLOC(size) malloc(size)
#define STBI_REALLOC(ptr, size) realloc(ptr, size)
#define STBI_FREE(ptr) free(ptr)
#define STBI_ASSERT(x) assert(x)

#include "stb_image.h"
#include "stb_image_write.h"
#include <cuda_runtime.h>
#include <iostream>
#include <cmath>
#include <chrono>

__global__ void sobelKernel(unsigned char* d_in, unsigned char* d_out, int width, int height) {
    int x = threadIdx.x + blockIdx.x * blockDim.x;
    int y = threadIdx.y + blockIdx.y * blockDim.y;

    if (x >= 1 && x < width - 1 && y >= 1 && y < height - 1) {
        int sobelX[3][3] = {
            {-1,  0,  1},
            {-2,  0,  2},
            {-1,  0,  1}
        };
        int sobelY[3][3] = {
            {-1, -2, -1},
            { 0,  0,  0},
            { 1,  2,  1}
        };

        float gradX = 0.0f;
        float gradY = 0.0f;

        for (int ky = -1; ky <= 1; ky++) {
            for (int kx = -1; kx <= 1; kx++) {
                int ix = x + kx;
                int iy = y + ky;
                unsigned char pixel = d_in[iy * width + ix];
                gradX += pixel * sobelX[ky + 1][kx + 1];
                gradY += pixel * sobelY[ky + 1][kx + 1];
            }
        }

        float magnitude = sqrtf(gradX * gradX + gradY * gradY);
        d_out[y * width + x] = static_cast<unsigned char>(min(255.0f, magnitude));
    }
}

int main() {
    // Take input image path from user
    std::string path;
    std::cout << "Enter the path to the input image: ";
    std::getline(std::cin, path);

    int width, height, channels;
    unsigned char *h_image = stbi_load(path.c_str(), &width, &height, &channels, 0);
    if (!h_image) {
        std::cerr << "Failed to find or read " << path << std::endl;
        return -1;
    }

    // Convert to grayscale
    unsigned char *h_gray_image = (unsigned char*)malloc(width * height * sizeof(unsigned char));
    if (!h_gray_image) {
        std::cerr << "Failed to allocate memory for grayscale image" << std::endl;
        stbi_image_free(h_image);
        return -1;
    }

    for (int i = 0; i < width * height; ++i) {
        int r = h_image[i * channels + 0];
        int g = h_image[i * channels + 1];
        int b = h_image[i * channels + 2];
        h_gray_image[i] = static_cast<unsigned char>(0.21f * r + 0.71f * g + 0.07f * b);
    }

    unsigned char *d_in, *d_out;
    cudaError_t err = cudaMalloc((void**)&d_in, width * height * sizeof(unsigned char));
    if (err != cudaSuccess) {
        std::cerr << "CUDA malloc failed for d_in: " << cudaGetErrorString(err) << std::endl;
        free(h_gray_image);
        stbi_image_free(h_image);
        return -1;
    }

    err = cudaMalloc((void**)&d_out, width * height * sizeof(unsigned char));
    if (err != cudaSuccess) {
        std::cerr << "CUDA malloc failed for d_out: " << cudaGetErrorString(err) << std::endl;
        cudaFree(d_in);
        free(h_gray_image);
        stbi_image_free(h_image);
        return -1;
    }

    err = cudaMemcpy(d_in, h_gray_image, width * height * sizeof(unsigned char), cudaMemcpyHostToDevice);
    if (err != cudaSuccess) {
        std::cerr << "CUDA memcpy failed: " << cudaGetErrorString(err) << std::endl;
        cudaFree(d_in);
        cudaFree(d_out);
        free(h_gray_image);
        stbi_image_free(h_image);
        return -1;
    }

    dim3 dimBlock(16, 16);
    dim3 dimGrid((width + dimBlock.x - 1) / dimBlock.x, (height + dimBlock.y - 1) / dimBlock.y);

    // Measure execution time
    auto start = std::chrono::high_resolution_clock::now();

    sobelKernel<<<dimGrid, dimBlock>>>(d_in, d_out, width, height);
    err = cudaDeviceSynchronize();
    if (err != cudaSuccess) {
        std::cerr << "CUDA kernel execution failed: " << cudaGetErrorString(err) << std::endl;
        cudaFree(d_in);
        cudaFree(d_out);
        free(h_gray_image);
        stbi_image_free(h_image);
        return -1;
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> duration = end - start;
    std::cout << "Execution time: " << duration.count() << " milliseconds" << std::endl;

    unsigned char *h_image_processed = (unsigned char*)malloc(width * height * sizeof(unsigned char));
    if (!h_image_processed) {
        std::cerr << "Failed to allocate memory for processed image" << std::endl;
        cudaFree(d_in);
        cudaFree(d_out);
        free(h_gray_image);
        stbi_image_free(h_image);
        return -1;
    }

    err = cudaMemcpy(h_image_processed, d_out, width * height * sizeof(unsigned char), cudaMemcpyDeviceToHost);
    if (err != cudaSuccess) {
        std::cerr << "CUDA memcpy failed: " << cudaGetErrorString(err) << std::endl;
        cudaFree(d_in);
        cudaFree(d_out);
        free(h_gray_image);
        free(h_image_processed);
        stbi_image_free(h_image);
        return -1;
    }

    std::string outputPath;
    std::cout << "Enter the path to save the output image: ";
    std::getline(std::cin, outputPath);

    stbi_write_jpg(outputPath.c_str(), width, height, 1, h_image_processed, 100);

    stbi_image_free(h_image);
    free(h_gray_image);
    free(h_image_processed);
    cudaFree(d_in);
    cudaFree(d_out);

    std::cout << "Image processed and saved as " << outputPath << std::endl;

    return 0;
}
