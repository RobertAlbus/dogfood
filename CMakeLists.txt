find_program(SCCACHE sccache REQUIRED)

set(CMAKE_C_COMPILER_LAUNCHER ${SCCACHE})
set(CMAKE_CXX_COMPILER_LAUNCHER ${SCCACHE})

include(CMakeLists.vcpkg.txt)
cmake_minimum_required(VERSION 3.14)

project(Clover_Proof)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_PARALLEL_LEVEL 8 STRING)

add_compile_options(-Ofast -march=native)

#--------------------------------
# dependencies

include_directories(${PROJECT_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/include)
include_directories(${CMAKE_CURRENT_LIST_DIR}/src)

list(APPEND CLOVER_INCLUDE_DIRECTORIES
    ${PROJECT_BINARY_DIR}/vcpkg_installed/${VCPKG_TARGET_TRIPLET}/include
)
set(CLOVER_PATH ~/code/clover)
include(${CLOVER_PATH}/CMakeLists.Clover.txt)

# temporarily suppress clover warnings
target_compile_options(Clover PRIVATE -w)

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
set_target_properties(01-qt-plot PROPERTIES AUTOMOC ON)

setup_project(02-interface-v2 Clover)

setup_project(03-midi Clover)
target_compile_definitions(03-midi PRIVATE __LINUX_ALSA__)

setup_project(100-fun Clover)
