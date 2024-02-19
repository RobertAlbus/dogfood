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


set(CLOVER_PATH ~/code/clover)
include(${CLOVER_PATH}/CMakeLists.Clover.txt)
target_link_libraries(Clover PUBLIC 
    portaudio
    SndFile::sndfile
    RtMidi::rtmidi
    )

#--------------------------------
# project setup

function(setup_project executable_name)
  file(GLOB_RECURSE SOURCE_LIST CONFIGURE_DEPENDS ${CMAKE_CURRENT_LIST_DIR}/${executable_name}/*.cpp)
  add_executable(${executable_name} ${SOURCE_LIST})
  target_link_libraries(${executable_name} PUBLIC ${ARGN})
endfunction()

find_package(Qt6 COMPONENTS Core Gui Widgets Charts REQUIRED)

#--------------------------------
# projects

setup_project(01-qt-plot Clover Qt6::Core Qt6::Gui Qt6::Widgets Qt6::Charts)
setup_project(02-simple-sounds Clover)
