cmake_minimum_required(VERSION 3.8.0)
project(VoxEngine_Entity_Creator VERSION 0.1.0 LANGUAGES C CXX)
set(CMAKE_CXX_STANDARD 20)

# Path to the build number file
set(BUILD_NUMBER_FILE "${CMAKE_SOURCE_DIR}/build_number.txt")

function(read_build_number)
    if(EXISTS "${BUILD_NUMBER_FILE}")
        file(READ "${BUILD_NUMBER_FILE}" BUILD_NUMBER)
        string(STRIP "${BUILD_NUMBER}" BUILD_NUMBER)
    else()
        set(BUILD_NUMBER "0")
    endif()
    set(BUILD_NUMBER "${BUILD_NUMBER}" PARENT_SCOPE)
endfunction()

function(write_build_number BUILD_NUMBER)
    file(WRITE "${BUILD_NUMBER_FILE}" "${BUILD_NUMBER}")
endfunction()

read_build_number()
math(EXPR NEW_BUILD_NUMBER "${BUILD_NUMBER} + 1")
write_build_number(${NEW_BUILD_NUMBER})
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}-${NEW_BUILD_NUMBER}")

# --- Data ---
file(
  COPY ${CMAKE_CURRENT_SOURCE_DIR}/data
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
)
configure_file (
  "${PROJECT_SOURCE_DIR}/src/cmake_defines.raw.hh"
  "${PROJECT_BINARY_DIR}/cmake_defines.hh"
)
include_directories("${PROJECT_BINARY_DIR}")

# --- Libraries ---
# YAML
include(FetchContent)
FetchContent_Declare(
  yaml-cpp
  GIT_REPOSITORY "https://github.com/jbeder/yaml-cpp.git"
  GIT_TAG "0.8.0"
)
FetchContent_GetProperties(yaml-cpp)
if(NOT yaml-cpp_POPULATED)
  message(STATUS "Fetching yaml-cpp...")
  FetchContent_Populate(yaml-cpp)
  add_subdirectory(${yaml-cpp_SOURCE_DIR} ${yaml-cpp_BINARY_DIR})
endif()
# GLFW
FetchContent_Declare(
  glfw
  GIT_REPOSITORY "https://github.com/glfw/glfw.git"
  GIT_TAG "3.4"
)
FetchContent_MakeAvailable(glfw)
# GLAD
FetchContent_Declare(
  glad
  GIT_REPOSITORY "https://github.com/DawidLokiec/glad.git"
  GIT_TAG "master"
)
FetchContent_MakeAvailable(glad)
# GLM
FetchContent_Declare(
  glm
  GIT_REPOSITORY "https://github.com/g-truc/glm.git"
  GIT_TAG "1.0.1"
)
FetchContent_MakeAvailable(glm)
# ImGui
set(IMGUI_PATH "imgui")
file(GLOB IMGUI_SOURCES ${IMGUI_PATH}/*.cpp ${IMGUI_PATH}/backends/imgui_impl_glfw.cpp ${IMGUI_PATH}/backends/imgui_impl_opengl3.cpp)
add_library("imgui" STATIC ${IMGUI_SOURCES})
target_include_directories("imgui" PUBLIC ${IMGUI_PATH} ${IMGUI_PATH}/backends)
target_link_libraries("imgui" glfw)

# --- Executables ---
file(GLOB Sources
  src/*.cc
  src/engine/*.cc
  src/utils/*.cc
)
add_executable("${PROJECT_NAME}" ${Sources})
target_link_libraries(${PROJECT_NAME}
  yaml-cpp::yaml-cpp glfw glad glm imgui
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPACK)
