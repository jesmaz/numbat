cmake_minimum_required(VERSION 2.6)
project(numbat)


#option(debug "debug" ON)


execute_process(COMMAND llvm-config-3.5 --includedir OUTPUT_VARIABLE LLVM_INCLUDE_DIRS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND llvm-config-3.5 --libdir OUTPUT_VARIABLE LLVM_LIBRARY_DIRS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND llvm-config-3.5 --libs all-targets core jit native analysis ipo systemz OUTPUT_VARIABLE REQ_LLVM_LIBRARIES OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND llvm-config-3.5 --ldflags OUTPUT_VARIABLE LLVM_LINKER_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)

#We only care about preprocessor flags
execute_process(COMMAND llvm-config --cppflags OUTPUT_VARIABLE LLVM_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)

include_directories(${LLVM_INCLUDE_DIRS} "${CMAKE_SOURCE_DIR}/include")
link_directories(${LLVM_LIBRARY_DIRS})

set(CMAKE_CXX_FLAGS "-Wall -std=c++11 -g -pthread ${LLVM_FLAGS}")

set(sources ${sources} "src/ast.cpp")
set(sources ${sources} "src/cast.cpp")
set(sources ${sources} "src/core.cpp")
set(sources ${sources} "src/generator.cpp")
set(sources ${sources} "src/lexer.cpp")
set(sources ${sources} "src/module.cpp")
set(sources ${sources} "src/numbat.cpp")
set(sources ${sources} "src/parser.cpp")
set(sources ${sources} "src/position.cpp")
set(sources ${sources} "src/ast/astbase.cpp")
set(sources ${sources} "src/ast/astbody.cpp")
set(sources ${sources} "src/ast/astcall.cpp")
set(sources ${sources} "src/ast/astnumbatinstr.cpp")
set(sources ${sources} "src/ast/asttuple.cpp")
set(sources ${sources} "src/ast/asttuplecall.cpp")
set(sources ${sources} "src/ast/functiondecleration.cpp")
set(sources ${sources} "src/ast/numbatpointertype.cpp")
set(sources ${sources} "src/ast/numbatrawtype.cpp")
set(sources ${sources} "src/ast/numbatscope.cpp")
set(sources ${sources} "src/ast/numbattype.cpp")
set(sources ${sources} "src/ast/operatordecleration.cpp")
add_library(numbat-static STATIC ${sources})

set(libs ${libs} ${REQ_LLVM_LIBRARIES} ${LLVM_LINKER_FLAGS} numbat-static dl z tinfo)

add_executable(numbat ${sources} src/main.cpp)
add_executable(parsertest ${sources} test/parserTest.cpp)
target_link_libraries(numbat ${libs})
target_link_libraries(parsertest ${libs})

install(FILES numbat.xml DESTINATION /usr/share/kde4/apps/katepart/syntax/)
install(DIRECTORY "libnbt/" DESTINATION /usr/include/numbat)
install(TARGETS numbat numbat-static numbat-static RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib/static)