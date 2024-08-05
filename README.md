
# Image Processing
**High-Performance Solutions for Image Processing**

## Project Overview
Image Processing / ImageMaster is an advanced image processing application designed to provide high-performance solutions for both medical and general image analysis.
Leveraging the power of CUDA for parallel processing and C for serial processing, ImageMaster efficiently handles large image datasets such as CT
 scans and MRIs, ensuring optimized performance and accuracy.

## Project Objectives
- **Develop 10 Image Processing Functionalities:** Implement diverse tasks including noise reduction, contrast enhancement, sharpness enhancement,
edge detection, gamma correction, histogram equalization, brightness adjustment, color correction, adaptive filtering, and image resizing and scaling.
- **Serial and Parallel Versions:** Provide both serial (C) and parallel (CUDA) implementations for each functionality to compare performance and efficiency.
- **Parameter Customization:** Allow users to manually set parameters or use default settings for each image processing task.
- **Optimized Hardware Utilization:** Enable the application to effectively utilize CPU, GPU, or both, maximizing performance based on available hardware.

## Functionalities
1. **Noise Reduction (Denoising)**
2. **Brightness Adjustment**
3. **Contrast Enhancement**
4. **Color Correction**
5. **Sharpness Enhancement**
6. **Image Resizing and Scaling**
7. **Edge Detection**
8. **Histogram Equalization**
9. **Adaptive Filtering**
10. **Gamma Correction**

## Technology Stack
- **Programming Languages:** C, CUDA
- **Development Tools:** GCC, NVCC,
- **Version Control:** Git, GitHub

## Repository Structure
```plaintext
ImageMaster/
│
├── src/
│   ├── noise_reduction/
│   ├── brightness_adjustment/
│   ├── contrast_enhancement/
│   ├── color_correction/
│   ├── sharpness_enhancement/
│   ├── image_resizing/
│   ├── edge_detection/
│   ├── histogram_equalization/
│   ├── adaptive_filtering/
│   ├── gamma_correction/
│   └── common/
│
├── docs/
│   ├── research_notes/
│   ├── usage/
│   └── api/
│
├── scripts/
│   ├── build.sh
│   ├── setup.sh
│   └── run_tests.sh
│
├── tests/
│   ├── test_noise_reduction/
│   ├── test_brightness_adjustment/
│   ├── test_contrast_enhancement/
│   ├── test_color_correction/
│   ├── test_sharpness_enhancement/
│   ├── test_image_resizing/
│   ├── test_edge_detection/
│   ├── test_histogram_equalization/
│   ├── test_adaptive_filtering/
│   ├── test_gamma_correction/
│   └── common/
│
├── README.md
├── LICENSE
├── .gitignore
└── CMakeLists.txt
'''

## Contribution Guidelines
- **Branching Strategy:** Use feature branches for new functionalities (`feature-functionality_name`) and bugfix branches for fixes (`bugfix-description`).
- **Commit Messages:** Use clear, concise commit messages following a consistent format.
- **Pull Requests:** Ensure PRs have a clear title and description, link relevant issues, assign reviewers, and provide constructive feedback during code reviews.
- **Documentation:** Update the main `README.md` with an overview and usage instructions, and ensure each functionality has its own detailed `README.md`.


## Steps for Team Members

1. **Clone the Repository**
   Each team member should clone the repository to their local machine.
   ```bash
   git clone https://github.com/yourusername/Image-Processing.git
   ```

2. **Create Branches for Individual Work**
   Each team member should create a branch for their specific functionality.
   ```bash
   git checkout -b feature-functionality_name
   ```

3. **Follow the Directory Structure**
   Each team member should create and work within their respective directories following the standard structure outlined earlier.

4. **Commit and Push Changes**
   Regularly commit changes with clear, descriptive commit messages.
   ```bash
   git add .
   git commit -m "Implemented serial version of brightness adjustment"
   git push origin feature-functionality_name
   ```

5. **Create Pull Requests**
   When a functionality is ready for review, team members should create a pull request (PR) from their feature branch to the main branch.
   1. Go to the repository on GitHub.
   2. Click on **Pull requests**.
   3. Click on **New pull request**.
   4. Select the base branch (main) and the compare branch (feature-functionality_name).
   5. Add a descriptive title and detailed description of the changes.
   6. Assign reviewers if needed and create the pull request.


## Contact
For any questions or inquiries, please contact the project lead:
- **Vishwjeet Ujgare** - Email: vrvishwujgare@gmail.com

### Team Members:
- **Anmol Pimpale** - Email: anmolpimpale90@gmail.com
- **Deepshikha Paikara** - Email: shikhapaikara@gmail.com
- **Maroti Kasbe** - Email: maroti.kasbe26@gmail.com
- **Sonali Waghmare** - Email: waghmaress162000@gmail.com
```

Feel free to customize the repository structure and other details as needed for your project.
