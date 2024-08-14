#include <stdlib.h>
#include <stdio.h>
#include "title.h"

void display_title() {
    printf("********************************************************\n");
    printf("         Image Processing Application         \n");
    printf("   High-Performance Solutions for Image Processing   \n");
    printf("********************************************************\n");
}

void clear_screen() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}
