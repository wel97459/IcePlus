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
    include(Switch)
    include(Platform/NintendoSwitch)
endif()


# GLOB all .c files in the src directory
file(GLOB_RECURSE MyApp_SOURCES src/*.c)
# Add the executable
add_executable(${PROJECT_NAME} ${MyApp_SOURCES})
target_include_directories(${PROJECT_NAME} PUBLIC src/)

# Find required SDL libraries
find_package(SDL2 REQUIRED)
find_package(SDL2_ttf REQUIRED)

if(NOT NINTENDO_SWITCH)
    find_package(SDL2_mixer REQUIRED)
    set(USE_LIB_MIXER "SDL2_mixer::SDL2_mixer")
endif()


message(STATUS, ${CMAKE_CURRENT_SOURCE_DIR})
# Link the libraries to the executable using the correct target names
target_link_libraries(${PROJECT_NAME}
    PRIVATE
    SDL2::SDL2
    SDL2_ttf::SDL2_ttf-static
    ${USE_LIB_MIXER}
    ${NX_LINKER_FLAGS}
    )

if(NINTENDO_SWITCH)
    target_compile_definitions(${PROJECT_NAME} PRIVATE __SWITCH__)
    nx_generate_nacp(OUTPUT "${PROJECT_NAME}.nacp" NAME "${PROJECT_NAME}" AUTHOR "wel97459" VERSION "1.0.0")
    nx_create_nro(${PROJECT_NAME} ROMFS "romfs" ICON "res/icon.jpg" NACP "${PROJECT_NAME}.nacp")
endif()