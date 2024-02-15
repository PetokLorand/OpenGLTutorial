cmake_minimum_required(VERSION 3.20)

project(Tutorial)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(OpenGL REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(assimp REQUIRED)


set(WarningSettings -Wall -Wextra -Wpedantic -Werror -Wno-missing-field-initializers)
set(LanguageStandard -std=c++20)
set(Libraries OpenGL::GL GLEW::GLEW glfw glm::glm assimp)

file(GLOB_RECURSE Sources src/*.cpp)

add_executable(Debug ${Sources})
target_include_directories(Debug PRIVATE include)
target_compile_options(Debug PRIVATE ${LanguageStandard} ${WarningSettings} -O0 -g)
target_link_libraries(Debug PRIVATE ${Libraries})

add_executable(Release ${Sources})
target_include_directories(Release PRIVATE include)
target_compile_options(Release PRIVATE ${LanguageStandard} ${WarningSettings} -O3 -fno-rtti -flto=auto)
target_link_libraries(Release PRIVATE ${Libraries})
