cmake_minimum_required(VERSION 3.31)
project(EvilStudio)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic -Werror -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wno-missing-field-initializers -Wno-stringop-overflow")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-exceptions -fexceptions")
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -march=native")

set(CMAKE_CXX_STANDARD 20)

# Get OpenGL
find_package(OpenGL REQUIRED)

include(FetchContent)

# Get SDL3 from https://github.com/libsdl-org/SDL.git
FetchContent_Declare(
    SDL3
    GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
    GIT_TAG        release-3.2.16
)

FetchContent_MakeAvailable(SDL3)

# Fetch https://github.com/inviwo/freetype2.git
FetchContent_Declare(
    freetype2
    GIT_REPOSITORY https://github.com/inviwo/freetype2.git
    GIT_TAG master
)

FetchContent_MakeAvailable(freetype2)

# Fetch https://github.com/nothings/stb.git

FetchContent_Declare(
        stb
        GIT_REPOSITORY https://github.com/nothings/stb.git
        GIT_TAG master
)

FetchContent_MakeAvailable(stb)

FetchContent_Declare(
        miniaudio
        GIT_REPOSITORY https://github.com/mackron/miniaudio.git
        GIT_TAG 0.11.22
)

FetchContent_MakeAvailable(miniaudio)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt
        GIT_TAG        e69e5f977d458f2650bb346dadf2ad30c5320281
) # 10.2.1

FetchContent_MakeAvailable(fmt)

# https://github.com/thestk/rtmidi.git
FetchContent_Declare(
        rtmidi
        GIT_REPOSITORY https://github.com/thestk/rtmidi.git
        GIT_TAG        6.0.0
)

FetchContent_MakeAvailable(rtmidi)

# Gather all the sources from ./src/*.cpp/h
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.h")
file(GLOB_RECURSE HEADERS "src/*.h")

add_executable(EvilStudio main.cpp ${SOURCES} ${HEADERS})

target_include_directories(EvilStudio PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
target_include_directories(EvilStudio PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/libs/glad/include)

set(MICRO_UI_DIR ${micro_ui_SOURCE_DIR})

target_include_directories(EvilStudio PRIVATE ${freetype2_SOURCE_DIR}/include ${stb_SOURCE_DIR} ${miniaudio_SOURCE_DIR})

# Add micro_ui cmakelists.txt from ./libs/micro_ui/CMakeLists.txt
add_subdirectory(libs/micro_ui)

target_link_libraries(EvilStudio PRIVATE OpenGL::GL SDL3::SDL3 freetype micro_ui miniaudio fmt::fmt rtmidi)

target_sources(EvilStudio PRIVATE libs/glad/src/glad.c)


# After building copy the ./resources to the build directory
add_custom_command(TARGET EvilStudio POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_CURRENT_SOURCE_DIR}/resources $<TARGET_FILE_DIR:EvilStudio>/resources
)
