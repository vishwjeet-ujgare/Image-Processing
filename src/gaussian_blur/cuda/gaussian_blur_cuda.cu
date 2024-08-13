#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>
#include <math.h>
#include <string.h>
#include <cuda_runtime.h>

#define INPUT_IMAGE "input_image.jpg"
#define OUTPUT_IMAGE "output_image.jpg"
#define KERNEL_WIDTH 21

// Function prototypes
void read_jpeg(const char *filename, unsigned char **image_data, int *width, int *height, int *channels);
void write_jpeg(const char *filename, unsigned char *image_data, int width, int height, int channels);
void generate_gaussian_filter(float **filter, int *filter_width, int blur_kernel_width);
void apply_gaussian_blur_cuda(unsigned char *image_data, unsigned char *output_image_data, int width, int height, int channels, float *filter, int filter_width);

__global__ void gaussian_blur_kernel(unsigned char *image_data, unsigned char *output_image_data, int width, int height, int channels, float *filter, int filter_width, int half_width) {
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (x < width && y < height) {
        float r = 0.0f, g = 0.0f, b = 0.0f;
        int filter_half = filter_width / 2;

        for (int fy = -filter_half; fy <= filter_half; fy++) {
            for (int fx = -filter_half; fx <= filter_half; fx++) {
                int iy = min(max(y + fy, 0), height - 1);
                int ix = min(max(x + fx, 0), width - 1);
                float weight = filter[(fy + filter_half) * filter_width + (fx + filter_half)];

                int index = (iy * width + ix) * channels;
                r += weight * image_data[index];
                g += weight * image_data[index + 1];
                b += weight * image_data[index + 2];
            }
        }

        int out_index = (y * width + x) * channels;
        output_image_data[out_index] = (unsigned char)fminf(fmaxf(r, 0.0f), 255.0f);
        output_image_data[out_index + 1] = (unsigned char)fminf(fmaxf(g, 0.0f), 255.0f);
        output_image_data[out_index + 2] = (unsigned char)fminf(fmaxf(b, 0.0f), 255.0f);
    }
}

void apply_gaussian_blur_cuda(unsigned char *image_data, unsigned char *output_image_data, int width, int height, int channels, float *filter, int filter_width) {
    unsigned char *d_image_data, *d_output_image_data;
    float *d_filter;

    size_t image_size = width * height * channels * sizeof(unsigned char);
    size_t filter_size = filter_width * filter_width * sizeof(float);

    cudaMalloc(&d_image_data, image_size);
    cudaMalloc(&d_output_image_data, image_size);
    cudaMalloc(&d_filter, filter_size);

    cudaMemcpy(d_image_data, image_data, image_size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_filter, filter, filter_size, cudaMemcpyHostToDevice);

    int block_size = 16;
    dim3 threads_per_block(block_size, block_size);
    dim3 num_blocks((width + block_size - 1) / block_size, (height + block_size - 1) / block_size);

    // CUDA timing
    cudaEvent_t start, stop;
    float elapsed_time;

    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    cudaEventRecord(start);
    
    gaussian_blur_kernel<<<num_blocks, threads_per_block>>>(d_image_data, d_output_image_data, width, height, channels, d_filter, filter_width, filter_width / 2);

    cudaEventRecord(stop);
    cudaEventSynchronize(stop);
    
    cudaEventElapsedTime(&elapsed_time, start, stop);

    printf("CUDA Gaussian blur execution time: %.2f ms\n", elapsed_time);

    cudaMemcpy(output_image_data, d_output_image_data, image_size, cudaMemcpyDeviceToHost);

    cudaFree(d_image_data);
    cudaFree(d_output_image_data);
    cudaFree(d_filter);

    cudaEventDestroy(start);
    cudaEventDestroy(stop);
}

int main(void) {
    unsigned char *image_data;
    unsigned char *output_image_data;
    int width, height, channels;
    float *filter;
    int filter_width;

    // Read the input JPEG image
    read_jpeg(INPUT_IMAGE, &image_data, &width, &height, &channels);

    // Generate Gaussian filter
    generate_gaussian_filter(&filter, &filter_width, KERNEL_WIDTH);

    // Allocate memory for the output image
    output_image_data = (unsigned char *)malloc(width * height * channels);
    if (!output_image_data) {
        fprintf(stderr, "Error allocating memory for output image.\n");
        free(image_data);
        free(filter);
        return 1;
    }

    // Apply Gaussian blur using CUDA
    apply_gaussian_blur_cuda(image_data, output_image_data, width, height, channels, filter, filter_width);

    // Write the output JPEG image
    write_jpeg(OUTPUT_IMAGE, output_image_data, width, height, channels);

    // Clean up
    free(image_data);
    free(output_image_data);
    free(filter);

    return 0;
}

// The read_jpeg and write_jpeg functions remain the same as in the CPU code

void read_jpeg(const char *filename, unsigned char **image_data, int *width, int *height, int *channels) {
    FILE *infile = fopen(filename, "rb");
    if (!infile) {
        fprintf(stderr, "Error opening input JPEG file %s\n", filename);
        exit(1);
    }

    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    *width = cinfo.output_width;
    *height = cinfo.output_height;
    *channels = cinfo.output_components;

    *image_data = (unsigned char *)malloc(*width * *height * *channels);
    if (!*image_data) {
        fprintf(stderr, "Error allocating memory for input image.\n");
        exit(1);
    }

    unsigned char *row_pointer = (unsigned char *)malloc(*width * *channels);
    while (cinfo.output_scanline < cinfo.output_height) {
        jpeg_read_scanlines(&cinfo, &row_pointer, 1);
        memcpy(*image_data + (*width * (*height - cinfo.output_scanline) * *channels), row_pointer, *width * *channels);
    }

    free(row_pointer);
    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
}

void write_jpeg(const char *filename, unsigned char *image_data, int width, int height, int channels) {
    FILE *outfile = fopen(filename, "wb");
    if (!outfile) {
        fprintf(stderr, "Error opening output JPEG file %s\n", filename);
        exit(1);
    }

    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, outfile);
    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = channels;
    cinfo.in_color_space = (channels == 1) ? JCS_GRAYSCALE : JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 90, TRUE);
    jpeg_start_compress(&cinfo, TRUE);

    unsigned char *row_pointer = (unsigned char *)malloc(width * channels);
    while (cinfo.next_scanline < cinfo.image_height) {
        memcpy(row_pointer, image_data + (width * (height - cinfo.next_scanline - 1) * channels), width * channels);
        jpeg_write_scanlines(&cinfo, &row_pointer, 1);
    }

    free(row_pointer);
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    fclose(outfile);
}

void generate_gaussian_filter(float **filter, int *filter_width, int blur_kernel_width) {
    *filter_width = blur_kernel_width;
    *filter = (float *)malloc(*filter_width * *filter_width * sizeof(float));
    if (!*filter) {
        fprintf(stderr, "Error allocating memory for filter.\n");
        exit(1);
    }

    float sigma = blur_kernel_width / 4.0f;
    float sum = 0.0f;

    int half_width = blur_kernel_width / 2;
    for (int y = -half_width; y <= half_width; y++) {
        for (int x = -half_width; x <= half_width; x++) {
            float value = expf(-(x * x + y * y) / (2 * sigma * sigma));
            (*filter)[(y + half_width) * blur_kernel_width + (x + half_width)] = value;
            sum += value;
        }
    }

    // Normalize the filter
    for (int i = 0; i < blur_kernel_width * blur_kernel_width; i++) {
        (*filter)[i] /= sum;
    }
}
