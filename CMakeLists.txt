cmake_minimum_required(VERSION 2.6)
project(numbat)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

find_package(LLVM REQUIRED)


include_directories(${LLVM_INCLUDE_DIRS})
link_directories(${LLVM_LIBRARY_DIRS})
add_definitions(${LLVM_DEFINITIONS})

llvm_map_components_to_libraries(REQ_LLVM_LIBRARIES core jit native analysis ipo)

set(CMAKE_CXX_FLAGS " -Wall -std=c++11 -O3 ")

set(sources ${sources} src/ast.cpp)
set(sources ${sources} src/lexer.cpp)
set(sources ${sources} src/numbat.cpp)
set(sources ${sources} src/generator.cpp)
add_library(numbat-static STATIC ${sources})

set(libs ${libs} ${REQ_LLVM_LIBRARIES} numbat-static)

set(sources ${sources} src/main.cpp)

add_executable(numbat ${sources})
target_link_libraries(numbat ${libs})

install(TARGETS numbat numbat-static numbat-static RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib/static)
