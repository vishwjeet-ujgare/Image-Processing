#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>
#include <math.h>
#include <float.h>
#include <string.h>

#define PATCH_SIZE 7
#define SEARCH_RADIUS 21
#define H 0.1

void read_image(const char *filename, unsigned char **image, int *width, int *height) {
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE *infile;

    if ((infile = fopen(filename, "rb")) == NULL) {
        fprintf(stderr, "Can't open %s\n", filename);
        exit(1);
    }

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);
    jpeg_stdio_src(&cinfo, infile);
    jpeg_read_header(&cinfo, TRUE);
    jpeg_start_decompress(&cinfo);

    *width = cinfo.output_width;
    *height = cinfo.output_height;

    *image = (unsigned char *)malloc(cinfo.output_width * cinfo.output_height * cinfo.output_components);

    JSAMPARRAY buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, cinfo.output_width * cinfo.output_components, 1);
    while (cinfo.output_scanline < cinfo.output_height) {
        jpeg_read_scanlines(&cinfo, buffer, 1);
        memcpy(*image + (cinfo.output_scanline - 1) * cinfo.output_width * cinfo.output_components, buffer[0], cinfo.output_width * cinfo.output_components);
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
}

void write_image(const char *filename, unsigned char *image, int width, int height) {
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    FILE *outfile;

    if ((outfile = fopen(filename, "wb")) == NULL) {
        fprintf(stderr, "Can't open %s\n", filename);
        exit(1);
    }

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 1; // Grayscale
    cinfo.in_color_space = JCS_GRAYSCALE;

    jpeg_set_defaults(&cinfo);
    jpeg_start_compress(&cinfo, TRUE);

    JSAMPROW row_pointer[1];
    row_pointer[0] = (JSAMPROW)malloc(width);

    while (cinfo.next_scanline < cinfo.image_height) {
        memcpy(row_pointer[0], image + cinfo.next_scanline * width, width);
        jpeg_write_scanlines(&cinfo, row_pointer, 1);
    }

    free(row_pointer[0]);
    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    fclose(outfile);
}

void compute_patch_distance(const unsigned char *image, double *distances, int width, int height, int x, int y, int patch_size) {
    int half_patch = patch_size / 2;
    double sum;
    for (int i = -half_patch; i <= half_patch; i++) {
        for (int j = -half_patch; j <= half_patch; j++) {
            int px = x + i;
            int py = y + j;
            if (px >= 0 && px < width && py >= 0 && py < height) {
                int p_index = (y + j) * width + (x + i);
                int q_index = (y) * width + (x);
                distances[(i + half_patch) * patch_size + (j + half_patch)] = (double)abs(image[p_index] - image[q_index]);
            } else {
                distances[(i + half_patch) * patch_size + (j + half_patch)] = DBL_MAX;
            }
        }
    }
}

// void denoise_image(const unsigned char *input_image, unsigned char *output_image, int width, int height, int patch_size, int search_radius, double h) {
//     int half_patch = patch_size / 2;
//     int half_radius = search_radius / 2;
//     double *distances = (double *)malloc(patch_size * patch_size * sizeof(double));

//     for (int y = half_patch; y < height - half_patch; y++) {
//         for (int x = half_patch; x < width - half_patch; x++) {
//             double weight_sum = 0.0;
//             double pixel_sum = 0.0;
//             for (int dy = -half_radius; dy <= half_radius; dy++) {
//                 for (int dx = -half_radius; dx <= half_radius; dx++) {
//                     int nx = x + dx;
//                     int ny = y + dy;
//                     if (nx >= 0 && nx < width && ny >= 0 && ny < height) {
//                         double distance;
//                         compute_patch_distance(input_image, distances, width, height, x, y, patch_size);
//                         double weight = 0.0;
//                         for (int i = -half_patch; i <= half_patch; i++) {
//                             for (int j = -half_patch; j <= half_patch; j++) {
//                                 weight += exp(-distances[(i + half_patch) * patch_size + (j + half_patch)] / (h * h));
//                             }
//                         }
//                         weight = weight / (h * h);
//                         pixel_sum += weight * input_image[ny * width + nx];
//                         weight_sum += weight;
//                     }
//                 }
//             }
//             output_image[y * width + x] = (unsigned char)(pixel_sum / weight_sum);
//         }
//     }

//     free(distances);
// }


void denoise_image(const unsigned char *input_image, unsigned char *output_image, int width, int height, int patch_size, int search_radius, double h) {
    int half_patch = patch_size / 2;
    int half_radius = search_radius / 2;
    double *distances = (double *)malloc(patch_size * patch_size * sizeof(double));

    for (int y = half_patch; y < height - half_patch; y++) {
        for (int x = half_patch; x < width - half_patch; x++) {
            double weight_sum = 0.0;
            double pixel_sum = 0.0;
            for (int dy = -half_radius; dy <= half_radius; dy++) {
                for (int dx = -half_radius; dx <= half_radius; dx++) {
                    int nx = x + dx;
                    int ny = y + dy;
                    if (nx >= half_patch && nx < width - half_patch && ny >= half_patch && ny < height - half_patch) {
                        double distance = 0.0;
                        compute_patch_distance(input_image, distances, width, height, x, y, patch_size);
                        double weight = 0.0;
                        for (int i = -half_patch; i <= half_patch; i++) {
                            for (int j = -half_patch; j <= half_patch; j++) {
                                int p_index = ((y + i) * width + (x + j)) * 1;  // Assuming grayscale (1 channel)
                                int q_index = ((ny + i) * width + (nx + j)) * 1;
                                distance += pow(input_image[p_index] - input_image[q_index], 2);
                            }
                        }
                        weight = exp(-distance / (h * h));
                        pixel_sum += weight * input_image[ny * width + nx];
                        weight_sum += weight;
                    }
                }
            }
            output_image[y * width + x] = (unsigned char)(pixel_sum / weight_sum);
        }
    }

    free(distances);
}

int main() {
    int width, height;
    unsigned char *input_image, *output_image;

    read_image("/home/hpcap/Desktop/image_process/Image-Processing/data/input/100080unimgNoise70.jpg", &input_image, &width, &height);

    output_image = (unsigned char *)malloc(width * height * sizeof(unsigned char));

    denoise_image(input_image, output_image, width, height, PATCH_SIZE, SEARCH_RADIUS, H);

    write_image("/home/hpcap/Desktop/image_process/Image-Processing/data/output/100080unimgNoise70.jpg", output_image, width, height);

    free(input_image);
    free(output_image);

    return 0;
}
