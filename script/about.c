#include <stdio.h>
#include "about.h"

// ANSI escape codes for text formatting
#define BOLD "\033[1m"
#define RESET "\033[0m"

void display_about()
{
    printf("********************************************************\n");
    printf(BOLD "About Image Processing Application:" RESET "\n");
    printf("********************************************************\n");

    printf("Project Motive:\n");
    printf("\tThe project aims to demonstrate the performance improvements achieved by processing image functionalities across different languages and devices.\n");
    printf("\tBy comparing the performance of image processing tasks implemented in C, CUDA, and OpenMP, we evaluate the efficiency of GPU acceleration (NVIDIA CUDA) versus CPU multi-threading (OpenMP).\n");
    printf("\tThe project highlights how leveraging modern computational resources can significantly speed up image processing tasks.\n");
    printf("********************************************************\n");

    printf("Project Name: Image Processing\n");
    printf("Duration: 15 Days\n");
    printf("\n");

    printf("Team Members:\n");
    printf("\t1. Vishwjeet Ujgare (Project Lead)\n");
    printf("\t2. Deepshikha Paikara\n");
    printf("\t3. Sonali Waghmare\n");
    printf("\t4. Anmol Pimpale\n");
    printf("\t5. Maroti Kasbe\n");
    printf("********************************************************\n");

    printf("Project Overview:\n");
    printf("\tThis application provides high-performance image processing solutions.\n");
    printf("\tThe main objective of the project is to process images faster by leveraging both CUDA for GPU acceleration and OpenMP for multi-threading on CPUs.\n");
    printf("********************************************************\n");

    printf("Functionalities:\n");
    printf("\t1. Grayscale Conversion\n");
    printf("\t2. Intensity Normalization\n");
    printf("\t3. Edge Detection\n");
    printf("\t4. Histogram Equalization\n");
    printf("\t5. Gaussian Blur\n");
    printf("\t6. Denoising\n");
    printf("********************************************************\n");

    printf("Languages and Tools Used:\n");
    printf("\t1. C\n");
    printf("\t2. CUDA\n");
    printf("\t3. OpenMP\n");
    printf("\t4. Profiling Tools: HPCToolkit, VTune, gprof\n");
    printf("\t5. External Library: libjpeg-turbo\n");
    printf("********************************************************\n");
}
