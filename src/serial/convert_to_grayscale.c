#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>
#include <string.h>

void convertToGrayscale(const char *inputFilePath, const char *outputFilePath) {
    struct jpeg_decompress_struct cinfo;
    struct jpeg_compress_struct cinfo_out;
    struct jpeg_error_mgr jerr;

    FILE *inputFile, *outputFile;
    JSAMPARRAY buffer;
    int row_stride;

    // Open the input file
    if ((inputFile = fopen(inputFilePath, "rb")) == NULL) {
        fprintf(stderr, "Error opening input file: %s\n", inputFilePath);
        perror("fopen"); // Detailed error message
        exit(EXIT_FAILURE);
    }

    // Set up error handling
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_decompress(&cinfo);

    // Specify the data source (input file)
    jpeg_stdio_src(&cinfo, inputFile);

    // Read the JPEG header
    jpeg_read_header(&cinfo, TRUE);

    // Start decompression
    jpeg_start_decompress(&cinfo);

    row_stride = cinfo.output_width * cinfo.output_components;

    // Allocate memory for the buffer
    buffer = (*cinfo.mem->alloc_sarray)((j_common_ptr)&cinfo, JPOOL_IMAGE, row_stride, 1);

    // Prepare to write the grayscale image
    cinfo_out.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo_out);

    // Open the output file
    if ((outputFile = fopen(outputFilePath, "wb")) == NULL) {
        fprintf(stderr, "Error opening output file: %s\n", outputFilePath);
        perror("fopen"); // Detailed error message
        exit(EXIT_FAILURE);
    }

    jpeg_stdio_dest(&cinfo_out, outputFile);

    cinfo_out.image_width = cinfo.output_width;
    cinfo_out.image_height = cinfo.output_height;
    cinfo_out.input_components = 1;  // Grayscale output
    cinfo_out.in_color_space = JCS_GRAYSCALE;

    jpeg_set_defaults(&cinfo_out);
    jpeg_start_compress(&cinfo_out, TRUE);

    // Process each scanline
    while (cinfo.output_scanline < cinfo.output_height) {
        jpeg_read_scanlines(&cinfo, buffer, 1);

        // Convert each pixel to grayscale
        for (int i = 0; i < row_stride; i += 3) {
            unsigned char gray = (unsigned char)(0.299 * buffer[0][i] +
                                                 0.587 * buffer[0][i + 1] +
                                                 0.114 * buffer[0][i + 2]);
            buffer[0][i / 3] = gray;
        }

        jpeg_write_scanlines(&cinfo_out, buffer, 1);
    }

    // Finish compression and clean up
    jpeg_finish_compress(&cinfo_out);
    jpeg_destroy_compress(&cinfo_out);

    jpeg_finish_decompress(&cinfo);
    jpeg_destroy_decompress(&cinfo);

    fclose(inputFile);
    fclose(outputFile);
}

int main() {
    char inputFilePath[256], outputFilePath[256];

    // Ask for the input image path
    printf("Enter the full path for the input image (e.g., /home/user/image.jpg): ");
    if (fgets(inputFilePath, sizeof(inputFilePath), stdin) == NULL) {
        fprintf(stderr, "Error reading input path.\n");
        return EXIT_FAILURE;
    }
    // Remove newline character from fgets
    inputFilePath[strcspn(inputFilePath, "\n")] = '\0';

    // Ask for the output image path
    printf("Enter the full path for the output image (e.g., /home/user/output.jpg): ");
    if (fgets(outputFilePath, sizeof(outputFilePath), stdin) == NULL) {
        fprintf(stderr, "Error reading output path.\n");
        return EXIT_FAILURE;
    }
    // Remove newline character from fgets
    outputFilePath[strcspn(outputFilePath, "\n")] = '\0';

    // Convert the image to grayscale
    convertToGrayscale(inputFilePath, outputFilePath);

    printf("Grayscale image saved successfully to %s.\n", outputFilePath);

    return EXIT_SUCCESS;
}
