cmake_minimum_required(VERSION 2.6)
project(numbat)



execute_process(COMMAND llvm-config --includedir OUTPUT_VARIABLE LLVM_INCLUDE_DIRS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND llvm-config --libdir OUTPUT_VARIABLE LLVM_LIBRARY_DIRS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND llvm-config --libs core jit native analysis ipo systemz OUTPUT_VARIABLE REQ_LLVM_LIBRARIES OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND llvm-config --ldflags OUTPUT_VARIABLE LLVM_LINKER_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)

#We only care about preprocessor flags
execute_process(COMMAND llvm-config --cppflags OUTPUT_VARIABLE LLVM_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)

include_directories(${LLVM_INCLUDE_DIRS} "${CMAKE_SOURCE_DIR}/include")
link_directories(${LLVM_LIBRARY_DIRS})

set(CMAKE_CXX_FLAGS "-Wall -std=c++11 -O3 ${LLVM_FLAGS}")



set(sources ${sources} src/ast.cpp)
set(sources ${sources} src/ast/astbody.cpp)
set(sources ${sources} src/ast/astcall.cpp)
set(sources ${sources} src/ast/astnumbatinstr.cpp)
set(sources ${sources} src/ast/functiondecleration.cpp)
set(sources ${sources} src/ast/numbatrawtype.cpp)
set(sources ${sources} src/ast/numbattype.cpp)
set(sources ${sources} src/ast/operatordecleration.cpp)
set(sources ${sources} src/lexer.cpp)
set(sources ${sources} src/numbat.cpp)
set(sources ${sources} src/generator.cpp)
add_library(numbat-static STATIC ${sources})

set(libs ${libs} ${REQ_LLVM_LIBRARIES} ${LLVM_LINKER_FLAGS} numbat-static)

set(sources ${sources} src/main.cpp)

add_executable(numbat ${sources})
target_link_libraries(numbat ${libs})

install(TARGETS numbat numbat-static numbat-static RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib/static)