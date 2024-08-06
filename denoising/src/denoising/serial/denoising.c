#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <jpeglib.h>
#include <string.h>
#include <sys/stat.h>

// Function to read a JPEG image
void readJPEG(const char *filename, unsigned char **image, int *width, int *height)
{
    FILE *infile = fopen(filename, "rb");
    if (infile == NULL)
    {
        fprintf(stderr, "Error opening JPEG file %s\n", filename);
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
    int row_stride = cinfo.output_width * cinfo.output_components;

    *image = (unsigned char *)malloc(cinfo.output_height * row_stride);
    if (*image == NULL)
    {
        fprintf(stderr, "Error allocating memory for JPEG image\n");
        exit(1);
    }

    while (cinfo.output_scanline < cinfo.output_height)
    {
        unsigned char *buffer_array[1];
        buffer_array[0] = *image + (cinfo.output_scanline) * row_stride;
        jpeg_read_scanlines(&cinfo, buffer_array, 1);
    }

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);
    fclose(infile);
}

// Function to save a JPEG image
void saveJPEG(const char *filename, unsigned char *image, int width, int height)
{
    FILE *outfile = fopen(filename, "wb");
    if (outfile == NULL)
    {
        fprintf(stderr, "Error opening JPEG file %s\n", filename);
        exit(1);
    }

    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, outfile);

    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 3; // RGB
    cinfo.in_color_space = JCS_RGB;

    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, 75, TRUE); // Quality level

    jpeg_start_compress(&cinfo, TRUE);

    int row_stride = width * 3; // RGB
    while (cinfo.next_scanline < cinfo.image_height)
    {
        unsigned char *buffer_array[1];
        buffer_array[0] = image + (cinfo.next_scanline) * row_stride;
        jpeg_write_scanlines(&cinfo, buffer_array, 1);
    }

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    fclose(outfile);
}

// Function to calculate weights for Non-Local Means
double calculateWeight(unsigned char *patch1, unsigned char *patch2, int patch_size)
{
    double weight = 0.0;
    for (int i = 0; i < patch_size; i++)
    {
        weight += pow((double)patch1[i] - (double)patch2[i], 2);
    }
    return exp(-weight / (2.0 * pow(0.2, 2))); // Adjust sigma to 0.2 or other values
}

// Non-Local Means Denoising Function
void denoiseImage(unsigned char *input, unsigned char *output, int width, int height)
{
    int patch_size = 9; // Patch size (3x3 for simplicity)
    int radius = patch_size / 2;

    // Loop over each pixel
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            double sum_weight[3] = {0.0, 0.0, 0.0}; // To store weights for each color channel
            double sum_value[3] = {0.0, 0.0, 0.0};  // To store weighted sum for each color channel

            // Loop over neighboring patches
            for (int dy = -radius; dy <= radius; dy++)
            {
                for (int dx = -radius; dx <= radius; dx++)
                {
                    int nx = x + dx;
                    int ny = y + dy;

                    if (nx >= 0 && ny >= 0 && nx < width && ny < height)
                    {
                        int pixel_index = (ny * width + nx) * 3; // RGB
                        int center_index = (y * width + x) * 3;
                        double weight = calculateWeight(&input[pixel_index], &input[center_index], patch_size * 3);

                        for (int c = 0; c < 3; c++)
                        { // Process each color channel
                            sum_weight[c] += weight;
                            sum_value[c] += weight * input[pixel_index + c];
                        }
                    }
                }
            }

            // Compute output value for each channel
            int output_index = (y * width + x) * 3;
            for (int c = 0; c < 3; c++)
            {
                if (sum_weight[c] > 0)
                {
                    output[output_index + c] = (unsigned char)(sum_value[c] / sum_weight[c]);
                }
                else
                {
                    output[output_index + c] = input[output_index + c]; // No denoising applied
                }
            }
        }
    }
}

// Main Function
int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s <input.jpeg> <output_folder> <output_filename>\n", argv[0]);
        return 1;
    }

    unsigned char *image, *outputImage;
    int width, height;

    readJPEG(argv[1], &image, &width, &height);
    outputImage = (unsigned char *)malloc(width * height * 3); // RGB

    if (outputImage == NULL)
    {
        perror("Failed to allocate memory for output image");
        free(image);
        return 1;
    }

    denoiseImage(image, outputImage, width, height);

    // Create the output folder if it doesn't exist
    char output_folder[256];
    strcpy(output_folder, argv[2]);
    mkdir(output_folder, 0777);

    // Construct the output file path
    char output_filename[256];
    strcpy(output_filename, output_folder);
    strcat(output_filename, "/");
    strcat(output_filename, argv[3]);

    saveJPEG(output_filename, outputImage, width, height);

    free(image);
    free(outputImage);

    return 0;
}