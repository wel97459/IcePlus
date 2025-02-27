cmake_minimum_required(VERSION 3.10)
project(IcePlus)

# Add the executable
add_executable(${PROJECT_NAME} main.c)

# Find required SDL libraries
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)

# Link the libraries to the executable using the correct target names
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    SDL2::SDL2
    SDL2_image::SDL2_image
    SDL2_mixer::SDL2_mixer
)