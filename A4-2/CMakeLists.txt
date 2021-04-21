cmake_minimum_required(VERSION 2.6)
project(project4_2_test)
 
# define version of cpp used
set(CMAKE_CXX_STANDARD 11)

# Locate GTest
ADD_SUBDIRECTORY (gtest)
enable_testing()
include_directories(gtest/include)


# a4-2 project - main
add_executable(GTest gtest.cc Schema.cc Record.cc Comparison.cc ComparisonEngine.cc File.cc DBFile.cc GenericDBFile.cc HeapFile.cc SortedFile.cc Function.cc Pipe.cc BigQ.cc RelOp.cc Statistics.cc QueryPlan.cc DataDefinitionLanguage.cc ${BISON_Parser_OUTPUTS} ${FLEX_Scanner_OUTPUTS} ParseTree.h ParseFunc.h)
target_link_libraries(GTest ${GTEST_LIBRARIES} fl pthread gtest)