#include <stdio.h>
#include <stdlib.h>
#include "about.h"
#include "functionalities.h"

// Function to display the welcome page
void display_welcome() {
    printf("********************************************************\n");
    printf("         Welcome to Image Processing         \n");
    printf("   High-Performance Solutions for Image Processing   \n");
    printf("********************************************************\n");
    printf("This application provides the following functionalities:\n");
    printf("1. Grayscale Conversion\n");
    printf("2. Intensity Normalization\n");
    printf("3. Edge Detection\n");
    printf("4. Histogram Equalization\n");
    printf("5. Gaussian Blur\n");
    printf("6. Denoising\n");
    printf("********************************************************\n");
}

// Function to display the menu and handle user input
void display_menu() {
    int choice;
    
    while (1) {
        printf("********************************************************\n");
        printf("Menu:\n");
        printf("1. About\n");
        printf("2. Functionalities\n");
        printf("3. Start Application\n");
        printf("4. Exit\n");
        printf("********************************************************\n");
        printf("Enter your choice (1-4): ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                display_about();
                break;
            case 2:
                display_functionalities();
                break;
            case 3:
                // Placeholder for starting the application
                printf("Starting application...\n");
                // Implement the start functionality here
                return;
            case 4:
                printf("Exiting the application.\n");
                exit(0);
            default:
                printf("Invalid choice. Please enter a number between 1 and 4.\n");
                break;
        }
    }
}

int main() {
    // Display the welcome page
    display_welcome();
    // Display the menu and handle user choices
    display_menu();

    return 0;
}
