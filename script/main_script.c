#include <stdio.h>
#include <stdlib.h>

// Define directories and file names
#define SRC_DIR "./src"
#define INPUT_DIR "./data/input"
#define OUTPUT_DIR "./data/output"
#define BUILD_DIR "./build"
#define LIB_DIR "./external_lib/stb"

// Function to compile a specific functionality
int compile(const char *func_name, const char *compiler, const char *flags, const char *output_file, const char *source_file) {
    char command[1024];
    snprintf(command, sizeof(command), "%s %s -o %s/%s %s -I%s", compiler, flags, BUILD_DIR, output_file, source_file, LIB_DIR);
    printf("Compiling %s...\n", func_name);
    return system(command);
}

// Function to run the compiled executable
int run(const char *func_name, const char *executable, const char *input_file, const char *output_file) {
    char command[1024];
    snprintf(command, sizeof(command), "%s/%s %s/%s %s/%s", BUILD_DIR, executable, INPUT_DIR, input_file, OUTPUT_DIR, output_file);
    printf("Running %s...\n", func_name);
    return system(command);
}

int main() {
    // // Create build directory if it doesn't exist
    // system("mkdir -p " BUILD_DIR);

    // Define file names for grayscale functionality
    const char *func_name = "grayscale";
    char source_c[256], source_cuda[256], source_openmp[256];
    char executable_c[256], executable_cuda[256], executable_openmp[256];
    char input_image[256] = "input_image.ppm";  // Default input image name
    char output_image_c[256], output_image_cuda[256], output_image_openmp[256];

    // Generate file names
    snprintf(source_c, sizeof(source_c), "%s/%s_c.c", SRC_DIR, func_name);
    snprintf(source_cuda, sizeof(source_cuda), "%s/%s_cuda.cu", SRC_DIR, func_name);
    snprintf(source_openmp, sizeof(source_openmp), "%s/%s_openmp.c", SRC_DIR, func_name);

    snprintf(executable_c, sizeof(executable_c), "%s_c", func_name);
    snprintf(executable_cuda, sizeof(executable_cuda), "%s_cuda", func_name);
    snprintf(executable_openmp, sizeof(executable_openmp), "%s_openmp", func_name);

    snprintf(output_image_c, sizeof(output_image_c), "%s_output_c.ppm", func_name);
    snprintf(output_image_cuda, sizeof(output_image_cuda), "%s_output_cuda.ppm", func_name);
    snprintf(output_image_openmp, sizeof(output_image_openmp), "%s_output_openmp.ppm", func_name);

    // Compile and run C version
    if (compile(func_name, "gcc", "-fopenmp", executable_c, source_c) == 0) {
        run(func_name, executable_c, input_image, output_image_c);
    }

    // Compile and run CUDA version
    if (compile(func_name, "nvcc", "-L/usr/local/cuda/lib64 -lcuda -lcudart", executable_cuda, source_cuda) == 0) {
        run(func_name, executable_cuda, input_image, output_image_cuda);
    }

    // Compile and run OpenMP version
    if (compile(func_name, "gcc", "-fopenmp", executable_openmp, source_openmp) == 0) {
        run(func_name, executable_openmp, input_image, output_image_openmp);
    }

    printf("Grayscale functionality processed.\n");
    return 0;
}
