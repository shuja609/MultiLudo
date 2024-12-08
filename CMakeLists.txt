cmake_minimum_required (VERSION 3.12)

project ("MultiLudo")
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

# Raylib vars - set RAYLIB_DIR to a directory with lib/ and include/ folders of Raylib
set(RAYLIB_DIR "/usr/local" CACHE PATH "Raylib install directory")
message("Raylib directory: " ${RAYLIB_DIR})

# Project config
include_directories(${RAYLIB_DIR}/include)
include_directories(${CMAKE_SOURCE_DIR}/include)
link_directories(${RAYLIB_DIR}/lib)

# Add source files
file(GLOB PROJECT_SOURCES 
    "src/*.cpp"
)

add_executable(${PROJECT_NAME} ${PROJECT_SOURCES})

target_link_libraries(${PROJECT_NAME} raylib Threads::Threads)
