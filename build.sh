#!/bin/bash


# - **Shuja Uddin** (22i-2553) | SE-D | FAST NUCES Islamabad
# - **Amna Hassan** (22i-8759) | SE-D | FAST NUCES Islamabad  
# - **Samra Saleem** (22i-2727) | SE-D | FAST NUCES Islamabad


#------------------------------------------------------------------------------
# MultiLudo Build Script
# This script handles the build process for the MultiLudo game, including:
# - Creating and managing build directory
# - Generating build files with CMake
# - Compiling the project
# - Setting up game assets and permissions
# - Launching the game
#------------------------------------------------------------------------------

# Create build directory if it doesn't exist
# -p flag creates parent directories as needed
mkdir -p build

# Navigate into the build directory for CMake operations
cd build

# Generate build files using CMake
# .. specifies parent directory containing CMakeLists.txt
cmake ..

# Compile the project using generated Makefile
# This step builds all targets defined in CMake
make

# Copy game assets to build directory
# -r flag copies directories recursively
cp -r ../assets ./

# Set executable permissions for the game binary
# +x adds execute permission for all users
chmod +x MultiLudo

# Notify user of build completion and launch game
echo "Build complete. Starting game..."

# Navigate to build directory and launch game
cd build
./MultiLudo
cd ..