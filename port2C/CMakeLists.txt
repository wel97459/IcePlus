cmake_minimum_required(VERSION 3.10)
project(IcePlus)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

# GLOB all .c files in the src directory
file(GLOB_RECURSE MyApp_SOURCES src/*.c)
# Add the executable
add_executable(${PROJECT_NAME} ${MyApp_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC src/)

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