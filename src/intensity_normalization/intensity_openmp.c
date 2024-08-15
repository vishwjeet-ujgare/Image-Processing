#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// Function to normalize image intensity
void normalize_image(unsigned char* img, int num_pixels, unsigned char min_val, unsigned char max_val) {
    float scale = (max_val != min_val) ? 255.0f / (max_val - min_val) : 1.0f;

    #pragma omp parallel for
    for (int i = 0; i < num_pixels; i++) {
        img[i] = (unsigned char)((img[i] - min_val) * scale);
    }
}

// Function to apply contrast stretching
void apply_contrast_stretching(unsigned char* img, int num_pixels, unsigned char low_bound, unsigned char high_bound) {
    #pragma omp parallel for
    for (int i = 0; i < num_pixels; i++) {
        if (img[i] < low_bound) img[i] = 0;
        else if (img[i] > high_bound) img[i] = 255;
        else {
            img[i] = (unsigned char)(((float)(img[i] - low_bound) / (high_bound - low_bound)) * 255.0f);
        }
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
    unsigned char min_val = 255, max_val = 0;

    // Find min and max values on host
    #pragma omp parallel
    {
        unsigned char local_min = 255, local_max = 0;
        #pragma omp for
        for (int i = 0; i < num_pixels; i++) {
            if (img[i] < local_min) local_min = img[i];
            if (img[i] > local_max) local_max = img[i];
        }
        
        #pragma omp critical
        {
            if (local_min < min_val) min_val = local_min;
            if (local_max > max_val) max_val = local_max;
        }
    }

    // Normalize image intensity
    normalize_image(img, num_pixels, min_val, max_val);

    // Apply contrast stretching
    unsigned char low_bound = (unsigned char)(min_val + 0.1f * (max_val - min_val));
    unsigned char high_bound = (unsigned char)(max_val - 0.1f * (max_val - min_val));
    apply_contrast_stretching(img, num_pixels, low_bound, high_bound);

    printf("Enter the output image filename (with .jpg, .png, etc.): ");
    scanf("%255s", output_filename);

    // Save the processed image
    if (!stbi_write_jpg(output_filename, width, height, channels, img, 90)) {
        fprintf(stderr, "Error saving image %s\n", output_filename);
        stbi_image_free(img);
        return 1;
    }

    stbi_image_free(img);

    clock_t end_time = clock();
    double duration = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    printf("Image normalized, contrast enhanced, and saved to %s\n", output_filename);
    printf("Time taken for processing: %.2f seconds\n", duration);

    return 0;
}
