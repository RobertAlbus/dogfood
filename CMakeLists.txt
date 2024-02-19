include(CMakeLists.vcpkg.txt)
cmake_minimum_required(VERSION 3.14)

project(Clover_Proof)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_PARALLEL_LEVEL 8 STRING)

set(CMAKE_AUTOMOC ON)

#--------------------------------
# dependencies

include_directories(${PROJECT_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/src)

find_package(portaudio CONFIG REQUIRED)
find_package(SndFile CONFIG REQUIRED)
find_package(rtmidi CONFIG REQUIRED)

find_package(Qt6 COMPONENTS Core Gui Widgets Charts REQUIRED)

set(CLOVER_PATH ~/code/clover)
include(${CLOVER_PATH}/CMakeLists.Clover.txt)
target_link_libraries(Clover PUBLIC 
    portaudio
    SndFile::sndfile
    RtMidi::rtmidi
)


#--------------------------------
# projects

#----------------
# 01-qt-plot

# note: project setup basics can probably be a re-usable function that takes a string
file(GLOB_RECURSE SOURCE_LIST_01_qt_plot
    CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_LIST_DIR}/01-qt-plot/*.cpp
)

add_executable(01-qt-plot ${SOURCE_LIST_01_qt_plot})
target_link_libraries(01-qt-plot PUBLIC Clover Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Charts)