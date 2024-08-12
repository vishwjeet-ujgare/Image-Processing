#include <stdio.h>
#include <stdlib.h>
#include "title.h"
#include "image_processing_options.h"



void start_image_processing() {
    int choice;
    clear_screen();
    display_title();
    printf("Select an Image Processing Functionality:\n");
    printf("1. Grayscale Conversion\n");
    printf("2. Intensity Normalization\n");
    printf("3. Edge Detection\n");
    printf("4. Histogram Equalization\n");
    printf("5. Gaussian Blur\n");
    printf("6. Denoising\n");
    printf("7. Back to Main Menu\n");
    printf("********************************************************\n");
    printf("Enter your choice (1-7): ");
    scanf("%d", &choice);

    switch (choice) {
        case 1:
            clear_screen();
            display_title();
            printf("Processing Grayscale Conversion...\n");
            // Add code to perform Grayscale Conversion
            printf("Grayscale Conversion completed successfully!\n");
            break;
        case 2:
            clear_screen();
            display_title();
            printf("Processing Intensity Normalization...\n");
            // Add code to perform Intensity Normalization
            printf("Intensity Normalization completed successfully!\n");
            break;
        case 3:
            clear_screen();
            display_title();
            printf("Processing Edge Detection...\n");
            // Add code to perform Edge Detection
            printf("Edge Detection completed successfully!\n");
            break;
        case 4:
            clear_screen();
            display_title();
            printf("Processing Histogram Equalization...\n");
            // Add code to perform Histogram Equalization
            printf("Histogram Equalization completed successfully!\n");
            break;
        case 5:
            clear_screen();
            display_title();
            printf("Processing Gaussian Blur...\n");
            // Add code to perform Gaussian Blur
            printf("Gaussian Blur completed successfully!\n");
            break;
        case 6:
            clear_screen();
            display_title();
            printf("Processing Denoising...\n");
            // Add code to perform Denoising
            printf("Denoising completed successfully!\n");
            break;
        case 7:
            return;  // Go back to main menu
        default:
            printf("Invalid choice. Please select a valid functionality.\n");
            break;
    }

    printf("Press Enter to return to the menu...");
    getchar();  // Wait for user to press Enter
    getchar();  // This extra getchar() handles the newline from pressing Enter
}
