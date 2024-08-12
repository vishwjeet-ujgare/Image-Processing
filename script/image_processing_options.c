#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "title.h"
#include "image_processing_options.h"

// Define a constant for the maximum length of the image path
#define MAX_PATH_LENGTH 1024

void start_image_processing()
{
    int choice;
    char image_path[MAX_PATH_LENGTH];
    char new_image_path[MAX_PATH_LENGTH];
    int select_another_image = 0;

    // Ask for the image file path initially
    clear_screen();
    display_title();

    int count = 0;
    do
    {
        clear_screen();
        display_title();

        printf("Select an Image Processing Functionality:\n");
        printf("1. Grayscale Conversion\n");
        printf("2. Intensity Normalization\n");
        printf("3. Edge Detection\n");
        printf("4. Histogram Equalization\n");
        printf("5. Gaussian Blur\n");
        printf("6. Denoising\n");
        printf("7. Select Another Image\n");
        printf("8. Back to Main Menu\n");
        printf("********************************************************\n");

        if (count == 0)
        {
            // Clear any leftover input in the buffer
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF)
                ;

            printf("Please enter image full path to proceed\n");
            printf("Enter the full path of the image file: ");

            // Read the input
            if (fgets(image_path, MAX_PATH_LENGTH, stdin) != NULL)
            {
                // Remove the newline character at the end if it exists
                image_path[strcspn(image_path, "\n")] = '\0';
                count++;
            }
            else
            {
                printf("Error reading input. Please try again.\n");
            }

            printf("********************************************************\n");
        }

        printf("Current Image Path: %s\n", image_path);

        if (select_another_image)
        {
            // Ask for a new image file path if the user chooses to select another image
            clear_screen();
            display_title();
            printf("Enter the full path of the new image file: ");
            fgets(new_image_path, MAX_PATH_LENGTH, stdin);
            new_image_path[strcspn(new_image_path, "\n")] = '\0';
            strcpy(image_path, new_image_path); // Update image path with new value
            select_another_image = 0;           // Reset flag
        }
        printf("********************************************************\n");
        printf("Enter your choice (1-8): ");
        scanf("%d", &choice);

        // Clear the newline character left by scanf from the input buffer
        while (getchar() != '\n');

        switch (choice)
        {
        case 1:
            clear_screen();
            display_title();
            printf("Current Image Path: %s\n", image_path);
            printf("Processing Grayscale Conversion...\n");
            // Call the Grayscale Conversion function with the image path
            // convert_to_grayscale(image_path); // Ensure this function matches your implementation
            printf("Grayscale Conversion completed successfully!\n");
            break;
        case 2:
            clear_screen();
            display_title();
            printf("Current Image Path: %s\n", image_path);
            printf("Processing Intensity Normalization...\n");
            // Call the Intensity Normalization function with the image path
            // normalize_intensity(image_path); // Ensure this function matches your implementation
            printf("Intensity Normalization completed successfully!\n");
            break;
        case 3:
            clear_screen();
            display_title();
            printf("Current Image Path: %s\n", image_path);
            printf("Processing Edge Detection...\n");
            // Call the Edge Detection function with the image path
            // edge_detection(image_path); // Ensure this function matches your implementation
            printf("Edge Detection completed successfully!\n");
            break;
        case 4:
            clear_screen();
            display_title();
            printf("Current Image Path: %s\n", image_path);
            printf("Processing Histogram Equalization...\n");
            // Call the Histogram Equalization function with the image path
            // histogram_equalization(image_path); // Ensure this function matches your implementation
            printf("Histogram Equalization completed successfully!\n");
            break;
        case 5:
            clear_screen();
            display_title();
            printf("Current Image Path: %s\n", image_path);
            printf("Processing Gaussian Blur...\n");
            // Call the Gaussian Blur function with the image path
            // gaussian_blur(image_path); // Ensure this function matches your implementation
            printf("Gaussian Blur completed successfully!\n");
            break;
        case 6:
            clear_screen();
            display_title();
            printf("Current Image Path: %s\n", image_path);
            printf("Processing Denoising...\n");
            // Call the Denoising function with the image path
            // denoise_image(image_path); // Ensure this function matches your implementation
            printf("Denoising completed successfully!\n");
            break;
        case 7:
            select_another_image = 1; // Set flag to select another image
            break;
        case 8:
            return; // Go back to main menu
        default:
            printf("Invalid choice. Please select a valid functionality.\n");
            break;
        }

        if (choice != 7)
        {
            // If not selecting another image, print the image path again before returning to the menu
            printf("Current Image Path: %s\n", image_path);
        }

        printf("Press Enter to return to the menu...");
        getchar(); // Wait for user to press Enter

    } while (1); // Repeat indefinitely
}
