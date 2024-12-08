

# - **Shuja Uddin** (22i-2553) | SE-D | FAST NUCES Islamabad
# - **Amna Hassan** (22i-8759) | SE-D | FAST NUCES Islamabad  
# - **Samra Saleem** (22i-2727) | SE-D | FAST NUCES Islamabad

#------------------------------------------------------------------------------
# MultiLudo CMake Configuration
# This file configures the build system for the MultiLudo game project
#------------------------------------------------------------------------------

# Specify minimum CMake version required
cmake_minimum_required (VERSION 3.12)

# Define project name and set C++17 standard
project ("MultiLudo")
set(CMAKE_CXX_STANDARD 17)

#------------------------------------------------------------------------------
# Threading Configuration
# Configure threading preferences and find required threading package
#------------------------------------------------------------------------------
set(CMAKE_THREAD_PREFER_PTHREAD TRUE)        # Prefer POSIX threads
set(THREADS_PREFER_PTHREAD_FLAG TRUE)        # Use -pthread compiler flag
find_package(Threads REQUIRED)               # Find threading package

#------------------------------------------------------------------------------
# Raylib Configuration
# Set up paths and variables for Raylib graphics library integration
#------------------------------------------------------------------------------
# Define Raylib installation directory (contains lib/ and include/)
set(RAYLIB_DIR "/usr/local" CACHE PATH "Raylib install directory")
message("Raylib directory: " ${RAYLIB_DIR})

#------------------------------------------------------------------------------
# Project Build Configuration
# Configure include paths and library directories
#------------------------------------------------------------------------------
# Add include directories for header files
include_directories(${RAYLIB_DIR}/include)   # Raylib headers
include_directories(${CMAKE_SOURCE_DIR}/include)  # Project headers
link_directories(${RAYLIB_DIR}/lib)          # Raylib library location

#------------------------------------------------------------------------------
# Source Files Configuration
# Collect all source files for the project
#------------------------------------------------------------------------------
# Gather all .cpp files from src directory
file(GLOB PROJECT_SOURCES 
    "src/*.cpp"
)

#------------------------------------------------------------------------------
# Target Configuration
# Define executable and link required libraries
#------------------------------------------------------------------------------
# Create executable from collected sources
add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

# Link against Raylib and threading libraries
target_link_libraries(${PROJECT_NAME} raylib Threads::Threads)
