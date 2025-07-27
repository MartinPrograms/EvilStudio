cmake_minimum_required(VERSION 3.31)
project(micro_ui C)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

include(FetchContent)

FetchContent_Declare(
        MICRO_UI
        GIT_REPOSITORY https://github.com/rxi/microui.git
        GIT_TAG master
)

FetchContent_MakeAvailable(MICRO_UI)

# Create the static library from microui.c
file(GLOB MICRO_UI_SOURCES "${micro_ui_SOURCE_DIR}/src/*.c")
file(GLOB CUSTOM_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
file(GLOB CUSTOM_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")

add_library(micro_ui STATIC ${MICRO_UI_SOURCES} ${CUSTOM_SOURCES} ${CUSTOM_HEADERS})

target_include_directories(micro_ui PUBLIC ${micro_ui_SOURCE_DIR}/src ${CMAKE_CURRENT_SOURCE_DIR})