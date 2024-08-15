#!/bin/bash

# Get the directory of this script
SCRIPT_DIR="$(dirname "$(realpath "$0")")"

# Print the script directory for debugging (optional)
echo "Current script directory: $SCRIPT_DIR"

# Navigate to the directory containing 'main_script'
# Assuming 'main_script' is in a subdirectory called 'script' within the same directory as this script
TARGET_DIR="$SCRIPT_DIR/script"

# Check if the target directory exists
if [ ! -d "$TARGET_DIR" ]; then
    echo "Error: Target directory $TARGET_DIR does not exist."
    exit 1
fi

# Navigate to the target directory
cd "$TARGET_DIR" || { echo "Failed to change directory to $TARGET_DIR"; exit 1; }

# Set environment variables (if needed)
# export LD_LIBRARY_PATH="$SCRIPT_DIR/lib:$LD_LIBRARY_PATH"

# Execute the 'main_script'
echo "Executing ./main_script"
./main_script

# Check if the execution was successful
if [ $? -ne 0 ]; then
    echo "Error: Command execution failed."
    exit 1
fi

exit 0
