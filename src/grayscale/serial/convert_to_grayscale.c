#include <stdio.h>
#include <stdlib.h>
#include <jpeglib.h>
#include <string.h>
#include <time.h>   // Include time.h for clock()
#include <libgen.h> // For basename()

#define MAX_PATH_LENGTH 512 // Increased buffer size

void convertToGrayscale(const char *inputFilePath, const char *outputFilePath)
{
    struct jpeg_decompress_struct cinfo;
    struct jpeg_compress_struct cinfo_out;
    struct jpeg_error_mgr jerr;

    FILE *inputFile, *outputFile;
    JSAMPARRAY buffer;
    int row_stride;

    // Open the input file
    if ((inputFile = fopen(inputFilePath, "rb")) == NULL)
    {
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
    if ((outputFile = fopen(outputFilePath, "wb")) == NULL)
    {
        fprintf(stderr, "Error opening output file: %s\n", outputFilePath);
        perror("fopen"); // Detailed error message
        exit(EXIT_FAILURE);
    }

    jpeg_stdio_dest(&cinfo_out, outputFile);

    cinfo_out.image_width = cinfo.output_width;
    cinfo_out.image_height = cinfo.output_height;
    cinfo_out.input_components = 1; // Grayscale output
    cinfo_out.in_color_space = JCS_GRAYSCALE;

    jpeg_set_defaults(&cinfo_out);
    jpeg_start_compress(&cinfo_out, TRUE);

    // Process each scanline
    while (cinfo.output_scanline < cinfo.output_height)
    {
        jpeg_read_scanlines(&cinfo, buffer, 1);

        // Convert each pixel to grayscale
        for (int i = 0; i < row_stride; i += 3)
        {
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

int main(int argc, char *argv[])
{
    char outputFilePath[MAX_PATH_LENGTH];
    const char *inputFilePath;

    // Ensure an input file path is passed as a command-line argument
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s <input_file_path>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Get the input file path from the command-line argument
    inputFilePath = argv[1];

    // Ask for the output image path
    printf("Enter the full path for the output image (or press Enter for default): ");
    if (fgets(outputFilePath, sizeof(outputFilePath), stdin) != NULL)
    {
        // Remove newline character from fgets
        outputFilePath[strcspn(outputFilePath, "\n")] = '\0';
    }

    // Extract the base name from the input file path
    char inputFileName[256];
    strcpy(inputFileName, basename((char *)inputFilePath)); // Extract "image.jpg" from "/path/image.jpg"

    // Remove the extension from the base name
    char baseName[256];
    strcpy(baseName, inputFileName);
    char *dot = strrchr(baseName, '.');
    if (dot)
        *dot = '\0'; // "image" from "image.jpg"

    // Append "_serial.jpg" to the base name
    strcat(baseName, "_serial.jpg");

    // Use default output path if the user does not provide one
    if (strlen(outputFilePath) == 0)
    {
        snprintf(outputFilePath, sizeof(outputFilePath),
                 "/home/hpcap/Desktop/image_process/Image-Processing/data/output/cuda_processed_img/grayscale/c/%s", baseName);
    }
    else if (outputFilePath[strlen(outputFilePath) - 1] == '/')
    {
        // User provided only a folder path, append the generated file name
        snprintf(outputFilePath + strlen(outputFilePath), sizeof(outputFilePath) - strlen(outputFilePath), "%s", baseName);
    }

    // Ensure the path does not exceed buffer size
    if (strlen(outputFilePath) >= MAX_PATH_LENGTH)
    {
        fprintf(stderr, "Error: Output file path is too long.\n");
        return EXIT_FAILURE;
    }

    // Measure start time
    clock_t start_time = clock();

    // Convert the image to grayscale
    convertToGrayscale(inputFilePath, outputFilePath);

    // Measure end time
    clock_t end_time = clock();

    // Calculate the time taken in seconds
    double time_taken = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("-----------------------------------------------------------");
    printf("\nGrayscale image saved successfully to %s.\n", outputFilePath);
    printf("-----------------------------------------------------------\n");

    printf("Time taken for conversion: %.2f seconds\n", time_taken);
    printf("-----------------------------------------------------------\n");


    return EXIT_SUCCESS;
}
