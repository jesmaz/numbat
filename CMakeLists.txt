cmake_minimum_required(VERSION 3.10 FATAL_ERROR)
project(numbat)


#option(debug "debug" ON)
enable_testing()

include(ExternalProject)
include(GoogleTest)
ExternalProject_Add (
	googletest
	GIT_REPOSITORY https://github.com/google/googletest.git
	GIT_TAG "release-1.8.0"
	PREFIX "${CMAKE_BINARY_DIR}/gtest"
	INSTALL_COMMAND ""
)
ExternalProject_Get_Property (googletest source_dir binary_dir)


execute_process(COMMAND llvm-config-5.0 --includedir OUTPUT_VARIABLE LLVM_INCLUDE_DIRS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND llvm-config-5.0 --libdir OUTPUT_VARIABLE LLVM_LIBRARY_DIRS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND llvm-config-5.0 --libs all OUTPUT_VARIABLE REQ_LLVM_LIBRARIES OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND llvm-config-5.0 --ldflags OUTPUT_VARIABLE LLVM_LINKER_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND llvm-config-5.0 --cppflags OUTPUT_VARIABLE LLVM_FLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)

include_directories(${LLVM_INCLUDE_DIRS} "${CMAKE_SOURCE_DIR}/include" "${source_dir}/googletest/include")
link_directories(${LLVM_LIBRARY_DIRS})

set(CMAKE_CXX_FLAGS "-Wall -std=c++14 -DUSE_LLVM=1 -g -pthread -march=native ${LLVM_FLAGS}")

