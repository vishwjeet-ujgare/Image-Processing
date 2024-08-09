#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "/home/user14/Vishwjeet_Ujgare_Feb_2024/vishwjeet_project/Image-Processing/external_lib/stb/stb_image_write.h"
#include "/home/user14/Vishwjeet_Ujgare_Feb_2024/vishwjeet_project/Image-Processing/external_lib/stb/stb_image.h"
#include <cuda_runtime.h>
#include <iostream>

__global__ void kernel(unsigned char* d_in, unsigned char* d_out, int width, int height, int channels) {
    int x = threadIdx.x + blockIdx.x * blockDim.x; // cols
    int y = threadIdx.y + blockIdx.y * blockDim.y; // rows

    if (y < height && x < width) {
        int greyOffset = y * width + x;
        int bgrOffset = greyOffset * channels;
        unsigned char b = d_in[bgrOffset];
        unsigned char g = d_in[bgrOffset + 1];
        unsigned char r = d_in[bgrOffset + 2];

        d_out[greyOffset] = 0.21f * r + 0.71f * g + 0.07f * b;
    }
}

int main() {
    // const char* path = "D:\\Cuda_prac\\Image\\1280x720.jpg"; // hardcoding the path

  const char* path = "/home/user14/Vishwjeet_Ujgare_Feb_2024/vishwjeet_project/Image-Processing/data/input/4k-3840-x-2160-wallpapers-themefoxx.jpg";
    // Load the image using stb_image
    int width, height, channels;
    unsigned char *h_image = stbi_load(path, &width, &height, &channels, 0);
    if (!h_image) {
        std::cerr << "Failed to find or read " << path << std::endl;
        return -1;
    }

    // Allocate memory on the GPU
    unsigned char *d_in, *d_out;
    cudaMalloc((void**)&d_in, width * height * channels * sizeof(unsigned char));
    cudaMalloc((void**)&d_out, width * height * sizeof(unsigned char));

    // Copy image data to the GPU
    cudaMemcpy(d_in, h_image, width * height * channels * sizeof(unsigned char), cudaMemcpyHostToDevice);

    // Setting block and grid dimensions
    dim3 dimBlock(16, 16);
    dim3 dimGrid((width + dimBlock.x - 1) / dimBlock.x, (height + dimBlock.y - 1) / dimBlock.y);

    // Launching the kernel
    kernel<<<dimGrid, dimBlock>>>(d_in, d_out, width, height, channels);
    cudaDeviceSynchronize();

    // Allocate host memory for the output image
    unsigned char *h_image_processed = (unsigned char*)malloc(width * height * sizeof(unsigned char));

    // Copy the processed image data back to the host
    cudaMemcpy(h_image_processed, d_out, width * height * sizeof(unsigned char), cudaMemcpyDeviceToHost);

    // Save the processed image using stb_image_write
    stbi_write_jpg("/home/user14/Vishwjeet_Ujgare_Feb_2024/vishwjeet_project/Image-Processing/data/output/cuda_processed_img/Processed_Image.jpg", width, height, 1, h_image_processed, 100);

    // Free the allocated memory
    stbi_image_free(h_image);
    free(h_image_processed);
    cudaFree(d_in);
    cudaFree(d_out);

    std::cout << "Image processed and saved as Processed_Image.jpg" << std::endl;

    return 0;
}
