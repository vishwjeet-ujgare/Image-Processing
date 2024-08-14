#include <stdio.h>
#include <stdlib.h>
#include "about.h"
#include "functionalities.h"
#include "title.h"
#include "image_processing_options.h"


void display_welcome() {
    display_title();  // Show the title without clearing the screen
    printf("This application provides the following functionalities:\n");
    printf("1. Grayscale Conversion\n");
    printf("2. Intensity Normalization\n");
    printf("3. Edge Detection\n");
    printf("4. Histogram Equalization\n");
    printf("5. Gaussian Blur\n");
    printf("6. Denoising\n");
    printf("********************************************************\n");
}

void display_menu() {
    int choice;
    
    while (1) {
        clear_screen();  // Clears the screen before showing the menu
        display_title();
        printf("Menu:\n");
        printf("1. About\n");
        printf("2. Functionalities\n");
        printf("3. Start Image Processing\n");
        printf("4. Exit\n");
        printf("********************************************************\n");
        printf("Enter your choice (1-4): ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                clear_screen();  // Clear screen before showing the about section
                display_title();
                display_about();
                break;
            case 2:
                clear_screen();  // Clear screen before showing functionalities
                display_title();
                display_functionalities();
                break;
            case 3:
                start_image_processing();  // Start the image processing
                break;
            case 4:
                clear_screen();  // Clear screen before exiting
                printf("Exiting the application.\n");
                exit(0);
            default:
                printf("Invalid choice. Please enter a number between 1 and 4.\n");
                break;
        }

        printf("Press Enter to return to the menu...");
        getchar();  // Wait for user to press Enter
        getchar();  // This extra getchar() handles the newline from pressing Enter
    }
}

int main() {
    clear_screen();  // Clears the screen at the start
    display_welcome();
    display_menu();  // Displays the menu

    return 0;
}