set(sources ${sources} "src/file.cpp")
set(sources ${sources} "src/lexer.cpp")
set(sources ${sources} "src/ast/call.cpp")
set(sources ${sources} "src/ast/context.cpp")
set(sources ${sources} "src/ast/flowcontrol.cpp")
set(sources ${sources} "src/ast/function.cpp")
set(sources ${sources} "src/ast/memory.cpp")
set(sources ${sources} "src/ast/meta.cpp")
set(sources ${sources} "src/ast/node.cpp")
set(sources ${sources} "src/ast/passes/abstract.cpp")
set(sources ${sources} "src/ast/passes/analysis.cpp")
set(sources ${sources} "src/ast/passes/callgraph.cpp")
set(sources ${sources} "src/ast/passes/execute.cpp")
set(sources ${sources} "src/ast/passes/functionpurity.cpp")
set(sources ${sources} "src/ast/passes/funcutil.cpp")
set(sources ${sources} "src/ast/passes/identity.cpp")
set(sources ${sources} "src/ast/passes/metatagswizzle.cpp")
set(sources ${sources} "src/ast/passes/nir.cpp")
set(sources ${sources} "src/ast/passes/prunedead.cpp")
set(sources ${sources} "src/ast/passes/reflectpass.cpp")
set(sources ${sources} "src/ast/passes/resolve.cpp")
set(sources ${sources} "src/ast/passes/stackmachinepass.cpp")
set(sources ${sources} "src/ast/passes/typecastpass.cpp")
set(sources ${sources} "src/ast/passes/typeutil.cpp")
set(sources ${sources} "src/ast/sequence.cpp")
set(sources ${sources} "src/ast/type.cpp")
set(sources ${sources} "src/ast/typecast.cpp")
set(sources ${sources} "src/ast/variable.cpp")
set(sources ${sources} "src/codegen/llvm.cpp")
set(sources ${sources} "src/codegen/interpreter.cpp")
set(sources ${sources} "src/codegen/target.cpp")
set(sources ${sources} "src/nir/block.cpp")
set(sources ${sources} "src/nir/function.cpp")
set(sources ${sources} "src/nir/inst/add.cpp")
set(sources ${sources} "src/nir/inst/div.cpp")
set(sources ${sources} "src/nir/inst/jump.cpp")
set(sources ${sources} "src/nir/inst/less.cpp")
set(sources ${sources} "src/nir/inst/mul.cpp")
set(sources ${sources} "src/nir/inst/sub.cpp")
set(sources ${sources} "src/nir/instruction.cpp")
set(sources ${sources} "src/nir/module.cpp")
set(sources ${sources} "src/nir/scope.cpp")
set(sources ${sources} "src/nir/type.cpp")
set(sources ${sources} "src/nir/type/array.cpp")
set(sources ${sources} "src/nir/type/functionType.cpp")
set(sources ${sources} "src/nir/type/number.cpp")
set(sources ${sources} "src/nir/type/pointer.cpp")
set(sources ${sources} "src/nir/type/tuple.cpp")
set(sources ${sources} "src/nir/value.cpp")
set(sources ${sources} "src/parse/handparser.cpp")
set(sources ${sources} "src/parse/tree.cpp")
set(sources ${sources} "src/parse/tree/base.cpp")
set(sources ${sources} "src/parse/tree/braceinitialiser.cpp")
set(sources ${sources} "src/parse/tree/call.cpp")
set(sources ${sources} "src/parse/tree/error.cpp")
set(sources ${sources} "src/parse/tree/function.cpp")
set(sources ${sources} "src/parse/tree/ifelse.cpp")
set(sources ${sources} "src/parse/tree/identifier.cpp")
set(sources ${sources} "src/parse/tree/import.cpp")
set(sources ${sources} "src/parse/tree/index.cpp")
set(sources ${sources} "src/parse/tree/keyword.cpp")
set(sources ${sources} "src/parse/tree/list.cpp")
set(sources ${sources} "src/parse/tree/literal.cpp")
set(sources ${sources} "src/parse/tree/metaTags.cpp")
set(sources ${sources} "src/parse/tree/operator.cpp")
set(sources ${sources} "src/parse/tree/resolvescope.cpp")
set(sources ${sources} "src/parse/tree/return.cpp")
set(sources ${sources} "src/parse/tree/slice.cpp")
set(sources ${sources} "src/parse/tree/struct.cpp")
set(sources ${sources} "src/parse/tree/symbol.cpp")
set(sources ${sources} "src/parse/tree/typemodifier.cpp")
set(sources ${sources} "src/parse/tree/variable.cpp")
set(sources ${sources} "src/stackmachine/instruction.cpp")
set(sources ${sources} "src/utility/alphanumeric.cpp")
set(sources ${sources} "src/utility/config.cpp")
set(sources ${sources} "src/utility/literals.cpp")
set(sources ${sources} "src/utility/report.cpp")

add_library(numbat-static STATIC ${sources})

set(libs ${libs} ${REQ_LLVM_LIBRARIES} ${LLVM_LINKER_FLAGS} numbat-static dl z tinfo gmpxx gmp)

add_executable(numbat src/main.cpp src/numbatmain.cpp src/codegen/llvm.cpp)

target_link_libraries(numbat ${libs})


set(test_sources test/test.cpp test/arithmatic.cpp test/interpretertest.cpp test/lexer.cpp test/stackmachinetests.cpp)
set(test_libs ${libs} "${binary_dir}/googlemock/gtest/libgtest.a" "${binary_dir}/googlemock/gtest/libgtest_main.a")
add_executable(tests ${test_sources})
add_executable(tests-build ${test_sources})
add_dependencies(tests googletest)
add_dependencies(tests-build googletest)
target_link_libraries(tests ${test_libs})
target_link_libraries(tests-build ${test_libs})

add_custom_command(
	TARGET tests-build
	COMMENT "Run tests"
	POST_BUILD 
	WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test/"
	COMMAND tests
)


install(FILES numbat.xml DESTINATION /usr/share/katepart5/syntax/)
install(DIRECTORY "libnbt/" DESTINATION /usr/include/numbat)
install(TARGETS numbat numbat-static numbat-static RUNTIME DESTINATION bin LIBRARY DESTINATION lib ARCHIVE DESTINATION lib/static)
