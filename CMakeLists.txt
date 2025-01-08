cmake_minimum_required(VERSION 3.11.0)
project(VulkanProject)


# Set the output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
# add single files header under /lib
include_directories(${PROJECT_SOURCE_DIR}/lib)
# Set the path to your Vulkan SDK
if (DEFINED $ENV{VK_SDK_PATH})
    set(Vulkan_INCLUDE_DIRS "$ENV{VK_SDK_PATH}/Include")
    set(Vulkan_LIBRARIES "$ENV{VK_SDK_PATH}/Lib")
    set(Vulkan_FOUND "True")
else()
    find_package(Vulkan REQUIRED) # throws error if could not find Vulkan
    message(STATUS "Found Vulkan: $ENV{VK_SDK_PATH}")
endif()
# Set the path to your GLFW library
set(GLFW_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}/lib/glfw/include")
set(GLFW_LIBRARIES "${PROJECT_SOURCE_DIR}/lib/glfw/lib-vc2022")
set(GLM_LIBRARIES "${PROJECT_SOURCE_DIR}/lib/glm")
set(IMGUI_LIBRARIES "${PROJECT_SOURCE_DIR}/include/imgui")
set(IMGUI_BK_LIBRARIES "${PROJECT_SOURCE_DIR}/include/imgui/backends")

set(CMAKE_CXX_STANDARD 20)  # using c++ version 20
set(CMAKE_CXX_STANDARD_REQUIRED True)
# Source files
file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.hpp" "src/*.h")
file(GLOB_RECURSE LIB "lib/tiny_obj_loader.h")
#include_directories(${CMAKE_SOURCE_DIR}/lib)
#file(GLOB_RECURSE INCLUDE "include/imgui/*.cpp" "include/imgui/*.h"  "include/imgui/backends/*.h" "include/imgui/backends/*.cpp")
#file(GLOB_RECURSE INCLUDE "include/imgui/*.cpp" )


add_library(imgui
	${IMGUI_LIBRARIES}/imgui.cpp
	${IMGUI_LIBRARIES}/imgui_draw.cpp
	${IMGUI_LIBRARIES}/imgui_tables.cpp
	${IMGUI_LIBRARIES}/imgui_widgets.cpp
    ${IMGUI_LIBRARIES}/backends/imgui_impl_vulkan.cpp
    ${IMGUI_LIBRARIES}/backends/imgui_impl_glfw.cpp
)

# Add executable
add_executable(${PROJECT_NAME} ${SOURCES} ${imgui} ${LIB})
target_include_directories(${PROJECT_NAME} PUBLIC 
    ${Vulkan_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIRS})
target_link_directories(${PROJECT_NAME} PUBLIC
    ${Vulkan_LIBRARIES}
    ${GLFW_LIBRARIES}
)
# Add Imgui-docking
include_directories(${IMGUI_LIBRARIES} ${IMGUI_BK_LIBRARIES} ..)
target_include_directories(${PROJECT_NAME} PUBLIC ${IMGUI_LIBRARIES} ${IMGUI_BK_LIBRARIES})

target_include_directories(${PROJECT_NAME} PRIVATE imgui ${IMGUI_LIBRARIES}/backends)
#target_include_directories(imgui PUBLIC ${IMGUI_LIBRARIES})

# Link libraries
target_link_libraries(${PROJECT_NAME} glfw3  Vulkan::Vulkan imgui)

# Copy folder to build destination 
add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD 
    COMMAND ${CMAKE_COMMAND} -E copy_directory 
    "${CMAKE_SOURCE_DIR}./assets/" 
    "$<TARGET_FILE_DIR:${PROJECT_NAME}>/assets")


message(STATUS "Build directory: ${CMAKE_SOURCE_DIR}")