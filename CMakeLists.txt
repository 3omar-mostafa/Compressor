cmake_minimum_required(VERSION 3.0)

set(PROJECT_NAME Compressor)

project(${PROJECT_NAME})

set(CMAKE_CXX_STANDARD 11)

file(GLOB_RECURSE UTILS Utils/*)
file(GLOB_RECURSE COMPRESSORS Compressors/*)

set(SOURCE_FILES_LIST main.cpp ${UTILS} ${COMPRESSORS})

add_executable(${PROJECT_NAME} ${SOURCE_FILES_LIST})

add_subdirectory(Bit-String)
target_link_libraries(${PROJECT_NAME} Bit_String)
include_directories(Bit-String/Bit_String)

########################################### For Visual Studio ###########################################

# Generate Folder Hierarchy instead of adding all files in the same folder
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES_LIST})

# Group Cmake predefined projects in CMakePredefinedTargets folder (as ZERO_CHECK , ALL_BUILD)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Set this project as startup project
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})