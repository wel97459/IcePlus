cmake_minimum_required(VERSION 3.10)
project(IcePlus)
set(EXECUTABLE ${PROJECT_NAME})
set(VERSION 1.0.0)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -g")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

if(NINTENDO_SWITCH)
    list(APPEND CMAKE_MODULE_PATH "$ENV{DEVKITPRO}/cmake/")
    list(APPEND CMAKE_MODULE_PATH "$ENV{DEVKITPRO}/cmake/Platform")
    include(Switch)
    include(NintendoSwitch)
endif()


# GLOB all .c files in the src directory
file(GLOB_RECURSE MyApp_SOURCES src/*.c)
# Add the executable
add_executable(${PROJECT_NAME} ${MyApp_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC src/)


# Find required SDL libraries
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

# Link the libraries to the executable using the correct target names
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    SDL2::SDL2
    SDL2_ttf::SDL2_ttf-static
    ${NX_LINKER_FLAGS}
    )

if(NINTENDO_SWITCH)
    target_compile_definitions(${PROJECT_NAME} PRIVATE __SWITCH__)
    nx_generate_nacp(${PROJECT_NAME},${PROJECT_NAME},"wel97459","1.0.0")
    nx_create_nro(${PROJECT_NAME} ROMFS "romfs")
endif()