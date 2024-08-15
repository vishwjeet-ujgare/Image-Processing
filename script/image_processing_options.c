#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "title.h"
#include "image_processing_options.h"

// Define a constant for the maximum length of the image path
#define MAX_PATH_LENGTH 1024

// Declare the function prototype
void execute_command(const char *command);

void execute_command(const char *command)
{
    int result = system(command);
    if (result != 0)
    {
        printf("Error: Command execution failed with result code %d.\n", result);
    }
    else
    {
        printf("execuation completed successfully!\n");
    }
}

void display_execution_options()
{
    printf("1. C (Serial)\n");
    printf("2. CUDA (GPU-Parallel)\n");
    printf("3. OpenMP (Multi-Core)\n");
    printf("4. Back to Functionalities Menu\n");
    printf("********************************************************\n");
    printf("Enter your choice (1-4): ");
}

void start_image_processing()
{
    int choice;
    char image_path[MAX_PATH_LENGTH];
    char new_image_path[MAX_PATH_LENGTH];
    char command[MAX_PATH_LENGTH + 200];

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
        while (getchar() != '\n')
            ;

        switch (choice)
        {
        case 1:

            int sub_choice;
            do
            {
                clear_screen();
                display_title();
                printf("Current Image Path: %s\n", image_path);
                printf("Processing Grayscale Conversion...\n");
                printf("Select the code to execute for Grayscale Conversion:\n");
                printf("1. C (Serial)\n");
                printf("2. CUDA (GPU-Parallel)\n");
                printf("3. OpenMP (Multi-Core)\n");
                printf("4. Back to Functionalities Menu\n");
                printf("Enter your choice (1-4): ");
                scanf("%d", &sub_choice);
                getchar(); // Consume newline character left by scanf

                switch (sub_choice)
                {
                case 1: // C (Serial)
                    snprintf(command, sizeof(command), "../src/grayscale/serial/convert_to_grayscale \"%s\"", image_path);
                    printf("Executing C (Serial) Grayscale Conversion...\n");
                    execute_command(command);
                    break;

                case 2: // CUDA (GPU-Parallel)
                    snprintf(command, sizeof(command), "../src/grayscale/cuda/a.out \"%s\"", image_path);
                    printf("Executing CUDA (GPU-Parallel) Grayscale Conversion...\n");
                    execute_command(command);
                    break;

                case 3: // OpenMP (Multi-Core)
                    snprintf(command, sizeof(command), "../src/grayscale/openMP/openMP_convert_to_grayscale \"%s\"", image_path);
                    printf("Executing OpenMP (Multi-Core) Grayscale Conversion...\n");
                    execute_command(command);
                    break;

                case 4:     // Back to Functionalities Menu
                    return; // Exit the function to go back to the functionalities menu

                default:
                    printf("Invalid choice. Please select a valid option (1-4).\n");
                    break;
                }

                printf("Press Enter to continue...");
                getchar(); // Wait for user to press Enter before showing the menu again
            } while (sub_choice != 4); // Continue loop until user chooses to go back
            break;

        case 2:
            while (1)
            {
                clear_screen();
                display_title();
                printf("Current Image Path: %s\n", image_path);
                printf("-------------------------------------------\n");
                printf("Select Intensity Normalization Implementation:\n");
                printf("1. C (Serial)\n");
                printf("2. CUDA (GPU-Parallel)\n");
                printf("3. OpenMP (Multi-Core)\n");
                printf("4. Back to Functionalities Menu\n");
                printf("********************************************************\n");
                printf("Enter your choice (1-4): ");
                scanf("%d", &choice);

                // Clear the newline character left by scanf from the input buffer
                while (getchar() != '\n')
                    ;

                switch (choice)
                {
                case 1: // C (Serial)
                    snprintf(command, sizeof(command), "../src/intensity_normalization/c_intensity \"%s\"", image_path);
                    printf("Executing C (Serial) Intensity Normalization...\n");
                    execute_command(command);
                    break;

                case 2: // CUDA (GPU-Parallel)
                    snprintf(command, sizeof(command), "../src/intensity_normalization/a.out \"%s\"", image_path);
                    printf("Executing CUDA (GPU-Parallel) Intensity Normalization...\n");
                    execute_command(command);
                    break;

                case 3: // OpenMP (Multi-Core)
                    snprintf(command, sizeof(command), "../src/intensity_normalization/openmp_intensity \"%s\"", image_path);
                    printf("Executing OpenMP (Multi-Core) Intensity Normalization...\n");
                    execute_command(command);
                    break;

                case 4:     // Back to Functionalities Menu
                    return; // Exit the function to go back to the functionalities menu

                default:
                    printf("Invalid choice. Please select a valid option (1-4).\n");
                    break;
                }

                printf("Intensity Normalization processing completed successfully!\n");
                printf("Press Enter to return to the Intensity Normalization menu...");
                getchar(); // Wait for the user to press Enter before showing the menu again
            }
            break;
        case 3: // Edge Detection
            clear_screen();
            display_title();
            printf("Current Image Path: %s\n", image_path);
            printf("Select the code to execute for Edge Detection:\n");
            printf("1. C (Serial)\n");
            printf("2. OpenMP (Multi-Core)\n");
            printf("3. CUDA (GPU-Parallel)\n");
            printf("4. Back to Functionalities Menu\n");
            printf("********************************************************\n");

            int edge_detection_choice;
            scanf("%d", &edge_detection_choice);

            // Clear the newline character left by scanf from the input buffer
            while (getchar() != '\n')
                ;

            switch (edge_detection_choice)
            {
            case 1: // C (Serial)
                clear_screen();
                display_title();
                printf("Current Image Path: %s\n", image_path);
                printf("Processing Edge Detection with C (Serial)...\n");
                snprintf(command, sizeof(command), "../src/edge_detection/c/c_edge_detection \"%s\"", image_path);
                printf("Executing C (Serial) Edge Detection...\n");
                execute_command(command);
                break;

            case 2: // OpenMP (Multi-Core)
                clear_screen();
                display_title();
                printf("Current Image Path: %s\n", image_path);
                printf("Processing Edge Detection with OpenMP (Multi-Core)...\n");
                snprintf(command, sizeof(command), "../src/edge_detection/openmp/openmp_edge_detection \"%s\"", image_path);
                printf("Executing OpenMP (Multi-Core) Edge Detection...\n");
                execute_command(command);
                break;

            case 3: // CUDA (GPU-Parallel)
                clear_screen();
                display_title();
                printf("Current Image Path: %s\n", image_path);
                printf("Processing Edge Detection with CUDA (GPU-Parallel)...\n");

                snprintf(command, sizeof(command), "../src/edge_detection/cuda/a.out \"%s\"", image_path);
                printf("Executing CUDA (GPU-Parallel) Edge Detection...\n");
                execute_command(command);
                break;

            case 4:     // Back to Functionalities Menu
                return; // Exit the function to go back to the functionalities menu

            default:
                printf("Invalid choice. Please select a valid option (1-4).\n");
                break;
            }

            printf("Edge Detection completed successfully!\n");
            break;

        case 4: // histogram_equalization
            while (1)
            {
                clear_screen();
                display_title();
                printf("Current Image Path: %s\n", image_path);
                printf("-------------------------------------------\n");
                printf("Select Histogram Equalization Implementation:\n");
                printf("1. C (Serial)\n");
                printf("2. CUDA (GPU-Parallel)\n");
                printf("3. OpenMP (Multi-Core)\n");
                printf("4. Back to Functionalities Menu\n");
                printf("********************************************************\n");
                printf("Enter your choice (1-4): ");
                scanf("%d", &choice);

                // Clear the newline character left by scanf from the input buffer
                while (getchar() != '\n')
                    ;

                switch (choice)
                {
                case 1: // C (Serial)
                    snprintf(command, sizeof(command), "../src/histrogram/c/c_histrogram \"%s\"", image_path);
                    printf("Executing C (Serial) Histogram Equalization...\n");
                    execute_command(command);
                    break;

                case 2: // CUDA (GPU-Parallel)
                    snprintf(command, sizeof(command), "../src/histogram_equalization/cuda/histogram_equalization_cuda \"%s\"", image_path);
                    printf("Executing CUDA (GPU-Parallel) Histogram Equalization...\n");
                    execute_command(command);
                    break;

                case 3: // OpenMP (Multi-Core)
                    snprintf(command, sizeof(command), "../src/histrogram/openmp/openmp_histrogram \"%s\"", image_path);
                    printf("Executing OpenMP (Multi-Core) Histogram Equalization...\n");
                    execute_command(command);
                    break;

                case 4:     // Back to Functionalities Menu
                    return; // Exit the function to go back to the functionalities menu

                default:
                    printf("Invalid choice. Please select a valid option (1-4).\n");
                    break;
                }

                printf("Histogram Equalization processing completed successfully!\n");
                printf("Press Enter to return to the Histogram Equalization menu...");
                getchar(); // Wait for the user to press Enter before showing the menu again
            }

            break;
        case 5: // Gaussian Blur
            while (1)
            {
                clear_screen();
                display_title();
                printf("Current Image Path: %s\n", image_path);
                display_execution_options();
                scanf("%d", &choice);

                // Clear the newline character left by scanf from the input buffer
                while (getchar() != '\n')
                    ;

                switch (choice)
                {
                case 1: // C (Serial)
                    snprintf(command, sizeof(command), "../src/gaussian_blur/c/sequenctial_gaussian_blur \"%s\"", image_path);
                    printf("Executing C (Serial) Gaussian Blur...\n");
                    execute_command(command);
                    break;

                case 2: // CUDA (GPU-Parallel)
                    snprintf(command, sizeof(command), "../src/gaussian_blur/cuda/gaussian_blur_cuda \"%s\"", image_path);
                    printf("Executing CUDA (GPU-Parallel) Gaussian Blur...\n");
                    execute_command(command);
                    break;

                case 3: // OpenMP (Multi-Core)
                    snprintf(command, sizeof(command), "../src/gaussian_blur/openmp/gaussian_blur_openmp \"%s\"", image_path);
                    printf("Executing OpenMP (Multi-Core) Gaussian Blur...\n");
                    execute_command(command);
                    break;

                case 4:     // Back to Functionalities Menu
                    return; // Exit the function to go back to the functionalities menu

                default:
                    printf("Invalid choice. Please select a valid option (1-4).\n");
                    break;
                }

                printf("Gaussian Blur processing completed successfully!\n");
                printf("Press Enter to return to the Gaussian Blur menu...");
                getchar(); // Wait for the user to press Enter before showing the menu again
            }
            break;

        case 6:

            while (1)
            {
                clear_screen();
                display_title();
               
                printf("-------------------------------------------\n");
                printf("Select Denoising Equalization Implementation:\n");
                printf("1. C (Serial)\n");
                printf("2. CUDA (GPU-Parallel)\n");
                printf("3. OpenMP (Multi-Core)\n");
                printf("4. Back to Functionalities Menu\n");
                printf("********************************************************\n");
                scanf("%d", &choice);

                // Clear the newline character left by scanf from the input buffer
                while (getchar() != '\n')
                    ;

                switch (choice)
                {
                case 1: // C (Serial)
                    snprintf(command, sizeof(command), "../src/denosing/c/denoising \"%s\"", image_path);
                    printf("Executing C (Serial) Denosing Blur...\n");
                    execute_command(command);
                    break;

                case 2: // CUDA (GPU-Parallel)
                    printf("\n As of know execution with cuda functinality is not available\n");
                    break;

                case 3: // OpenMP (Multi-Core)
                    printf("\n As of now execution with openMP functinality is not available\n");
                    break;

                case 4: // Back to Functionalities Menu
                    return;

                default:
                    printf("Invalid choice. Please select a valid option (1-4).\n");
                    break;
                }

                getchar(); // Wait for the user to press Enter before showing the menu again
            }
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
