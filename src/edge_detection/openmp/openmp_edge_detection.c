#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <jpeglib.h>
#include <setjmp.h>
#include <time.h>
#include <string.h>
#include <omp.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef struct {
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
} jpeg_decompress_info;

typedef struct {
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
} jpeg_compress_info;

void handle_error(j_common_ptr cinfo) {
    (*(cinfo->err->output_message))(cinfo);
    exit(1);
}

void sobelEdgeDetection(unsigned char* input, unsigned char* output, int width, int height, int channels) {
    int kernel_x[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int kernel_y[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

    #pragma omp parallel for
    for (int y = 1; y < height - 1; y++) {
        for (int x = 1; x < width - 1; x++) {
            int sum_x = 0;
            int sum_y = 0;

            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    int pixel = (input[((y + i) * width + (x + j)) * channels] * 0.299 +
                                 input[((y + i) * width + (x + j)) * channels + 1] * 0.587 +
                                 input[((y + i) * width + (x + j)) * channels + 2] * 0.114);
                    sum_x += pixel * kernel_x[i + 1][j + 1];
                    sum_y += pixel * kernel_y[i + 1][j + 1];
                }
            }

            int gradient = (int)sqrt((double)(sum_x * sum_x + sum_y * sum_y));
            int threshold = 128; 
            output[(y * width) + x] = (gradient > threshold) ? 255 : 0;
        }
    }
}

void read_jpeg_file(const char* filename, unsigned char** image_buffer, int* width, int* height, int* channels) {
    jpeg_decompress_info dinfo;
    dinfo.cinfo.err = jpeg_std_error(&dinfo.jerr);
    dinfo.jerr.error_exit = handle_error;

    jpeg_create_decompress(&dinfo.cinfo);

    FILE* infile = fopen(filename, "rb");
    if (infile == NULL) {
        fprintf(stderr, "Can't open %s\n", filename);
        exit(1);
    }

    jpeg_stdio_src(&dinfo.cinfo, infile);
    jpeg_read_header(&dinfo.cinfo, TRUE);
    jpeg_start_decompress(&dinfo.cinfo);

    *width = dinfo.cinfo.output_width;
    *height = dinfo.cinfo.output_height;
    *channels = dinfo.cinfo.output_components;

    printf("Width: %d, Height: %d, Channels: %d\n", *width, *height, *channels);

    *image_buffer = (unsigned char*)malloc(*width * *height * *channels);
    if (*image_buffer == NULL) {
        fprintf(stderr, "Memory allocation failed for image buffer\n");
        exit(1);
    }

    unsigned char* row_pointer = (unsigned char*)malloc(*width * *channels);
    if (row_pointer == NULL) {
        fprintf(stderr, "Memory allocation failed for row pointer\n");
        exit(1);
    }

    while (dinfo.cinfo.output_scanline < dinfo.cinfo.output_height) {
        jpeg_read_scanlines(&dinfo.cinfo, &row_pointer, 1);
        memcpy(*image_buffer + (*height - dinfo.cinfo.output_scanline) * *width * *channels, row_pointer, *width * *channels);
    }

    free(row_pointer);
    jpeg_finish_decompress(&dinfo.cinfo);
    jpeg_destroy_decompress(&dinfo.cinfo);
    fclose(infile);
}

void write_jpeg_file(const char* filename, unsigned char* image_buffer, int width, int height, int channels, int quality) {
    jpeg_compress_info cinfo;
    cinfo.cinfo.err = jpeg_std_error(&cinfo.jerr);
    cinfo.jerr.error_exit = handle_error;

    jpeg_create_compress(&cinfo.cinfo);

    FILE* outfile = fopen(filename, "wb");
    if (outfile == NULL) {
        fprintf(stderr, "Can't open %s\n", filename);
        exit(1);
    }

    jpeg_stdio_dest(&cinfo.cinfo, outfile);

    cinfo.cinfo.image_width = width;
    cinfo.cinfo.image_height = height;
    cinfo.cinfo.input_components = 1; 
    cinfo.cinfo.in_color_space = JCS_GRAYSCALE;

    jpeg_set_defaults(&cinfo.cinfo);
    jpeg_set_quality(&cinfo.cinfo, quality, TRUE);
    jpeg_start_compress(&cinfo.cinfo, TRUE);

    unsigned char* row_pointer = (unsigned char*)malloc(width);
    if (row_pointer == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        jpeg_finish_compress(&cinfo.cinfo);
        jpeg_destroy_compress(&cinfo.cinfo);
        fclose(outfile);
        exit(1);
    }

    while (cinfo.cinfo.next_scanline < cinfo.cinfo.image_height) {
        memcpy(row_pointer, image_buffer + (cinfo.cinfo.image_height - cinfo.cinfo.next_scanline - 1) * width, width);
        jpeg_write_scanlines(&cinfo.cinfo, &row_pointer, 1);
    }

    free(row_pointer);
    jpeg_finish_compress(&cinfo.cinfo);
    jpeg_destroy_compress(&cinfo.cinfo);
    fclose(outfile);
}

int main() {
    char filename[1000];
    printf("Enter the filename of the input image: ");
    scanf("%s", filename);

    printf("\nRunning...");

    int width, height, channels;
    unsigned char* input_image = NULL;
    read_jpeg_file(filename, &input_image, &width, &height, &channels);

    if (input_image == NULL) {
        fprintf(stderr, "Error reading input image.\n");
        return 1;
    }

    unsigned char* edge_image = (unsigned char*)malloc(width * height);
    if (edge_image == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        free(input_image);
        return 1;
    }

    // Measure execution time in milliseconds
    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start); // Start measuring execution time
    sobelEdgeDetection(input_image, edge_image, width, height, channels);
    clock_gettime(CLOCK_MONOTONIC, &end); // Stop measuring execution time

    double execution_time = (end.tv_sec - start.tv_sec) * 1000.0; // seconds to milliseconds
    execution_time += (end.tv_nsec - start.tv_nsec) / 1000000.0; // nanoseconds to milliseconds
    printf("Execution time: %f milliseconds\n", execution_time);

    free(input_image);

    write_jpeg_file("/home/hpcap/Desktop/image_process/Image-Processing/data/output/edge_detection/openmp_output.jpg", edge_image, width, height, 1, 100);
    free(edge_image);

    return 0;
}
