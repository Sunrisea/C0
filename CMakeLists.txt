cmake_minimum_required(VERSION 3.12)

project(cc0)

add_subdirectory(3rd_party/argparse)
add_subdirectory(3rd_party/fmt)

set(PROJECT_EXE ${PROJECT_NAME})
set(PROJECT_LIB "${PROJECT_NAME}_lib")

set(lib_src
	tokenizer/token.h
	tokenizer/tokenizer.h
	tokenizer/tokenizer.cpp
	tokenizer/utils.hpp
	error/error.h
	analyser/analyser.h
	analyser/analyser.cpp
	instruction/instruction.h
	symboltable/symboltable.cpp symboltable/symboltable.h)

set(main_src
	main.cpp
	fmts.hpp
		)

add_library(${PROJECT_LIB} ${lib_src})

add_executable(${PROJECT_EXE} ${main_src})

set_target_properties(${PROJECT_EXE} PROPERTIES
                      CXX_STANDARD 17
                      CXX_STANDARD_REQUIRED ON
)

set_target_properties(${PROJECT_LIB} PROPERTIES
                      CXX_STANDARD 17
                      CXX_STANDARD_REQUIRED ON
)

target_include_directories(${PROJECT_EXE} PRIVATE .)
target_include_directories(${PROJECT_LIB} PRIVATE .)



if(MSVC)
	target_compile_options(${PROJECT_EXE} PRIVATE /W3)
	target_compile_options(${PROJECT_LIB} PRIVATE /W3)
else()
	target_compile_options(${PROJECT_EXE} PRIVATE -Wall -Wextra -pedantic)
	target_compile_options(${PROJECT_LIB} PRIVATE -Wall -Wextra -pedantic)
endif()

# This will add the include path, respectively.
# target_link_libraries(${PROJECT_LIB} fmt::fmt)
target_link_libraries(${PROJECT_EXE} ${PROJECT_LIB} argparse fmt::fmt)

# For tests
