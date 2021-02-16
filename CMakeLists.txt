cmake_minimum_required(VERSION 3.16) # version can be different

project(DBI) #name of your project

include_directories(${CMAKE_SOURCE_DIR})

find_package(BISON REQUIRED)
BISON_TARGET(Parser Parser.y ${CMAKE_CURRENT_BINARY_DIR}/parser.cc)

find_package(FLEX REQUIRED)
FLEX_TARGET(Scanner Lexer.l ${CMAKE_CURRENT_BINARY_DIR}/lex.yy.c)

ADD_FLEX_BISON_DEPENDENCY(Scanner Parser)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

add_subdirectory(gtest) # add googletest subdirectory

include_directories(gtest/include) # this is so we can #include <gtest/gtest.h>

set(SOURCES DBFile.cc Heap.cc File.cc Record.cc Schema.cc Comparison.cc ComparisonEngine.cc TwoWayList.cc Pipe.cc BigQ.cc test.h DBFile.h Heap.h File.h Defs.h Errors.h Record.h Schema.h ParseTree.h Comparison.h ComparisonEngine.h TwoWayList.h Pipe.h BigQ.h ${BISON_Parser_OUTPUTS} ${FLEX_Scanner_OUTPUTS})

set (GTESTSRCS gTests.cc test.h ${SOURCES})

add_executable(gTests  ${GTESTSRCS})

target_link_libraries(gTests PRIVATE gtest)

set (TESTSRCS test.cc ${SOURCES})

add_executable(test.out  ${TESTSRCS})

set (MAINSRCS main.cc ${SOURCES})

add_executable(main ${MAINSRCS})

target_link_libraries(main Threads::Threads)

target_link_libraries(test.out Threads::Threads)


