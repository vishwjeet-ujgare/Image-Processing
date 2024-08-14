#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>
#include <math.h>
#include <string.h>  // Include this for memcpy
#include <time.h>    // Include this for clock()


#define INPUT_IMAGE "/home/hpcap/Desktop/image_process/Image-Processing/src/gaussian_blur/cuda/input_image.jpg"
#define OUTPUT_IMAGE "/home/hpcap/Desktop/image_process/Image-Processing/src/gaussian_blur/openmp/output_image.jpg"
#define KERNEL_WIDTH 21

// Function prototypes
void read_jpeg(const char *filename, unsigned char **image_data, int *width, int *height, int *channels);
void write_jpeg(const char *filename, unsigned char *image_data, int width, int height, int channels);
void generate_gaussian_filter(float **filter, int *filter_width, int blur_kernel_width);
void apply_gaussian_blur(unsigned char *image_data, unsigned char *output_image_data, int width, int height, int channels, float *filter, int filter_width);

int main(void) {
    unsigned char *image_data;
    unsigned char *output_image_data;
    int width, height, channels;
    float *filter;
    int filter_width;
    clock_t start, end;
    double cpu_time_used;

    // Start overall timing
    start = clock();

    // Timing image reading
    read_jpeg(INPUT_IMAGE, &image_data, &width, &height, &channels);

    // Timing filter generation
    generate_gaussian_filter(&filter, &filter_width, KERNEL_WIDTH);

    // Allocate memory for the output image
    output_image_data = (unsigned char *)malloc(width * height * channels);
    if (!output_image_data) {
        fprintf(stderr, "Error allocating memory for output image.\n");
        free(image_data);
        free(filter);
        return 1;
    }

    // Timing Gaussian blur application
    apply_gaussian_blur(image_data, output_image_data, width, height, channels, filter, filter_width);

    // Timing image writing
    write_jpeg(OUTPUT_IMAGE, output_image_data, width, height, channels);

    // End overall timing
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC * 1000; // Convert to milliseconds
    printf("Total execution time: %.3f ms\n", cpu_time_used);

    // Clean up
    free(image_data);
    free(output_image_data);
    free(filter);

    return 0;
}

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

void apply_gaussian_blur(unsigned char *image_data, unsigned char *output_image_data, int width, int height, int channels, float *filter, int filter_width) {
    int half_width = filter_width / 2;
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            float r = 0, g = 0, b = 0;

            for (int fy = -half_width; fy <= half_width; fy++) {
                for (int fx = -half_width; fx <= half_width; fx++) {
                    int iy = y + fy;
                    int ix = x + fx;
                    if (ix >= 0 && ix < width && iy >= 0 && iy < height) {
                        float weight = filter[(fy + half_width) * filter_width + (fx + half_width)];
                        unsigned char *pixel = image_data + (iy * width + ix) * channels;
                        r += weight * pixel[0];
                        g += weight * pixel[1];
                        b += weight * pixel[2];
                    }
                }
            }

            unsigned char *out_pixel = output_image_data + (y * width + x) * channels;
            out_pixel[0] = (unsigned char)fminf(fmaxf(r, 0), 255);
            out_pixel[1] = (unsigned char)fminf(fmaxf(g, 0), 255);
            out_pixel[2] = (unsigned char)fminf(fmaxf(b, 0), 255);
        }
    }
}
