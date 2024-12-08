#!/bin/bash

# Create build directory if it doesn't exist
mkdir -p build

# Navigate to build directory
cd build

# Generate build files
cmake ..

# Build the project
make

# Copy assets
cp -r ../assets ./

# Make the game executable
chmod +x MultiLudo

echo "Build complete. Starting game..."
cd build
./MultiLudo
cd ..